import openturns as ot
import scipy.stats as st
import math

# pPearsonCorrelation vs scipy.stats.t.cdf
# Under H0: independence, t = rho * sqrt((n-2) / (1-rho^2)) ~ t(n-2)
print("pPearsonCorrelation vs scipy.stats.t.cdf")
errMax = 0.0
for n in [5, 10, 100]:
    for rho in [-0.9, -0.5, -0.3, 0.0, 0.3, 0.5, 0.9]:
        ot_val = ot.DistFunc.pPearsonCorrelation(n, rho, False)
        t = rho * math.sqrt((n - 2.0) / (1.0 - rho * rho))
        sp_val = st.t.cdf(t, n - 2)
        err = abs(ot_val - sp_val)
        errMax = max(errMax, err)
        if err > 1e-15:
            print("  n=%d rho=%.1f OT=%g SciPy=%g err=%g" % (n, rho, ot_val, sp_val, err))
print("  max err:", errMax)

# rBinomial: empirical distribution vs scipy.stats.binom.pmf
print("\nrBinomial vs scipy.stats.binom.pmf")
ot.RandomGenerator.SetSeed(0)
n, p, nR = 20, 0.3, 50000
sample = ot.DistFunc.rBinomial(n, p, nR)
errMax = 0.0
for k in range(n + 1):
    ot_pmf = sum(1.0 for v in sample if v == k) / nR
    sp_pmf = st.binom.pmf(k, n, p)
    err = abs(ot_pmf - sp_pmf)
    errMax = max(errMax, err)
    if err > 1e-2:
        print("  k=%d OT=%g SciPy=%g err=%g" % (k, ot_pmf, sp_pmf, err))
print("  max abs err:", errMax)

# rBinomial tail probabilities vs scipy.stats.binom.cdf
cdfErr = 0.0
for k in range(n + 1):
    ot_cdf = sum(1.0 for v in sample if v <= k) / nR
    sp_cdf = st.binom.cdf(k, n, p)
    err = abs(ot_cdf - sp_cdf)
    cdfErr = max(cdfErr, err)
print("  max CDF abs err:", cdfErr)

# qDickeyFullerTrend critical values
print("\nqDickeyFullerTrend critical values")
for prob in [0.01, 0.05, 0.10]:
    ot_q = ot.DistFunc.qDickeyFullerTrend(prob, False)
    print("  level=%.2f OT q=%g" % (prob, ot_q))
