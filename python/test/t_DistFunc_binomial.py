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
