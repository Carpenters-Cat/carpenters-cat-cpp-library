// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/linear_recurrence.hpp>
#include <random>
int main() {
    using M = cp::modint998244353;
    using F = cp::FPS<M>;
    std::mt19937 rng(765432);
    for (int t = 0; t < 150; t++) {
        int d = 1 + rng() % 30;
        std::vector<M> coeff(d), s(250);
        for (auto &x : coeff)
            x = rng();
        for (int i = 0; i < d; i++)
            s[i] = rng();
        for (int i = d; i < 250; i++)
            for (int j = 0; j < d; j++)
                s[i] += coeff[j] * s[i - j - 1];
        std::vector<M> prefix(s.begin(), s.begin() + 2 * d), initial(s.begin(), s.begin() + d);
        auto inferred = cp::berlekamp_massey(prefix);
        for (int i = inferred.size(); i < 250; i++) {
            M sum = 0;
            for (int j = 0; j < static_cast<int>(inferred.size()); j++)
                sum += inferred[j] * s[i - j - 1];
            assert(sum == s[i]);
        }
        for (int k : {0, d - 1, 2 * d, 249}) {
            assert(cp::linear_recurrence_nth(initial, coeff, k) == s[k]);
            assert(cp::bmbm(prefix, k) == s[k]);
        }
        F numerator(d + 3), denominator(d + 1);
        for (auto &x : numerator)
            x = rng();
        for (auto &x : denominator)
            x = rng();
        denominator[0] = 1;
        auto direct = numerator.divide(denominator, 100);
        for (int k : {0, 1, 10, 99})
            assert(cp::bostan_mori(numerator, denominator, k) == direct[k]);
    }
}
