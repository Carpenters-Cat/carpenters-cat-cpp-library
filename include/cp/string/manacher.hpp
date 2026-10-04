#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace cp {
class Manacher {
public:
  explicit Manacher(std::string_view text) {
    if (text.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
      throw std::length_error("Manacher input length exceeds INT_MAX");
    int n = static_cast<int>(text.size());
    odd_.assign(text.size(), 0);
    even_.assign(text.size() + 1, 0);
    for (int i = 0, left = 0, right = -1; i < n; ++i) {
      int radius =
          i > right ? 1 : std::min(odd_[left + (right - i)], right - i + 1);
      while (i - radius >= 0 && i + radius < n &&
             text[i - radius] == text[i + radius])
        ++radius;
      odd_[i] = radius;
      if (i + radius - 1 > right) {
        left = i - radius + 1;
        right = i + radius - 1;
      }
    }
    for (int i = 0, left = 0, right = -1; i < n; ++i) {
      int radius = i > right
                       ? 0
                       : std::min(even_[left + (right - i) + 1], right - i + 1);
      while (i - radius - 1 >= 0 && i + radius < n &&
             text[i - radius - 1] == text[i + radius])
        ++radius;
      even_[i] = radius;
      if (i + radius - 1 > right) {
        left = i - radius;
        right = i + radius - 1;
      }
    }
  }
  int size() const { return static_cast<int>(odd_.size()); }
  const std::vector<int> &odd_radii() const { return odd_; }
  const std::vector<int> &even_radii() const { return even_; }
  int odd_radius(int center) const {
    assert(0 <= center && center < size());
    return odd_[center];
  }
  int even_radius(int gap) const {
    assert(0 <= gap && gap <= size());
    return even_[gap];
  }
  std::pair<int, int> odd_interval(int center) const {
    int radius = odd_radius(center);
    return {center - radius + 1, center + radius};
  }
  std::pair<int, int> even_interval(int gap) const {
    int radius = even_radius(gap);
    return {gap - radius, gap + radius};
  }
  bool is_palindrome(int left, int right) const {
    assert(0 <= left && left <= right && right <= size());
    int length = right - left;
    if (length == 0)
      return true;
    int center = left + length / 2;
    return length % 2 ? odd_[center] >= length / 2 + 1
                      : even_[center] >= length / 2;
  }

private:
  std::vector<int> odd_, even_;
};
} // namespace cp
