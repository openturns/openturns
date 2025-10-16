#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

alpha = 0.05
beta = 0.95
algo = otexp.QuantileConfidence(alpha, beta)
print(algo)
print(repr(algo))
algo.setAlpha(alpha)
algo.setBeta(beta)
assert algo.getAlpha() == alpha
assert algo.getBeta() == beta

# lower rank
print("lower rank ...")
k_ref = {
    59: 0,
    93: 1,
    124: 2,
    153: 3,
    181: 4,
    208: 5,
    311: 9,
    410: 13,
    506: 17,
    717: 26,
    809: 30,
    900: 34,
    1036: 40,
}
for i in range(991, 1013):
    k_ref[i] = 38
k_ref[1013] = 39
for n in k_ref.keys():
    ll = algo.computeUnilateralRank(n, True)
    p = ot.Binomial(n, alpha).computeComplementaryCDF(ll)
    print(f"n={n} l={ll} ref={k_ref[n]} p={p}")
    assert ll == k_ref[n]
    assert p >= beta

# upper rank
print("upper rank ...")
algo.setAlpha(0.95)
k_ref = {
    59: 58,
    93: 91,
    124: 121,
    153: 149,
    181: 176,
    208: 202,
    311: 301,
    410: 396,
    506: 488,
    601: 579,
    717: 690,
    809: 778,
    900: 865,
    1013: 973,
    1036: 995,
}
for n in k_ref.keys():
    u = algo.computeUnilateralRank(n)
    p = ot.Binomial(n, alpha).computeCDF(u)
    print(f"n={n} u={u} ref={k_ref[n]} p={p}")
    assert u == k_ref[n]
    assert p >= beta

# bilateral ranks
print("bilateral ranks ...")
algo.setAlpha(0.05)
algo = otexp.QuantileConfidence(alpha, beta)
k_ref = {
    59: (0, 9),
    60: (0, 8),
    70: (0, 8),
    80: (0, 8),
    90: (0, 8),
    100: (1, 10),
    150: (1, 12),
    200: (2, 15),
    250: (3, 18),
    300: (4, 21),
    400: (11, 28),
    500: (12, 33),
    600: (19, 40),
    700: (25, 50),
    800: (22, 50),
    900: (34, 69),
    1000: (36, 63),
    10000: (437, 536),
    100000: (4879, 5160),
}
for n in k_ref.keys():
    k1, k2 = algo.computeBilateralRank(n)
    binomial = ot.Binomial(n, alpha)
    p = binomial.computeProbability(ot.Interval(k1, k2))
    print(f"n={n} k1={k1} k2={k2} p={p:.6f}")
    assert (k1, k2) == k_ref[n]
    assert p >= beta

    # we can also make sure by exploring all combinations (costly)
    if n > 100:
        continue
    bestP = 2
    for i1 in range(n):
        for i2 in range(i1, n):
            # we compute P(k1<X<=k2)=P(k1+1<=X<=k2), CDF(k2)-CDF(k1) also works
            p = binomial.computeProbability(ot.Interval(i1 + 1, i2))
            if (p >= beta) and (p < bestP):
                bestP = p
                bestK1K2 = i1, i2
    # print(n, bestK1K2, bestP)
    assert (k1, k2) == bestK1K2

# validate confidence intervals by probability estimation
n = 100
distribution = ot.Gumbel()
nreps = 3000
p1 = 0.0
p2 = 0.0
p3 = 0.0
for r in range(nreps):
    sample = distribution.getSample(n)
    interval1 = algo.computeUnilateralConfidenceInterval(sample)
    interval1_bis, coverage1 = algo.computeUnilateralConfidenceIntervalWithCoverage(
        sample
    )
    interval2 = algo.computeUnilateralConfidenceInterval(sample, True)
    interval2_bis, coverage2 = algo.computeUnilateralConfidenceIntervalWithCoverage(
        sample, True
    )
    interval3 = algo.computeBilateralConfidenceInterval(sample)
    interval3_bis, coverage3 = algo.computeBilateralConfidenceIntervalWithCoverage(
        sample
    )

    assert interval1 == interval1_bis
    assert interval2 == interval2_bis
    assert interval3 == interval3_bis

    assert coverage1 >= beta
    assert coverage2 >= beta
    assert coverage3 >= beta

    ott.assert_almost_equal(coverage1, 0.97, 0.005, 0.0)
    ott.assert_almost_equal(coverage2, 0.96, 0.005, 0.0)
    ott.assert_almost_equal(coverage3, 0.95, 0.005, 0.0)

    q = distribution.computeQuantile(alpha)
    if interval1.contains(q):
        p1 += 1.0 / nreps
    if interval2.contains(q):
        p2 += 1.0 / nreps
    if interval3.contains(q):
        p3 += 1.0 / nreps

print(f"p1={p1:.6f} p2={p2:.6f} p3={p3:.6f}")
assert p1 >= beta
assert p2 >= beta
assert p3 >= beta

ott.assert_almost_equal(p1, coverage1, 0.005, 0.0)
ott.assert_almost_equal(p2, coverage2, 0.01, 0.0)
ott.assert_almost_equal(p3, coverage3, 0.005, 0.0)


# minimum size
print("minimum size ...")
ref_a = {}  # dict of reference size values for different (alpha, beta) pairs
ref_a[(0.01, 0.99)] = 459
ref_a[(0.01, 0.95)] = 299
ref_a[(0.01, 0.9)] = 230
ref_a[(0.05, 0.99)] = 90
ref_a[(0.05, 0.95)] = 59
ref_a[(0.05, 0.9)] = 45
ref_a[(0.1, 0.99)] = 44
ref_a[(0.1, 0.95)] = 29
ref_a[(0.1, 0.9)] = 22
ref_a[(0.75, 0.99)] = 4
ref_a[(0.75, 0.95)] = 3
ref_a[(0.75, 0.9)] = 2
ref_a[(0.9, 0.99)] = 2
ref_a[(0.9, 0.95)] = 2
ref_a[(0.9, 0.9)] = 1
ref_a[(0.95, 0.99)] = 2
ref_a[(0.95, 0.95)] = 1
ref_a[(0.95, 0.9)] = 1
ref_a[(0.99, 0.99)] = 1
ref_a[(0.99, 0.95)] = 1
ref_a[(0.99, 0.9)] = 1

ref_b = {}
ref_b[(0.01, 0.9)] = 1
ref_b[(0.01, 0.95)] = 1
ref_b[(0.01, 0.99)] = 1
ref_b[(0.05, 0.9)] = 1
ref_b[(0.05, 0.95)] = 1
ref_b[(0.05, 0.99)] = 2
ref_b[(0.1, 0.9)] = 1
ref_b[(0.1, 0.95)] = 2
ref_b[(0.1, 0.99)] = 2
ref_b[(0.25, 0.9)] = 2
ref_b[(0.25, 0.95)] = 3
ref_b[(0.25, 0.99)] = 4
ref_b[(0.75, 0.9)] = 9
ref_b[(0.75, 0.95)] = 11
ref_b[(0.75, 0.99)] = 17
ref_b[(0.9, 0.9)] = 22
ref_b[(0.9, 0.95)] = 29
ref_b[(0.9, 0.99)] = 44
ref_b[(0.95, 0.9)] = 45
ref_b[(0.95, 0.95)] = 59
ref_b[(0.95, 0.99)] = 90
ref_b[(0.99, 0.9)] = 230
ref_b[(0.99, 0.95)] = 299
ref_b[(0.99, 0.99)] = 459

ref_c = {}
ref_c[(0.01, 0.9)] = 230
ref_c[(0.01, 0.95)] = 299
ref_c[(0.01, 0.99)] = 459
ref_c[(0.05, 0.9)] = 45
ref_c[(0.05, 0.95)] = 59
ref_c[(0.05, 0.99)] = 90
ref_c[(0.1, 0.9)] = 22
ref_c[(0.1, 0.95)] = 29
ref_c[(0.1, 0.99)] = 44
ref_c[(0.25, 0.9)] = 9
ref_c[(0.25, 0.95)] = 11
ref_c[(0.25, 0.99)] = 17
ref_c[(0.75, 0.9)] = 9
ref_c[(0.75, 0.95)] = 11
ref_c[(0.75, 0.99)] = 17
ref_c[(0.9, 0.9)] = 22
ref_c[(0.9, 0.95)] = 29
ref_c[(0.9, 0.99)] = 44
ref_c[(0.95, 0.9)] = 45
ref_c[(0.95, 0.95)] = 59
ref_c[(0.95, 0.99)] = 90
ref_c[(0.99, 0.9)] = 230
ref_c[(0.99, 0.95)] = 299
ref_c[(0.99, 0.99)] = 459

for alpha in [0.01, 0.05, 0.10, 0.25, 0.75, 0.90, 0.95, 0.99]:
    for beta in [0.90, 0.95, 0.99]:
        algo = otexp.QuantileConfidence(alpha, beta)
        # lower
        for r in range(5):
            n = algo.computeUnilateralMinimumSampleSize(r)
            p = ot.Binomial(n, alpha).computeComplementaryCDF(r)
            pPrev = ot.Binomial(n - 1, alpha).computeComplementaryCDF(r)
            print(
                f"alpha={alpha:.3f} beta={beta:.3f} r={r} n={n} p={p:.6f} pPrev={pPrev:.6f}"
            )
            assert p >= beta
            assert pPrev < beta
            if r == 0 and (alpha, beta) in ref_a:
                assert n == ref_a[(alpha, beta)]

        # upper
        for r in range(5):
            n = algo.computeUnilateralMinimumSampleSize(r, True)
            p = ot.Binomial(n, alpha).computeCDF(n - 1 - r)
            pPrev = ot.Binomial(n - 1, alpha).computeCDF((n - 1) - 1 - r)
            print(
                f"alpha={alpha:.3f} beta={beta:.3f} r={r} n={n} p={p:.6f} pPrev={pPrev:.6f}"
            )
            assert p >= beta
            assert pPrev < beta
            if r == 0 and (alpha, beta) in ref_b:
                assert n == ref_b[(alpha, beta)]

        # bilateral
        n = algo.computeBilateralMinimumSampleSize()
        p = 1 - alpha**n - (1 - alpha) ** n
        pPrev = 1 - alpha ** (n - 1) - (1 - alpha) ** (n - 1)
        print(
            f"alpha={alpha:.3f} beta={beta:.3f} r={r} n={n} p={p:.6f} pPrev={pPrev:.6f}"
        )
        assert p >= beta
        assert pPrev < beta
        if (alpha, beta) in ref_c:
            assert n == ref_c[(alpha, beta)]

# table J13 from Meeker2017
for alpha in [
    0.5,
    0.55,
    0.6,
    0.65,
    0.7,
    0.75,
    0.8,
    0.85,
    0.9,
    0.95,
    0.96,
    0.97,
    0.98,
    0.99,
    0.995,
    0.999,
]:
    print(f"{alpha:.3f} | ", end=" ")
    for beta in [0.5, 0.75, 0.9, 0.95, 0.98, 0.99, 0.999]:
        algo = otexp.QuantileConfidence(alpha, beta)
        n = algo.computeUnilateralMinimumSampleSize(0, True)
        if alpha == 0.5 and beta == 0.75:
            print(f"alpha={alpha} beta={beta} n={n}")
        print(f"{n: >6}", end=" ")
    print("")

n_ref = {}
n_ref[(0.5, 0.5)] = 1
n_ref[(0.5, 0.75)] = 2
n_ref[(0.5, 0.9)] = 4
n_ref[(0.5, 0.95)] = 5
n_ref[(0.5, 0.98)] = 6
n_ref[(0.5, 0.99)] = 7
n_ref[(0.5, 0.999)] = 10

n_ref[(0.55, 0.5)] = 2
n_ref[(0.55, 0.75)] = 3
n_ref[(0.55, 0.9)] = 4
n_ref[(0.55, 0.95)] = 6
n_ref[(0.55, 0.98)] = 7
n_ref[(0.55, 0.99)] = 8
n_ref[(0.55, 0.999)] = 12

n_ref[(0.6, 0.5)] = 2
n_ref[(0.6, 0.75)] = 3
n_ref[(0.6, 0.9)] = 5
n_ref[(0.6, 0.95)] = 6
n_ref[(0.6, 0.98)] = 8
n_ref[(0.6, 0.99)] = 10
n_ref[(0.6, 0.999)] = 14

n_ref[(0.65, 0.5)] = 2
n_ref[(0.65, 0.75)] = 4
n_ref[(0.65, 0.9)] = 6
n_ref[(0.65, 0.95)] = 7
n_ref[(0.65, 0.98)] = 10
n_ref[(0.65, 0.99)] = 11
n_ref[(0.65, 0.999)] = 17

n_ref[(0.7, 0.5)] = 2
n_ref[(0.7, 0.75)] = 4
n_ref[(0.7, 0.9)] = 7
n_ref[(0.7, 0.95)] = 9
n_ref[(0.7, 0.98)] = 11
n_ref[(0.7, 0.99)] = 13
n_ref[(0.7, 0.999)] = 20

n_ref[(0.75, 0.5)] = 3
n_ref[(0.75, 0.75)] = 5
n_ref[(0.75, 0.9)] = 9
n_ref[(0.75, 0.95)] = 11
n_ref[(0.75, 0.98)] = 14
n_ref[(0.75, 0.99)] = 17
n_ref[(0.75, 0.999)] = 25

n_ref[(0.8, 0.5)] = 4
n_ref[(0.8, 0.75)] = 7
n_ref[(0.8, 0.9)] = 11
n_ref[(0.8, 0.95)] = 14
n_ref[(0.8, 0.98)] = 18
n_ref[(0.8, 0.99)] = 21
n_ref[(0.8, 0.999)] = 31

n_ref[(0.85, 0.5)] = 5
n_ref[(0.85, 0.75)] = 9
n_ref[(0.85, 0.9)] = 15
n_ref[(0.85, 0.95)] = 19
n_ref[(0.85, 0.98)] = 25
n_ref[(0.85, 0.99)] = 29
n_ref[(0.85, 0.999)] = 43

n_ref[(0.9, 0.5)] = 7
n_ref[(0.9, 0.75)] = 14
n_ref[(0.9, 0.9)] = 22
n_ref[(0.9, 0.95)] = 29
n_ref[(0.9, 0.98)] = 38
n_ref[(0.9, 0.99)] = 44
n_ref[(0.9, 0.999)] = 66

n_ref[(0.95, 0.5)] = 14
n_ref[(0.95, 0.75)] = 28
n_ref[(0.95, 0.9)] = 45
n_ref[(0.95, 0.95)] = 59
n_ref[(0.95, 0.98)] = 77
n_ref[(0.95, 0.99)] = 90
n_ref[(0.95, 0.999)] = 135

n_ref[(0.96, 0.5)] = 17
n_ref[(0.96, 0.75)] = 34
n_ref[(0.96, 0.9)] = 57
n_ref[(0.96, 0.95)] = 74
n_ref[(0.96, 0.98)] = 96
n_ref[(0.96, 0.99)] = 113
n_ref[(0.96, 0.999)] = 170

n_ref[(0.97, 0.5)] = 23
n_ref[(0.97, 0.75)] = 46
n_ref[(0.97, 0.9)] = 76
n_ref[(0.97, 0.95)] = 99
n_ref[(0.97, 0.98)] = 129
n_ref[(0.97, 0.99)] = 152
n_ref[(0.97, 0.999)] = 227

n_ref[(0.98, 0.5)] = 35
n_ref[(0.98, 0.75)] = 69
n_ref[(0.98, 0.9)] = 114
n_ref[(0.98, 0.95)] = 149
n_ref[(0.98, 0.98)] = 194
n_ref[(0.98, 0.99)] = 228
n_ref[(0.98, 0.999)] = 342

n_ref[(0.99, 0.5)] = 69
n_ref[(0.99, 0.75)] = 138
n_ref[(0.99, 0.9)] = 230
n_ref[(0.99, 0.95)] = 299
n_ref[(0.99, 0.98)] = 390
n_ref[(0.99, 0.99)] = 459
n_ref[(0.99, 0.999)] = 688

n_ref[(0.995, 0.5)] = 139
n_ref[(0.995, 0.75)] = 277
n_ref[(0.995, 0.9)] = 460
n_ref[(0.995, 0.95)] = 598
n_ref[(0.995, 0.98)] = 781
n_ref[(0.995, 0.99)] = 919
n_ref[(0.995, 0.999)] = 1379

n_ref[(0.999, 0.5)] = 693
n_ref[(0.999, 0.75)] = 1386
n_ref[(0.999, 0.9)] = 2302
n_ref[(0.999, 0.95)] = 2995
n_ref[(0.999, 0.98)] = 3911
n_ref[(0.999, 0.99)] = 4603
n_ref[(0.999, 0.999)] = 6905

for alpha, beta in n_ref.keys():
    algo = otexp.QuantileConfidence(alpha, beta)
    n = algo.computeUnilateralMinimumSampleSize(0, True)
    ref = n_ref[(alpha, beta)]
    print(f"alpha={alpha} beta={beta} n={n} ref={ref}")
    assert n == ref


# asymptotic confidence
print("asymptotic confidence ...")
alpha = 0.1
beta = 0.95
algo = otexp.QuantileConfidence(alpha, beta)
dist = ot.Gumbel()
qalpha = dist.computeQuantile(alpha)
for i in range(3, 7):
    n = 10**i
    k1, k2 = algo.computeAsymptoticBilateralRank(n)
    binom = ot.Binomial(n, alpha)
    p = binom.computeProbability(ot.Interval(k1, k2))
    print(f"n={n} ci=[{k1}, {k2}] p={p}")
    atol = 2.0 * n**-0.5
    ott.assert_almost_equal(p, beta, 0.0, atol)
    if n <= 1e4:
        sample = dist.getSample(n)
        ci = algo.computeAsymptoticBilateralConfidenceInterval(sample)
        assert ci.contains(qalpha)
