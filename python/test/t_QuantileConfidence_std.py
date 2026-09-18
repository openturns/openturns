#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott


def check_basic():
    print("check_basic ...")
    alpha = 0.05
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    print(algo)
    print(repr(algo))
    algo.setAlpha(alpha)
    algo.setBeta(beta)
    assert algo.getAlpha() == alpha
    assert algo.getBeta() == beta
    # Check exceptions in the constructor
    alpha = -0.5
    beta = 0.95
    with ott.assert_raises(TypeError):
        algo = ot.QuantileConfidence(alpha, beta)
    alpha = 1.5
    beta = 0.95
    with ott.assert_raises(TypeError):
        algo = ot.QuantileConfidence(alpha, beta)
    alpha = 0.95
    beta = -0.5
    with ott.assert_raises(TypeError):
        algo = ot.QuantileConfidence(alpha, beta)
    alpha = 0.95
    beta = 1.5
    with ott.assert_raises(TypeError):
        algo = ot.QuantileConfidence(alpha, beta)


# lower rank
def get_lower_rank_ref():
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
    return k_ref


# lower rank
def check_lower_rank():
    print("check_lower_rank ...")
    alpha = 0.05
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    # lower rank
    k_ref = get_lower_rank_ref()
    lower_bounded = True
    for n in k_ref.keys():
        ll = algo.computeUnilateralRank(n, lower_bounded)
        p = algo.computeUnilateralCoverage(n, ll, lower_bounded)
        print(f"n={n} l={ll} ref={k_ref[n]} p={p}")
        assert ll == k_ref[n]
        assert p >= beta
    # Check exceptions
    # N is lower than the minimum sample size
    with ott.assert_raises(TypeError):
        n = 58
        ll = algo.computeUnilateralRank(n, lower_bounded)
    # The rank is larger or equal to the size
    with ott.assert_raises(TypeError):
        n = 59
        ll = 59
        p = algo.computeUnilateralCoverage(n, ll, lower_bounded)


# upper rank
def get_upper_rank_ref():
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
    return k_ref


# upper rank
def check_upper_rank():
    print("check_upper_rank ...")
    alpha = 0.95
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    # upper rank
    k_ref = get_upper_rank_ref()
    for n in k_ref.keys():
        u = algo.computeUnilateralRank(n)
        p = algo.computeUnilateralCoverage(n, u)
        print(f"n={n} u={u} ref={k_ref[n]} p={p}")
        assert u == k_ref[n]
        assert p >= beta
    # Check exceptions
    # N is lower than the minimum sample size
    with ott.assert_raises(TypeError):
        n = 58
        ll = algo.computeUnilateralRank(n)
    # The rank is larger or equal to the size
    with ott.assert_raises(TypeError):
        n = 59
        ll = 59
        p = algo.computeUnilateralCoverage(n, ll)


# bilateral ranks
def get_bilateral_rank_ref():
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
    return k_ref


# bilateral ranks
def check_bilateral_rank():
    print("check_bilateral_rank ...")
    alpha = 0.05
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    #
    # bilateral ranks
    k_ref = get_bilateral_rank_ref()

    for n in k_ref.keys():
        k1, k2 = algo.computeBilateralRank(n)
        print(f"n={n} k1={k1} k2={k2}")
        p = algo.computeBilateralCoverage(n, k1, k2)
        print(f"coverage={p}")
        assert (k1, k2) == k_ref[n]
        assert p >= beta

        # we can also make sure by exploring all combinations (costly)
        if n > 100:
            continue
        bestP = 2
        bestK1K2 = None
        for i1 in range(n):
            for i2 in range(i1, n):
                # we compute P(k1<X<=k2)=P(k1+1<=X<=k2), CDF(k2)-CDF(k1) also works
                p = algo.computeBilateralCoverage(n, i1, i2)
                if (p >= beta) and (p < bestP):
                    bestP = p
                    bestK1K2 = i1, i2
        # print(n, bestK1K2, bestP)
        assert bestK1K2 is not None, f"No valid (k1, k2) pair found for n={n}"
        assert (k1, k2) == bestK1K2

    # Check exceptions
    # N is lower than the minimum sample size
    with ott.assert_raises(TypeError):
        n = 58
        k1, k2 = algo.computeBilateralRank(n)
    # The rank k1 is larger than k2
    with ott.assert_raises(TypeError):
        n = 59
        k1 = 10
        k2 = 9
        p = algo.computeBilateralCoverage(n, k1, k2)
    # The rank k2 is larger or equal to the size
    with ott.assert_raises(TypeError):
        n = 59
        k1 = 0
        k2 = 59
        p = algo.computeBilateralCoverage(n, k1, k2)


# validate confidence intervals by probability estimation
def check_validate_empirical_coverage():
    print("check_validate_empirical_coverage ...")
    # validate confidence intervals by probability estimation
    # Estimate the probability that the interval contains the true quantile
    # by Monte Carlo sampling.
    # Then the computed probability should be close to the expected
    # coverage.
    alpha = 0.95
    beta = 0.90
    algo = ot.QuantileConfidence(alpha, beta)
    size = 100
    distribution = ot.Normal()
    quantile_alpha = distribution.computeQuantile(alpha)
    # This should be as large as possible (but is limited by CPU time)
    number_of_repetitions = 3000
    count_upper = 0
    count_lower = 0
    count_bilateral = 0
    for _ in range(number_of_repetitions):
        sample = distribution.getSample(size)
        interval_upper = algo.computeUnilateralConfidenceInterval(sample)
        interval_lower = algo.computeUnilateralConfidenceInterval(sample, True)
        interval_bilateral = algo.computeBilateralConfidenceInterval(sample)

        if interval_upper.contains(quantile_alpha):
            count_upper += 1
        if interval_lower.contains(quantile_alpha):
            count_lower += 1
        if interval_bilateral.contains(quantile_alpha):
            count_bilateral += 1

    estimated_coverage_upper = count_upper / number_of_repetitions
    estimated_coverage_lower = count_lower / number_of_repetitions
    estimated_coverage_bilateral = count_bilateral / number_of_repetitions
    print(f"estimated_coverage_upper={estimated_coverage_upper}")
    print(f"estimated_coverage_lower={estimated_coverage_lower}")
    print(f"estimated_coverage_bilateral={estimated_coverage_bilateral}")

    upper_rank = algo.computeUnilateralRank(size)
    lower_rank = algo.computeUnilateralRank(size, True)
    bilateral_k1, bilateral_k2 = algo.computeBilateralRank(size)
    exact_coverage_upper = algo.computeUnilateralCoverage(size, upper_rank)
    exact_coverage_lower = algo.computeUnilateralCoverage(size, lower_rank, True)
    exact_coverage_bilateral = algo.computeBilateralCoverage(
        size, bilateral_k1, bilateral_k2
    )
    print(f"exact_coverage_upper={exact_coverage_upper}")
    print(f"exact_coverage_lower={exact_coverage_lower}")
    print(f"exact_coverage_bilateral={exact_coverage_bilateral}")

    ott.assert_almost_equal(estimated_coverage_upper, exact_coverage_upper, 0.01, 0.0)
    ott.assert_almost_equal(estimated_coverage_lower, exact_coverage_lower, 0.01, 0.0)
    ott.assert_almost_equal(
        estimated_coverage_bilateral, exact_coverage_bilateral, 0.01, 0.0
    )


# minimum size
def get_minimum_sample_size_lower():
    ref_minimum_sample_size_lower = {
        (0.01, 0.99): 459,
        (0.01, 0.95): 299,
        (0.01, 0.9): 230,
        (0.05, 0.99): 90,
        (0.05, 0.95): 59,
        (0.05, 0.9): 45,
        (0.1, 0.99): 44,
        (0.1, 0.95): 29,
        (0.1, 0.9): 22,
        (0.75, 0.99): 4,
        (0.75, 0.95): 3,
        (0.75, 0.9): 2,
        (0.9, 0.99): 2,
        (0.9, 0.95): 2,
        (0.9, 0.9): 1,
        (0.95, 0.99): 2,
        (0.95, 0.95): 1,
        (0.95, 0.9): 1,
        (0.99, 0.99): 1,
        (0.99, 0.95): 1,
        (0.99, 0.9): 1,
    }
    return ref_minimum_sample_size_lower


def get_minimum_sample_size_upper():
    ref_minimum_sample_size_upper = {
        (0.01, 0.9): 1,
        (0.01, 0.95): 1,
        (0.01, 0.99): 1,
        (0.05, 0.9): 1,
        (0.05, 0.95): 1,
        (0.05, 0.99): 2,
        (0.1, 0.9): 1,
        (0.1, 0.95): 2,
        (0.1, 0.99): 2,
        (0.25, 0.9): 2,
        (0.25, 0.95): 3,
        (0.25, 0.99): 4,
        (0.75, 0.9): 9,
        (0.75, 0.95): 11,
        (0.75, 0.99): 17,
        (0.9, 0.9): 22,
        (0.9, 0.95): 29,
        (0.9, 0.99): 44,
        (0.95, 0.9): 45,
        (0.95, 0.95): 59,
        (0.95, 0.99): 90,
        (0.99, 0.9): 230,
        (0.99, 0.95): 299,
        (0.99, 0.99): 459,
    }
    return ref_minimum_sample_size_upper


def get_minimum_sample_size_bilateral():
    ref_minimum_sample_size_bilateral = {
        (0.01, 0.9): 230,
        (0.01, 0.95): 299,
        (0.01, 0.99): 459,
        (0.05, 0.9): 45,
        (0.05, 0.95): 59,
        (0.05, 0.99): 90,
        (0.1, 0.9): 22,
        (0.1, 0.95): 29,
        (0.1, 0.99): 44,
        (0.25, 0.9): 9,
        (0.25, 0.95): 11,
        (0.25, 0.99): 17,
        (0.75, 0.9): 9,
        (0.75, 0.95): 11,
        (0.75, 0.99): 17,
        (0.9, 0.9): 22,
        (0.9, 0.95): 29,
        (0.9, 0.99): 44,
        (0.95, 0.9): 45,
        (0.95, 0.95): 59,
        (0.95, 0.99): 90,
        (0.99, 0.9): 230,
        (0.99, 0.95): 299,
        (0.99, 0.99): 459,
    }
    return ref_minimum_sample_size_bilateral


# minimum size
def check_minimum_sample_size():
    print("check_minimum_sample_size ...")
    # minimum size
    # dict of reference size values for different (alpha, beta) pairs
    ref_minimum_sample_size_lower = get_minimum_sample_size_lower()
    ref_minimum_sample_size_upper = get_minimum_sample_size_upper()
    ref_minimum_sample_size_bilateral = get_minimum_sample_size_bilateral()

    for alpha in [0.01, 0.05, 0.10, 0.25, 0.75, 0.90, 0.95, 0.99]:
        for beta in [0.90, 0.95, 0.99]:
            algo = ot.QuantileConfidence(alpha, beta)
            # lower
            lower_bound = True
            for r in range(5):
                n = algo.computeUnilateralMinimumSampleSize(r, lower_bound)
                p = ot.Binomial(n, alpha).computeComplementaryCDF(r)
                pPrev = ot.Binomial(n - 1, alpha).computeComplementaryCDF(r)
                print(
                    f"alpha={alpha:.3f} beta={beta:.3f} r={r} n={n} p={p:.6f} pPrev={pPrev:.6f}"
                )
                assert p >= beta
                assert pPrev < beta
                if r == 0 and (alpha, beta) in ref_minimum_sample_size_lower:
                    assert n == ref_minimum_sample_size_lower[(alpha, beta)]

            # upper
            # lower_bound = False is the default value
            for r in range(5):
                n = algo.computeUnilateralMinimumSampleSize(r)
                p = ot.Binomial(n, alpha).computeCDF(n - 1 - r)
                pPrev = ot.Binomial(n - 1, alpha).computeCDF((n - 1) - 1 - r)
                print(
                    f"alpha={alpha:.3f} beta={beta:.3f} r={r} n={n} p={p:.6f} pPrev={pPrev:.6f}"
                )
                assert p >= beta
                assert pPrev < beta
                if r == 0 and (alpha, beta) in ref_minimum_sample_size_upper:
                    assert n == ref_minimum_sample_size_upper[(alpha, beta)]

            # bilateral
            n = algo.computeBilateralMinimumSampleSize()
            p = 1 - alpha**n - (1 - alpha) ** n
            pPrev = 1 - alpha ** (n - 1) - (1 - alpha) ** (n - 1)
            print(
                f"alpha={alpha:.3f} beta={beta:.3f} r={r} n={n} p={p:.6f} pPrev={pPrev:.6f}"
            )
            assert p >= beta
            assert pPrev < beta
            if (alpha, beta) in ref_minimum_sample_size_bilateral:
                assert n == ref_minimum_sample_size_bilateral[(alpha, beta)]


# table J13 from Meeker2017
def check_print_minimum_unilateral_sample_size_meeker():
    print("check_print_minimum_unilateral_sample_size_meeker ...")
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
            algo = ot.QuantileConfidence(alpha, beta)
            n = algo.computeUnilateralMinimumSampleSize(0, True)
            if alpha == 0.5 and beta == 0.75:
                print(f"alpha={alpha} beta={beta} n={n}")
            print(f"{n: >6}", end=" ")
        print("")


def get_minimum_unilateral_sample_size_ref():
    # See Meeker2017, table J.13
    # In Meeker2017, beta is the quantile level, 1 - alpha is the confidence level
    n_ref = {
        (0.5, 0.5): 1,
        (0.5, 0.75): 2,
        (0.5, 0.9): 4,
        (0.5, 0.95): 5,
        (0.5, 0.98): 6,
        (0.5, 0.99): 7,
        (0.5, 0.999): 10,
        (0.55, 0.5): 2,
        (0.55, 0.75): 3,
        (0.55, 0.9): 4,
        (0.55, 0.95): 6,
        (0.55, 0.98): 7,
        (0.55, 0.99): 8,
        (0.55, 0.999): 12,
        (0.6, 0.5): 2,
        (0.6, 0.75): 3,
        (0.6, 0.9): 5,
        (0.6, 0.95): 6,
        (0.6, 0.98): 8,
        (0.6, 0.99): 10,
        (0.6, 0.999): 14,
        (0.65, 0.5): 2,
        (0.65, 0.75): 4,
        (0.65, 0.9): 6,
        (0.65, 0.95): 7,
        (0.65, 0.98): 10,
        (0.65, 0.99): 11,
        (0.65, 0.999): 17,
        (0.7, 0.5): 2,
        (0.7, 0.75): 4,
        (0.7, 0.9): 7,
        (0.7, 0.95): 9,
        (0.7, 0.98): 11,
        (0.7, 0.99): 13,
        (0.7, 0.999): 20,
        (0.75, 0.5): 3,
        (0.75, 0.75): 5,
        (0.75, 0.9): 9,
        (0.75, 0.95): 11,
        (0.75, 0.98): 14,
        (0.75, 0.99): 17,
        (0.75, 0.999): 25,
        (0.8, 0.5): 4,
        (0.8, 0.75): 7,
        (0.8, 0.9): 11,
        (0.8, 0.95): 14,
        (0.8, 0.98): 18,
        (0.8, 0.99): 21,
        (0.8, 0.999): 31,
        (0.85, 0.5): 5,
        (0.85, 0.75): 9,
        (0.85, 0.9): 15,
        (0.85, 0.95): 19,
        (0.85, 0.98): 25,
        (0.85, 0.99): 29,
        (0.85, 0.999): 43,
        (0.9, 0.5): 7,
        (0.9, 0.75): 14,
        (0.9, 0.9): 22,
        (0.9, 0.95): 29,
        (0.9, 0.98): 38,
        (0.9, 0.99): 44,
        (0.9, 0.999): 66,
        (0.95, 0.5): 14,
        (0.95, 0.75): 28,
        (0.95, 0.9): 45,
        (0.95, 0.95): 59,
        (0.95, 0.98): 77,
        (0.95, 0.99): 90,
        (0.95, 0.999): 135,
        (0.96, 0.5): 17,
        (0.96, 0.75): 34,
        (0.96, 0.9): 57,
        (0.96, 0.95): 74,
        (0.96, 0.98): 96,
        (0.96, 0.99): 113,
        (0.96, 0.999): 170,
        (0.97, 0.5): 23,
        (0.97, 0.75): 46,
        (0.97, 0.9): 76,
        (0.97, 0.95): 99,
        (0.97, 0.98): 129,
        (0.97, 0.99): 152,
        (0.97, 0.999): 227,
        (0.98, 0.5): 35,
        (0.98, 0.75): 69,
        (0.98, 0.9): 114,
        (0.98, 0.95): 149,
        (0.98, 0.98): 194,
        (0.98, 0.99): 228,
        (0.98, 0.999): 342,
        (0.99, 0.5): 69,
        (0.99, 0.75): 138,
        (0.99, 0.9): 230,
        (0.99, 0.95): 299,
        (0.99, 0.98): 390,
        (0.99, 0.99): 459,
        (0.99, 0.999): 688,
        (0.995, 0.5): 139,
        (0.995, 0.75): 277,
        (0.995, 0.9): 460,
        (0.995, 0.95): 598,
        (0.995, 0.98): 781,
        (0.995, 0.99): 919,
        (0.995, 0.999): 1379,
        (0.999, 0.5): 693,
        (0.999, 0.75): 1386,
        (0.999, 0.9): 2302,
        (0.999, 0.95): 2995,
        (0.999, 0.98): 3911,
        (0.999, 0.99): 4603,
        (0.999, 0.999): 6905,
    }
    return n_ref


def check_minimum_unilateral_sample_size():
    print("check_minimum_unilateral_sample_size ...")
    n_ref = get_minimum_unilateral_sample_size_ref()
    for lower_bound in [True, False]:
        for alpha, beta in n_ref.keys():
            if lower_bound:
                algo = ot.QuantileConfidence(1.0 - alpha, beta)
            else:
                algo = ot.QuantileConfidence(alpha, beta)
            n = algo.computeUnilateralMinimumSampleSize(0, lower_bound)
            ref = n_ref[(alpha, beta)]
            print(
                f"lower_bound={lower_bound} "
                f"alpha={alpha} beta={beta} n={n} ref={ref}"
            )
            assert n == ref


# asymptotic confidence
def check_asymptotic_confidence():
    print("check_asymptotic_confidence ...")
    # asymptotic confidence
    print("asymptotic confidence ...")
    alpha = 0.1
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
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

    # Exceptions in computeAsymptoticBilateralConfidenceInterval, dimension > 1
    with ott.assert_raises(TypeError):
        sample = ot.Sample(10, 2)
        ci = algo.computeAsymptoticBilateralConfidenceInterval(sample)
    # Exceptions in computeAsymptoticBilateralConfidenceInterval, size = 0
    with ott.assert_raises(TypeError):
        sample = ot.Sample()
        ci = algo.computeAsymptoticBilateralConfidenceInterval(sample)


# bilateral ranks bug
def check_bilateral_rank_n_large():
    print("check_bilateral_rank_n_large...")
    # Case 1: computeBilateralRank when n is large and alpha > 0.5
    alpha = 0.95
    beta = 0.90
    n = 975
    algo = ot.QuantileConfidence(alpha, beta)
    k1, k2 = algo.computeBilateralRank(n)
    k1_ref = 910
    k2_ref = 935
    assert k1 == k1_ref
    assert k2 == k2_ref
    coverage = algo.computeBilateralCoverage(n, k1, k2)
    assert coverage >= beta

    # Case 2: Very large n test
    alpha = 0.51762756742630334
    beta = 0.95
    n = 674726183
    algo = ot.QuantileConfidence(alpha, beta)
    k1, k2 = algo.computeBilateralRank(n)
    binom = ot.Binomial(n, alpha)
    p = binom.computeCDF(k2) - binom.computeCDF(k1)
    print(f"n={n} k1={k1} k2={k2} p={p}")
    assert p >= beta


# edge cases for computeBilateralRank
def check_bilateral_rank_edge_cases():
    print("check_bilateral_rank_edge_cases ...")
    # edge cases for computeBilateralRank
    # 1. alpha < 0.5 with CDF(0) underflow (CDF(0) < epsilon)
    for alpha in [0.01, 0.05]:
        beta = 0.95
        algo = ot.QuantileConfidence(alpha, beta)
        nMin = algo.computeBilateralMinimumSampleSize()
        for n in [nMin, nMin + 100, nMin + 1000]:
            k1, k2 = algo.computeBilateralRank(n)
            binom = ot.Binomial(n, alpha)
            p = binom.computeCDF(k2) - binom.computeCDF(k1)
            print(f"alpha={alpha} n={n} k1={k1} k2={k2} p={p}")
            assert k2 < n
            assert k1 < k2
            assert p >= beta
            # k1=0 should provide a valid interval at nMin
            if n == nMin:
                p0_val = binom.computeCDF(0)
                k2_zero = binom.computeScalarQuantile(p0_val + beta)
                if k2_zero < n:
                    p0 = binom.computeCDF(k2_zero) - p0_val
                    assert p0 >= beta

    # 2. alpha > 0.5 (quantile in upper tail)
    for alpha in [0.95, 0.99]:
        beta = 0.90
        algo = ot.QuantileConfidence(alpha, beta)
        nMin = algo.computeBilateralMinimumSampleSize()
        for n in [nMin, nMin + 100, nMin + 1000]:
            k1, k2 = algo.computeBilateralRank(n)
            binom = ot.Binomial(n, alpha)
            p = binom.computeCDF(k2) - binom.computeCDF(k1)
            print(f"alpha={alpha} n={n} k1={k1} k2={k2} p={p}")
            assert k2 < n
            assert k1 < k2
            assert p >= beta
            if n == nMin:
                p0_val = binom.computeCDF(0)
                k2_zero = binom.computeScalarQuantile(p0_val + beta)
                if k2_zero < n:
                    p0 = binom.computeCDF(k2_zero) - p0_val
                    assert p0 >= beta

    # 3. alpha = 0.5 (symmetric)
    for beta in [0.9, 0.99]:
        alpha = 0.5
        algo = ot.QuantileConfidence(alpha, beta)
        nMin = algo.computeBilateralMinimumSampleSize()
        for n in [nMin, nMin + 100]:
            k1, k2 = algo.computeBilateralRank(n)
            binom = ot.Binomial(n, alpha)
            p = binom.computeCDF(k2) - binom.computeCDF(k1)
            print(f"alpha={alpha} beta={beta} n={n} k1={k1} k2={k2} p={p}")
            assert k2 < n
            assert k1 < k2
            assert p >= beta

    # 4. beta close to 1
    alpha = 0.05
    for beta in [0.99, 0.999]:
        algo = ot.QuantileConfidence(alpha, beta)
        nMin = algo.computeBilateralMinimumSampleSize()
        for n in [nMin, nMin + 1000]:
            k1, k2 = algo.computeBilateralRank(n)
            binom = ot.Binomial(n, alpha)
            p = binom.computeCDF(k2) - binom.computeCDF(k1)
            print(f"alpha={alpha} beta={beta} n={n} k1={k1} k2={k2} p={p}")
            assert k2 < n
            assert k1 < k2
            assert p >= beta

    # 5. very large n with alpha=0.05 (CDF(0) underflows)
    alpha = 0.05
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    for n in [5000, 10000, 100000, 1000000]:
        k1, k2 = algo.computeBilateralRank(n)
        binom = ot.Binomial(n, alpha)
        p = binom.computeCDF(k2) - binom.computeCDF(k1)
        print(f"alpha={alpha} n={n} k1={k1} k2={k2} p={p}")
        assert k2 < n
        assert k1 < k2
        assert p >= beta

    # 6. confidence level such that it is exactly equal to the difference of known ranks
    n = int(1e6)
    k1_ref = 49625
    k2_ref = 50533
    alpha = 0.05
    binom = ot.Binomial(n, alpha)
    beta = binom.computeCDF(k2_ref) - binom.computeCDF(k1_ref)
    algo = ot.QuantileConfidence(alpha, beta)
    k1, k2 = algo.computeBilateralRank(n)
    assert k1 == k1_ref
    assert k2 == k2_ref


# random tests
def check_bilateral_rank_random(beta=0.95):
    for _ in range(int(1e2)):
        alpha = ot.RandomGenerator.Generate()
        algo = ot.QuantileConfidence(alpha, beta)
        nMin = algo.computeBilateralMinimumSampleSize()
        n = nMin + ot.RandomGenerator.IntegerGenerate(int(1e6))
        k1, k2 = algo.computeBilateralRank(n)
        binom = ot.Binomial(n, alpha)
        p = binom.computeCDF(k2) - binom.computeCDF(k1)
        assert k2 < n
        assert k1 < k2
        assert p >= beta


def check_interval_coverage_consistency():
    print("check_interval_coverage_consistency ...")
    # Check that computeUnilateralConfidenceInterval and
    # computeUnilateralConfidenceIntervalWithCoverage
    # produce the same results on a given sample
    alpha = 0.95
    beta = 0.90
    algo = ot.QuantileConfidence(alpha, beta)
    size = 100
    distribution = ot.Normal()
    sample = distribution.getSample(size)
    interval_upper = algo.computeUnilateralConfidenceInterval(sample)
    interval_upper_bis, coverage_upper = (
        algo.computeUnilateralConfidenceIntervalWithCoverage(sample)
    )
    interval_lower = algo.computeUnilateralConfidenceInterval(sample, True)
    interval_lower_bis, coverage_lower = (
        algo.computeUnilateralConfidenceIntervalWithCoverage(sample, True)
    )
    interval_bilateral = algo.computeBilateralConfidenceInterval(sample)
    interval_bilateral_bis, coverage_bilateral = (
        algo.computeBilateralConfidenceIntervalWithCoverage(sample)
    )

    assert interval_upper == interval_upper_bis
    assert interval_lower == interval_lower_bis
    assert interval_bilateral == interval_bilateral_bis

    assert coverage_upper >= beta
    assert coverage_lower >= beta
    assert coverage_bilateral >= beta

    # The sample is has 2 dimensions, instead of 1
    with ott.assert_raises(TypeError):
        sample = ot.Sample(10, 2)
        interval_upper = algo.computeUnilateralConfidenceInterval(sample)
    # The sample is empty
    with ott.assert_raises(TypeError):
        sample = ot.Sample()
        interval_upper = algo.computeUnilateralConfidenceInterval(sample)
    # The sample is has 2 dimensions, instead of 1
    with ott.assert_raises(TypeError):
        sample = ot.Sample(10, 2)
        interval_bilateral = algo.computeBilateralConfidenceInterval(sample)
    # The sample is empty
    with ott.assert_raises(TypeError):
        sample = ot.Sample()
        interval_bilateral = algo.computeBilateralConfidenceInterval(sample)


# Probability of coverage
def check_empirical_coverage(rtol=1.0e-14):
    print("check_empirical_coverage ...")
    # 1. Lower bound test
    alpha = 0.05
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    size_lower = 300
    lower_bounded = True
    rank_lower = algo.computeUnilateralRank(size_lower, lower_bounded)
    coverage1 = algo.computeUnilateralCoverage(size_lower, rank_lower, lower_bounded)
    ott.assert_almost_equal(coverage1, 0.9659341864785022, rtol, 0.0)

    # 2. Upper bound test
    alpha = 0.95
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    size_upper = 300
    lower_bounded = False
    rank_upper = algo.computeUnilateralRank(size_upper, lower_bounded)
    coverage2 = algo.computeUnilateralCoverage(size_upper, rank_upper, lower_bounded)
    ott.assert_almost_equal(coverage2, 0.9659341864785024, rtol, 0.0)

    # 3. Bilateral test
    alpha = 0.95
    beta = 0.95
    algo = ot.QuantileConfidence(alpha, beta)
    size_bilateral = 100
    k1, k2 = algo.computeBilateralRank(size_bilateral)
    coverage3 = algo.computeBilateralCoverage(size_bilateral, k1, k2)
    ott.assert_almost_equal(coverage3, 0.9514463806051577, rtol, 0.0)

    # 4. Exceptions in computeUnilateralCoverage, lower case
    with ott.assert_raises(TypeError):
        alpha = 0.05
        beta = 0.95
        algo = ot.QuantileConfidence(alpha, beta)
        size_lower = 300
        rank_lower = size_lower
        lower_bounded = True
        coverage1 = algo.computeUnilateralCoverage(
            size_lower, rank_lower, lower_bounded
        )

    # 5. Exceptions in computeUnilateralCoverage, upper case
    with ott.assert_raises(TypeError):
        alpha = 0.05
        beta = 0.95
        algo = ot.QuantileConfidence(alpha, beta)
        size_lower = 300
        rank_lower = size_lower
        lower_bounded = False
        coverage1 = algo.computeUnilateralCoverage(
            size_lower, rank_lower, lower_bounded
        )

    # 6. Exceptions in computeBilateralCoverage: rank1 > rank2
    with ott.assert_raises(TypeError):
        alpha = 0.05
        beta = 0.95
        algo = ot.QuantileConfidence(alpha, beta)
        size_bilateral = 300
        k1 = 20
        k2 = 19
        coverage3 = algo.computeBilateralCoverage(size_bilateral, k1, k2)

    # 7. Exceptions in computeBilateralCoverage: rank2 >= size
    with ott.assert_raises(TypeError):
        alpha = 0.05
        beta = 0.95
        algo = ot.QuantileConfidence(alpha, beta)
        size_bilateral = 300
        k1 = 10
        k2 = 300
        coverage3 = algo.computeBilateralCoverage(size_bilateral, k1, k2)


if __name__ == "__main__":
    check_basic()
    check_lower_rank()
    check_upper_rank()
    check_bilateral_rank()
    check_bilateral_rank_n_large()
    check_interval_coverage_consistency()
    check_empirical_coverage()
    check_validate_empirical_coverage()
    check_minimum_sample_size()
    check_print_minimum_unilateral_sample_size_meeker()
    check_minimum_unilateral_sample_size()
    check_asymptotic_confidence()
    check_bilateral_rank_edge_cases()
    check_bilateral_rank_random()
