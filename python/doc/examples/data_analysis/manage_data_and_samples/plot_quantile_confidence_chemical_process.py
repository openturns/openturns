"""
Estimate quantile confidence intervals from chemical process data
=================================================================
"""

# %%
# In this example, we introduce use two methods to estimate confidence intervals of the
# :math:`\alpha` level quantile (:math:`\alpha \in [0,1]`).
# This example is adapted from [meeker2017]_ pages 76 to 81.
#
# See  :ref:`quantile_confidence_estimation`  and :ref:`quantile_asymptotic_confidence_estimation` to get details on the signification of these confidence interval.
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

# %%
# The data represents the ordered measured amount of a chemical compound in parts per million (ppm)
# taken from :math:`n=100` randomly selected batches from the process (see 5.1 data p. 76).
data = [
    1.49,
    1.66,
    2.05,
    2.24,
    2.29,
    2.69,
    2.77,
    2.77,
    3.10,
    3.23,
    3.28,
    3.29,
    3.31,
    3.36,
    3.84,
    4.04,
    4.09,
    4.13,
    4.14,
    4.16,
    4.57,
    4.63,
    4.83,
    5.06,
    5.17,
    5.19,
    5.89,
    5.97,
    6.28,
    6.38,
    6.51,
    6.53,
    6.54,
    6.55,
    6.83,
    7.08,
    7.28,
    7.53,
    7.54,
    7.62,
    7.81,
    7.87,
    7.94,
    8.43,
    8.70,
    8.97,
    8.98,
    9.13,
    9.14,
    9.22,
    9.24,
    9.30,
    9.44,
    9.69,
    9.86,
    9.99,
    11.28,
    11.37,
    12.03,
    12.32,
    12.93,
    13.03,
    13.09,
    13.43,
    13.58,
    13.70,
    14.17,
    14.36,
    14.96,
    15.89,
    16.57,
    16.60,
    16.85,
    17.16,
    17.17,
    17.74,
    18.04,
    18.78,
    19.84,
    20.45,
    20.89,
    22.28,
    22.48,
    23.66,
    24.33,
    24.72,
    25.46,
    25.67,
    25.77,
    26.64,
    28.28,
    28.28,
    29.07,
    29.16,
    31.14,
    31.83,
    33.24,
    37.32,
    53.43,
    58.11,
]
sample = ot.Sample.BuildFromPoint(data)

# %%
# In the example, we consider the quantile of level :math:`\alpha = 10\%`,
# with a confidence level of :math:`\beta = 95\%` (see example 5.7 p. 85).
alpha = 0.1
beta = 0.95
algo = otexp.QuantileConfidence(alpha, beta)

# %%
# Estimate bilateral rank: math:`(l,u') such that :math:`0 \leq l \< u \leq \sampleSize -1`
# and defined by:
#
# .. math::
#
#    \begin{array}{ll}
#    (l,u) = & \argmin \Prob{X_{(l)} \leq x_{\alpha} \leq X_{(u)}}\\
#           & \mbox{s.t.} \Prob{X_{(l)} \leq x_{\alpha} \leq X_{(u)}} \geq \beta
#    \end{array}
#
# Care: indices are given in the :math:`\llbracket 0, n-1 \rrbracket` integer interval whereas the book gives them in :math:`\llbracket 1, n \rrbracket`.
n = len(sample)
l, u = algo.computeBilateralRank(n)
print(f"l={l} u={u}")

# %%
# We can directly estimate the bilateral confidence interval of the 0.1 quantile from the order statistics defined the order statistics :math:`X_{(l)}` and :math:`X_{(u)}`.
ci = algo.computeBilateralConfidenceInterval(sample)
print(f"ci={ci}")

# %%
# In this example, we consider the quantile of level :math:`\alpha = 90\%`,
# with a confidence level of :math:`\beta = 95\%` (see example 5.1 p. 81).
new_alpha = 0.90
algo.setAlpha(new_alpha)

# %%
# We get the bilateral confidence interval of the 0.91 quantile.
l, u = algo.computeBilateralRank(n)
print(f"l={l} u={u}")

# %%
ci = algo.computeBilateralConfidenceInterval(sample)
print(f"ci={ci}")

# %%
# We can estimate the rank :math:`k_{low}` which is the largest rank :math:`k`  with
# :math:`0 \leq k \leq \sampleSize -1` such that:
#
# .. math::
#
#    \Prob{X_{(k)} \leq x_{\alpha}} \geq \beta.
#
# We notice that the order statistics of the lower bound is the same as in the bilateral confidence interval.
k_low = algo.computeUnilateralRank(n, True)
print(f"k_low={k_low}")

# %%
# In other words, the interval :math:`\left[ X_{(k_{low})}, +\infty\right[` is a unilateral
# confidence interval for the 0.9 quantile with confidence :math:`\beta`. We can directly
# estimate this interval.
ci_low = algo.computeUnilateralConfidenceInterval(sample, True)
print(f"ci_low={ci_low}")

# %%
# We now estimate the rank :math:`k_{up}` which is the smallest rank :math:`k`  with
# :math:`0 \leq k \leq \sampleSize -1` such that:
#
# .. math::
#
#    \Prob{X_{(k)} \geq x_{\alpha}} \geq \beta.
#
# We notice that the order statistics of the upper bound is slightly smaller than in the bilateral confidence interval.
k_up = algo.computeUnilateralRank(n)
print(f"k_up={k_up}")

# %%
# In other words, the interval :math:`\left]-\infty,  X_{(k_{up})}\right]` is a unilateral
# confidence interval for the 0.9 quantile with confidence :math:`\beta`. We can directly
# estimate this interval.
ci_up = algo.computeUnilateralConfidenceInterval(sample)
print(f"ci_up={ci_up}")

# %%
# We had the empirical estimation of the quantile, with the order statistics :math:`X_{(\lfloor \sampleSize \alpha, \rfloor)}`.
emp_quant = sample.computeQuantile(new_alpha)[0]

# %%
# We illustrate here the confidence intervals we obtained. To do that, we
# draw the empirical cumulative distribution function and the bounds of the bilateral confidence intervals.
# We first draw the empirical cumulative distribution function.
user_defined_dist = ot.UserDefined(sample)
g = user_defined_dist.drawCDF(sample.getMin(), sample.getMax()*1.5)

# %%
# Then we had the bounds of the bilateral confidence intervals.
# First the bilateral interval.
line_bil_low = ot.Curve(
    [ci.getLowerBound(), ci.getLowerBound()],
    [[0.0], [user_defined_dist.computeCDF(ci.getLowerBound())]],
)
line_bil_low.setLineStyle("dashed")

# %%
line_bil_up = ot.Curve(
    [ci.getUpperBound(), ci.getUpperBound()],
    [[0.0], [user_defined_dist.computeCDF(ci.getUpperBound())]],
)
line_bil_up.setLineStyle("dashed")
line_bil_up.setColor(line_bil_low.getColor())

# %%
g.add(line_bil_low)
g.add(line_bil_up)

# %%
# Then the unilateral confidence intervals.
line_unil_low = ot.Curve(
    [ci_low.getLowerBound(), ci_low.getLowerBound()],
    [[0.0], [user_defined_dist.computeCDF(ci_low.getLowerBound())]],
)
line_unil_low.setLineStyle("dotted")

# %%
line_unil_up = ot.Curve(
    [ci_up.getUpperBound(), ci_up.getUpperBound()],
    [[0.0], [user_defined_dist.computeCDF(ci_up.getUpperBound())]],
)
line_unil_up.setLineStyle("dashed")

# %%
g.add(line_unil_low)
g.add(line_unil_up)

# %%
# At last, the empirical estimation.
line_emp = ot.Curve(
    [[emp_quant], [emp_quant], [0.0]],
    [
        [0.0],
        [user_defined_dist.computeCDF(emp_quant)],
        [user_defined_dist.computeCDF(emp_quant)],
    ],
)
line_bil_up.setLineStyle("dashed")

# %%
g.add(line_emp)

# %%
# We add some labels.
text_bil_low = ot.Text([[ci.getLowerBound()[0], -0.1]], ["bilat. low. bound"])
text_bil_up = ot.Text([[ci.getUpperBound()[0], -0.1]], ["bilat. up. bound"])
text_low = ot.Text([[ci_low.getLowerBound()[0], -0.05]], ["unilat. low. bound"])
text_up = ot.Text([[ci_up.getUpperBound()[0], -0.05]], ["unilat. up. bound"])
text_emp = ot.Text([[emp_quant, -0.075]], ["emp quant"])
text_emp_2 = ot.Text([[-0.1, 0.9]], ["quant level"])
g.add(text_bil_low)
g.add(text_bil_up)
g.add(text_low)
g.add(text_up)
g.add(text_emp)
g.add(text_emp_2)

# %%
g.setColors(
    [
        "#1f77b4",
        "#ff7f0e",
        "#ff7f0e",
        "#9467bd",
        "#d62728",
        "black",
        "#ff7f0e",
        "#ff7f0e",
        "#9467bd",
        "#d62728",
        "black",
        "black",
    ]
)
g.setLegends(
    [
        "Emp. CDF",
        "Bilat CI: lower bound",
        "Bilat CI: upper bound",
        "Unilat CI: lower bound",
        "Unilat CI: upper bound",
        "Emp quant",
    ]
)
g.setLegendPosition("bottomright")
g.setTitle("Estimation of the quantile of level 0.9")
g.setXTitle("x")
view = otv.View(g)

# %%
# Display all the graphs.
otv.View.ShowAll()
