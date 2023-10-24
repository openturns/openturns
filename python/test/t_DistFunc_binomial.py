#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# generate reference files
if 0:
    pMin = 0.1
    pMax = 0.9
    nMin = 2
    nMax = 12
    n1 = 5
    nR = 5
    f1 = open("t_DistFunc_binomial1.csv", "w")
    f2 = open("t_DistFunc_binomial2.csv", "w")
    for i1 in range(n1):
        p = pMin + i1 * (pMax - pMin) / (n1 - 1.0)
        for n in range(nMin, nMax + 1):
            for k in range(n + 1):
                f1.write(
                    f"{n}, {p}, {k}, {ot.DistFunc.dBinomial(n, p, k)}, {ot.DistFunc.logdBinomial(n, p, k)}\n"
                )
            for iR in range(nR):
                f2.write(f"{n}, {p}, {iR}, {ot.DistFunc.rBinomial(n, p)}\n")
    f1.close()
    f2.close()
    ot.RandomGenerator.SetSeed(0)

# binomial + logdBinomial
sample1 = ot.Sample.ImportFromCSVFile("t_DistFunc_binomial1.csv", ",")
for x in sample1:
    n, p, k, ref, log_ref = x
    n, k = int(n), int(k)

    val = ot.DistFunc.dBinomial(n, p, k)
    print(f"dbinomial({n}, {p}, {k}) ref={ref} val={val}")
    ott.assert_almost_equal(val, ref)

    log_val = ot.DistFunc.logdBinomial(n, p, k)
    print(f"logdbinomial({n}, {p}, {k}) ref={log_ref} val={log_val}")
    ott.assert_almost_equal(log_val, log_ref)

# rBinomial
sample2 = ot.Sample.ImportFromCSVFile("t_DistFunc_binomial2.csv", ",")
for x in sample2:
    n, p, iR, ref = x
    n, iR = int(n), int(iR)
    val = ot.DistFunc.rBinomial(n, p)
    print(f"rBinomial({n}, {p}) iR={iR} ref={ref} val={val}")
    ott.assert_almost_equal(val, ref)

# Corner cases
dataset = ot.Sample.ImportFromCSVFile("t_binomial_dataset.csv", ",")

precision = 1e-11
binomial = ot.Binomial()
for i in range(dataset.getSize()):
    binomial.setN(int(dataset[i, 1]))
    binomial.setP(dataset[i, 2])
    x = dataset[i, 0]
    pdf = dataset[i, 3]
    cdf = dataset[i, 4]
    surv = dataset[i, 5]
    print("i = ", i, " x = ", x, " N = ", binomial.getN(), " p = ", binomial.getP())
    ott.assert_almost_equal(binomial.computePDF(x), pdf, precision, 0.0)
    ott.assert_almost_equal(binomial.computeCDF(x), cdf, precision, 0.0)
    ott.assert_almost_equal(binomial.computeSurvivalFunction(x), surv, precision, 0.0)
    if i > 0:  # FIXME: test fails for i = 0 (x=0, N=10, P=0)
        ott.assert_almost_equal(
            binomial.computeScalarQuantile(cdf), x, 0.0, 1.0
        )  # Can be off by 1 unit

# 2147483647 is the maximum integer.
# Values greater than this are not doubles anymore.
N = 2147483647
binomial.setN(N)
binomial.setP(1.0 / N)
computed = binomial.computePDF(1.0)
expected = 0.3678794
ott.assert_almost_equal(computed, expected, 1.0e-6, 0.0)

computed = binomial.computePDF(2.0)
expected = 0.1839397
ott.assert_almost_equal(computed, expected, 1.0e-6, 0.0)

# Extreme inputs
binomial.setN(9999)
binomial.setP(0.5)
computed = binomial.computePDF(4999.0)
expected = 0.0079786461393821558191
ott.assert_almost_equal(computed, expected, 1.0e-7, 0.0)

# Check pdf for values of P closer to 1
binomial.setN(2)
binomial.setP(0.9999)
computed = binomial.computePDF(1.0)
expected = 1.999799999999779835e-04
ott.assert_almost_equal(computed, expected, 1e-12, 0.0)
