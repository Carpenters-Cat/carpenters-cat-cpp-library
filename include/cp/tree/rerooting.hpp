#pragma once
#include <cp/tree/tree.hpp>
#include <type_traits>

namespace cp {
// Neighbor order is the original Tree::neighbors(v) order, even when excluding
// one neighbor. combine is associative with a two-sided identity.
template <class State, class Combine, class Lift, class Finish>
auto rerooting(const Tree &tree, State identity, Combine combine, Lift lift,
               Finish finish)
    -> std::vector<
        std::remove_cvref_t<std::invoke_result_t<Finish, const State &, int>>> {
  using Result =
      std::remove_cvref_t<std::invoke_result_t<Finish, const State &, int>>;
  RootedTree rooted(tree);
  int n = tree.size();
  std::vector<State> down(n, identity), from_parent(n, identity);
  for (auto it = rooted.order.rbegin(); it != rooted.order.rend(); ++it) {
    int v = *it;
    if (rooted.parent[v] < 0)
      continue;
    State aggregate = identity;
    for (auto arc : tree.neighbors(v))
      if (arc.to != rooted.parent[v])
        aggregate = combine(aggregate, down[arc.to]);
    Result value = finish(std::as_const(aggregate), v);
    down[v] =
        lift(std::as_const(value), v, rooted.parent[v], *rooted.parent_edge[v]);
  }
  std::vector<std::optional<Result>> temporary(n);
  for (int v : rooted.order) {
    const auto &neighbors = tree.neighbors(v);
    std::size_t degree = neighbors.size();
    std::vector<State> prefix(degree + 1, identity),
        suffix(degree + 1, identity);
    auto incoming = [&](std::size_t i) -> const State & {
      return neighbors[i].to == rooted.parent[v] ? from_parent[v]
                                                 : down[neighbors[i].to];
    };
    for (std::size_t i = 0; i < degree; ++i)
      prefix[i + 1] = combine(prefix[i], incoming(i));
    for (std::size_t i = degree; i--;)
      suffix[i] = combine(incoming(i), suffix[i + 1]);
    temporary[v].emplace(finish(std::as_const(prefix.back()), v));
    for (std::size_t i = 0; i < degree; ++i)
      if (neighbors[i].to != rooted.parent[v]) {
        State excluded = combine(prefix[i], suffix[i + 1]);
        Result value = finish(std::as_const(excluded), v);
        from_parent[neighbors[i].to] = lift(
            std::as_const(value), v, neighbors[i].to, neighbors[i].edge_id);
      }
  }
  std::vector<Result> result;
  result.reserve(n);
  for (auto &value : temporary)
    result.push_back(std::move(*value));
  return result;
}
} // namespace cp
