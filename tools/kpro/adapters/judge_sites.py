from __future__ import annotations

import json
import re
from collections.abc import Callable
from dataclasses import dataclass
from urllib.parse import urlsplit
from urllib.request import Request, urlopen

from kpro.core import KproError


@dataclass(frozen=True)
class Problem:
    id: str
    title: str
    url: str


@dataclass(frozen=True)
class Contest:
    id: str
    platform: str
    url: str
    problems: list[Problem]


@dataclass(frozen=True)
class ContestReference:
    platform: str
    remote_id: str
    url: str

    @property
    def id(self) -> str:
        return self.remote_id if self.platform == "atcoder" else f"{self.platform}-{self.remote_id}"


@dataclass(frozen=True)
class ProblemReference:
    platform: str
    id: str
    url: str
    contest_id: str | None = None


HOSTS = {"atcoder.jp": "atcoder", "codeforces.com": "codeforces", "yukicoder.me": "yukicoder"}
GetMetadata = Callable[[str, str], dict]


def _number(value: str) -> str:
    try:
        if not re.fullmatch(r"[0-9]+", value):
            raise ValueError("Non-numeric id")
        number = int(value)
        if number <= 0:
            raise ValueError("Non-positive id")
        return str(number)
    except ValueError as exc:
        raise KproError("Judge ids must be positive integers of a supported size.") from exc


def _url_parts(url: str) -> tuple[str, str]:
    try:
        parts = urlsplit(url)
    except ValueError as exc:
        raise KproError("Invalid judge URL.") from exc
    if (
        parts.scheme not in {"https", "http"}
        or parts.netloc not in HOSTS
        or parts.query
        or parts.fragment
    ):
        raise KproError("Use an AtCoder, Codeforces or yukicoder URL without query or fragment.")
    return HOSTS[parts.netloc], parts.path.rstrip("/")


def resolve_contest(identifier: str) -> ContestReference:
    if identifier.isdecimal():
        raise KproError("Numeric contest ids need a site: codeforces:<id> or yukicoder:<id>.")
    short = re.fullmatch(r"(?:(atcoder|codeforces|yukicoder):)?([a-z0-9_-]+)", identifier)
    if short:
        platform, remote_id = short[1] or "atcoder", short[2]
    else:
        platform, path = _url_parts(identifier)
        prefix = "/contest/" if platform == "codeforces" else "/contests/"
        if not path.startswith(prefix):
            raise KproError(
                "Use a contest URL. Codeforces Gym and group contests are not supported."
            )
        remote_id = path.removeprefix(prefix)
    if platform == "atcoder":
        if not re.fullmatch(r"[a-z0-9_-]+", remote_id):
            raise KproError("Invalid AtCoder contest id.")
        url = f"https://atcoder.jp/contests/{remote_id}"
    else:
        remote_id = _number(remote_id)
        url = (
            f"https://codeforces.com/contest/{remote_id}"
            if platform == "codeforces"
            else f"https://yukicoder.me/contests/{remote_id}"
        )
    return ContestReference(platform, remote_id, url)


def resolve_problem(url: str) -> ProblemReference:
    platform, path = _url_parts(url)
    if platform == "atcoder":
        match = re.fullmatch(r"/contests/([a-z0-9_-]+)/tasks/([a-zA-Z0-9_-]+)", path)
        if match:
            return ProblemReference(
                platform, match[2].lower(), f"https://atcoder.jp{path}", match[1]
            )
    elif platform == "codeforces":
        match = re.fullmatch(
            r"/(?:contest/([0-9]+)/problem|problemset/problem/([0-9]+))/([a-zA-Z][a-zA-Z0-9]*)",
            path,
        )
        if match:
            contest_id = _number(match[1] or match[2])
            index = match[3].upper()
            return ProblemReference(
                platform,
                index.lower(),
                f"https://codeforces.com/contest/{contest_id}/problem/{index}",
                contest_id,
            )
    else:
        match = re.fullmatch(r"/problems/(?:(no)/)?([0-9]+)", path)
        if match:
            problem_id = _number(match[2])
            kind = match[1] or "id"
            suffix = f"no/{problem_id}" if kind == "no" else problem_id
            return ProblemReference(
                platform, f"{kind}-{problem_id}", f"https://yukicoder.me/problems/{suffix}"
            )
    raise KproError("Unsupported problem URL. Codeforces Gym and group problems are not supported.")


def _text(value: object) -> str:
    if not isinstance(value, str) or not value.strip():
        raise ValueError("Expected nonempty string")
    return value


class OjSite:
    supports_submission = True

    def __init__(self, platform: str):
        self.platform = platform

    def fetch_contest(self, reference: ContestReference, get: GetMetadata) -> Contest:
        data = get("get-contest", reference.url)
        try:
            if not isinstance(data["problems"], list):
                raise ValueError("Expected problem list")
            problems = []
            for item in data["problems"]:
                target = resolve_problem(_text(item["url"]))
                if target.platform != reference.platform or (
                    target.contest_id is not None and target.contest_id != reference.remote_id
                ):
                    raise ValueError("Problem belongs to another contest/site")
                alphabet = item.get("context", {}).get("alphabet", target.id)
                problem_id = _text(alphabet).lower()
                if not re.fullmatch(r"[a-z0-9_-]+", problem_id):
                    raise ValueError("Invalid problem id")
                problems.append(Problem(problem_id, _text(item["name"]), target.url))
            if len({problem.id for problem in problems}) != len(problems):
                raise ValueError("Duplicate problem ids")
        except (KeyError, TypeError, AttributeError, ValueError, KproError) as exc:
            raise KproError(f"Unexpected oj-api {self.platform} contest metadata.") from exc
        return Contest(reference.id, reference.platform, reference.url, problems)

    def fetch_problem(self, reference: ProblemReference, get: GetMetadata) -> Problem:
        data = get("get-problem", reference.url)
        try:
            target = resolve_problem(_text(data["url"]))
            if target != reference:
                raise ValueError("Unexpected problem URL")
            # yukicoder's get-problem has no title/alphabet; keep its URL-derived id.
            problem_id = _text(data.get("context", {}).get("alphabet", reference.id)).lower()
            if not re.fullmatch(r"[a-z0-9_-]+", problem_id):
                raise ValueError("Invalid problem id")
            title = _text(data.get("name", reference.id))
        except (KeyError, TypeError, AttributeError, ValueError, KproError) as exc:
            raise KproError(f"Unexpected oj-api {self.platform} problem metadata.") from exc
        return Problem(problem_id, title, reference.url)


class CodeforcesSite:
    supports_submission = False

    def fetch_contest(self, reference: ContestReference, get: GetMetadata) -> Contest:
        # The locked oj-api adds from/count, which public regular contests no longer accept.
        # Do not attach oj cookies or API credentials to this anonymous public endpoint.
        url = f"https://codeforces.com/api/contest.standings?contestId={reference.remote_id}"
        request = Request(url, headers={"Accept": "application/json", "User-Agent": "kpro"})
        try:
            with urlopen(request, timeout=120) as response:
                data = json.load(response)
        except (OSError, ValueError) as exc:
            raise KproError(
                "Could not fetch Codeforces public contest metadata. Check URL/network."
            ) from exc
        try:
            if data["status"] != "OK" or not isinstance(data["result"]["problems"], list):
                raise ValueError("API failed")
            result = data["result"]
            if result["contest"]["id"] != int(reference.remote_id):
                raise ValueError("Unexpected contest id")
            problems = []
            for item in result["problems"]:
                index = _text(item["index"])
                target = resolve_problem(f"{reference.url}/problem/{index}")
                if item.get("contestId", int(reference.remote_id)) != int(reference.remote_id):
                    raise ValueError("Unexpected problem contest id")
                problems.append(Problem(target.id, _text(item["name"]), target.url))
            if not problems or len({problem.id for problem in problems}) != len(problems):
                raise ValueError("Empty/duplicate problems")
        except (KeyError, TypeError, ValueError, KproError) as exc:
            raise KproError(
                "Codeforces returned unavailable or invalid contest metadata. "
                "Only public regular contests with visible standings are supported."
            ) from exc
        return Contest(reference.id, reference.platform, reference.url, problems)

    def fetch_problem(self, reference: ProblemReference, get: GetMetadata) -> Problem:
        contest = self.fetch_contest(resolve_contest(f"codeforces:{reference.contest_id}"), get)
        for problem in contest.problems:
            if problem.id == reference.id:
                return problem
        raise KproError("Codeforces problem is not in the public contest problem list.")


SITES = {
    "atcoder": OjSite("atcoder"),
    "codeforces": CodeforcesSite(),
    "yukicoder": OjSite("yukicoder"),
}
