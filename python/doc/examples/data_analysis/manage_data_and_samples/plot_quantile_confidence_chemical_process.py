"""
Estimate quantile confidence intervals from data
================================================
"""

# %%
# In this example, we introduce use two methods to estimate confidence intervals of the
# :math:`\alpha` level quantile (:math:`\alpha \in [0,1]`).
# This example is adapted from [meeker2017]_ pages 76 to 81.

# %%
import openturns as ot
import openturns.experimental as otexp

# %%
# The data represents the ordered measured amount of a chemical compound in parts per million (ppm)
# taken from :math:`n=100` randomly selected batches from the process (see 5.1 data p. 76).
data = [1.49, 1.66, 2.05, 2.24, 2.29, 2.69, 2.77, 2.77, 3.10, 3.23,
        3.28, 3.29, 3.31, 3.36, 3.84, 4.04, 4.09, 4.13, 4.14, 4.16,
        4.57, 4.63, 4.83, 5.06, 5.17, 5.19, 5.89, 5.97, 6.28, 6.38,
        6.51, 6.53, 6.54, 6.55, 6.83, 7.08, 7.28, 7.53, 7.54, 7.62,
        7.81, 7.87, 7.94, 8.43, 8.70, 8.97, 8.98, 9.13, 9.14, 9.22,
        9.24, 9.30, 9.44, 9.69, 9.86, 9.99, 11.28, 11.37, 12.03, 12.32,
        12.93, 13.03, 13.09, 13.43, 13.58, 13.70, 14.17, 14.36, 14.96, 15.89,
        16.57, 16.60, 16.85, 17.16, 17.17, 17.74, 18.04, 18.78, 19.84, 20.45,
        20.89, 22.28, 22.48, 23.66, 24.33, 24.72, 25.46, 25.67, 25.77, 26.64,
        28.28, 28.28, 29.07, 29.16, 31.14, 31.83, 33.24, 37.32, 53.43, 58.11]
sample = ot.Sample.BuildFromPoint(data)

# %%
# In the example, we consider the quantile of level :math:`\alpha = 10\%`,
# with a confidence level of :math:`\beta = 95\%` (see example 5.7 p. 85).
alpha = 0.1
beta = 0.95
algo = otexp.QuantileConfidence(alpha, beta)

# %%
# Estimate bilateral rank.
# Indices are given in the :math:`\llbracket 0, n-1 \rrbracket` integer interval whereas the book gives them in :math:`\llbracket 1, n \rrbracket`.
n = len(sample)
l, u = algo.computeBilateralRank(n)
print(f"l={l} u={u}")

# %%
# Estimate the bilateral confidence interval of the 0.1 quantile from the order statistics:
#
# ..math::
#
#    \Prob{X_{(k_1)} \leq x_{\alpha} \leq X_{(k_2)}} \geq \beta
#
ci = algo.computeBilateralConfidenceInterval(sample)
print(f"ci={ci}")

# %%
# In this example, we consider the quantile of level :math:`\alpha = 90\%`,
# with a confidence level of :math:`\beta = 95\%` (see example 5.1 p. 81).
algo.setAlpha(0.9)

# %%
# Estimate the one-sided rank for the upper side (in :math:`\llbracket 0, n-1 \rrbracket`).
k = algo.computeUnilateralRank(n, True)
print(f"k={k}")

# %%
# Estimate the one-sided upper quantile confidence interval of the 0.9 quantile
ci = algo.computeUnilateralConfidenceInterval(sample, True)
print(f"ci={ci}")
