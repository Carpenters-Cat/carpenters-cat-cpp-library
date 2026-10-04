#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace cp {

struct RollingHashParameters {
  static constexpr std::uint32_t modulus_first = 1000000007;
  static constexpr std::uint32_t modulus_second = 1000000009;
  std::uint32_t base_first, base_second;

  static constexpr RollingHashParameters from_seed(std::uint64_t seed = 0) {
    auto next = [&]() {
      seed += 0x9e3779b97f4a7c15ULL;
      std::uint64_t x = seed;
      x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
      x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
      return x ^ (x >> 31);
    };
    return {std::uint32_t(257 + next() % (modulus_first - 258)),
            std::uint32_t(257 + next() % (modulus_second - 258))};
  }
  void validate() const {
    if (base_first < 257 || base_first >= modulus_first - 1 ||
        base_second < 257 || base_second >= modulus_second - 1)
      throw std::invalid_argument(
          "Rolling hash bases must be in [257, modulus-2]");
  }
  bool operator==(const RollingHashParameters &) const = default;
};

class RollingHash;

class RollingHashValue {
public:
  explicit RollingHashValue(
      RollingHashParameters parameters = RollingHashParameters::from_seed())
      : parameters_(parameters) {
    parameters_.validate();
  }
  std::size_t length() const { return length_; }
  RollingHashParameters parameters() const { return parameters_; }
  std::pair<std::uint32_t, std::uint32_t> fingerprint() const {
    return {first_, second_};
  }
  bool compatible(const RollingHashValue &other) const {
    return parameters_ == other.parameters_;
  }
  bool operator==(const RollingHashValue &other) const {
    return parameters_ == other.parameters_ && length_ == other.length_ &&
           first_ == other.first_ && second_ == other.second_;
  }

private:
  friend class RollingHash;
  RollingHashValue(std::uint32_t first, std::uint32_t second,
                   std::uint32_t power_first, std::uint32_t power_second,
                   std::size_t length, RollingHashParameters parameters)
      : parameters_(parameters), first_(first), second_(second),
        power_first_(power_first), power_second_(power_second),
        length_(length) {}
  RollingHashParameters parameters_;
  std::uint32_t first_ = 0, second_ = 0, power_first_ = 1, power_second_ = 1;
  std::size_t length_ = 0;
};

class RollingHash {
public:
  explicit RollingHash(
      std::string_view text,
      RollingHashParameters parameters = RollingHashParameters::from_seed())
      : parameters_(parameters) {
    parameters_.validate();
    if (text.size() == std::numeric_limits<std::size_t>::max())
      throw std::length_error("Rolling hash input length cannot be SIZE_MAX");
    prefix_.assign(text.size() + 1, {0, 0});
    powers_.assign(text.size() + 1, {1, 1});
    for (std::size_t i = 0; i < text.size(); ++i) {
      std::uint32_t symbol = static_cast<unsigned char>(text[i]) + 1U;
      prefix_[i + 1] = {
          (multiply(prefix_[i][0], parameters_.base_first, mod_first) +
           symbol) %
              mod_first,
          (multiply(prefix_[i][1], parameters_.base_second, mod_second) +
           symbol) %
              mod_second};
      powers_[i + 1] = {
          multiply(powers_[i][0], parameters_.base_first, mod_first),
          multiply(powers_[i][1], parameters_.base_second, mod_second)};
    }
  }
  RollingHash(std::string_view text, std::uint64_t seed)
      : RollingHash(text, RollingHashParameters::from_seed(seed)) {}
  std::size_t size() const { return prefix_.size() - 1; }
  RollingHashParameters parameters() const { return parameters_; }
  RollingHashValue get(std::size_t left, std::size_t right) const {
    assert(left <= right && right <= size());
    std::size_t length = right - left;
    return {(prefix_[right][0] + mod_first -
             multiply(prefix_[left][0], powers_[length][0], mod_first)) %
                mod_first,
            (prefix_[right][1] + mod_second -
             multiply(prefix_[left][1], powers_[length][1], mod_second)) %
                mod_second,
            powers_[length][0],
            powers_[length][1],
            length,
            parameters_};
  }
  RollingHashValue whole() const { return get(0, size()); }
  static RollingHashValue concat(const RollingHashValue &left,
                                 const RollingHashValue &right) {
    if (!left.compatible(right))
      throw std::invalid_argument("Rolling hash bases differ");
    if (right.length_ > std::numeric_limits<std::size_t>::max() - left.length_)
      throw std::length_error(
          "Concatenated rolling hash length overflows size_t");
    return {
        (multiply(left.first_, right.power_first_, mod_first) + right.first_) %
            mod_first,
        (multiply(left.second_, right.power_second_, mod_second) +
         right.second_) %
            mod_second,
        multiply(left.power_first_, right.power_first_, mod_first),
        multiply(left.power_second_, right.power_second_, mod_second),
        left.length_ + right.length_,
        left.parameters_};
  }
  bool equal(std::size_t left, std::size_t right, const RollingHash &other,
             std::size_t other_left, std::size_t other_right) const {
    require_compatible(other);
    return get(left, right) == other.get(other_left, other_right);
  }
  std::size_t
  lcp(const RollingHash &other, std::size_t first_pos = 0,
      std::size_t second_pos = 0,
      std::size_t limit = std::numeric_limits<std::size_t>::max()) const {
    assert(first_pos <= size() && second_pos <= other.size());
    require_compatible(other);
    std::size_t low = 0, high = std::min({size() - first_pos,
                                          other.size() - second_pos, limit});
    while (low < high) {
      std::size_t middle = low + (high - low + 1) / 2;
      if (get(first_pos, first_pos + middle) ==
          other.get(second_pos, second_pos + middle))
        low = middle;
      else
        high = middle - 1;
    }
    return low;
  }

private:
  static constexpr auto mod_first = RollingHashParameters::modulus_first;
  static constexpr auto mod_second = RollingHashParameters::modulus_second;
  static std::uint32_t multiply(std::uint32_t a, std::uint32_t b,
                                std::uint32_t modulus) {
    return static_cast<std::uint32_t>(std::uint64_t(a) * b % modulus);
  }
  void require_compatible(const RollingHash &other) const {
    if (parameters_ != other.parameters_)
      throw std::invalid_argument("Rolling hash bases differ");
  }
  RollingHashParameters parameters_;
  std::vector<std::array<std::uint32_t, 2>> prefix_, powers_;
};
} // namespace cp
