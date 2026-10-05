// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/two_sat.hpp>
int main() {
  cp::TwoSat empty;
  assert(empty.satisfiable() && empty.answer().empty());
  cp::TwoSat sat(2);
  sat.add_clause(0, true, 0, true);
  sat.add_clause(1, false, 1, false);
  assert(sat.satisfiable());
  assert(sat.answer()[0] && !sat.answer()[1]);
  assert(sat.satisfiable());
  sat.add_clause(0, false, 0, false);
  assert(!sat.satisfiable());
  cp::TwoSat tautology(1);
  tautology.add_clause(0, true, 0, false);
  assert(tautology.satisfiable());
}
