#pragma once

#include <cp/graph/scc.hpp>

#include <cassert>
#include <limits>
#include <vector>

namespace cp {

class TwoSat {
public:
  explicit TwoSat(int n = 0) : n_(n), graph_(graph_size(n)) {
    answer_.resize(n);
  }

  // Adds (x_i == f) OR (x_j == g).
  void add_clause(int i, bool f, int j, bool g) {
    assert(0 <= i && i < n_);
    assert(0 <= j && j < n_);
    graph_.add_edge(2 * i + !f, 2 * j + g);
    graph_.add_edge(2 * j + !g, 2 * i + f);
  }

  bool satisfiable() {
    auto [count, ids] = graph_.scc_ids();
    (void)count;
    for (int i = 0; i < n_; ++i) {
      if (ids[2 * i] == ids[2 * i + 1])
        return false;
      answer_[i] = ids[2 * i] < ids[2 * i + 1];
    }
    return true;
  }

  std::vector<bool> answer() const { return answer_; }

private:
  static int graph_size(int n) {
    assert(0 <= n && n <= std::numeric_limits<int>::max() / 2);
    return 2 * n;
  }
  int n_;
  SccGraph graph_;
  std::vector<bool> answer_;
};

} // namespace cp
