"""
Compare the distribution of quantile estimators
===============================================
"""

# %%
# In this example, we consider the quantile of level :math:`\alpha` of a distribution
# :math:`X`. The objective is to estimate some confidence intervals based on order statistics
# that bound the quantile
# :math:`x_{\alpha}` with a given confidence :math:`\beta \in [0,1]`.
#
# We define three quantile estimators which are all defined as particular order statistics:
#
# - the empirical estimator of :math:`x_{\alpha}`,
# - the estimator of an upper bound of :math:`x_{\alpha}` with confidence :math:`\beta`,
# - the estimator of an lower bound of :math:`x_{\alpha}` with confidence :math:`\beta`.
#
# We draw the distribution of each estimator.
#
# In this example, we consider the quantile of level :math:`\alpha = 95\%`,
# with a confidence level :math:`\beta = 90\%`.
#
# See  :ref:`quantile_confidence_estimation` and :ref:`quantile_asymptotic_confidence_estimation` to get theoretical details.
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv


# %%
# We consider a random variable :math:`X` of dimension 1. Its distribution has no importance but is assumed to have
# a density.
X_dist = ot.Normal(0, 1)

# %%
# We define the level :math:`\alpha` of the quantile and the confidence level :math:`\beta`.
alpha = 0.95
beta = 0.90

# %%
# The exact value of :math:`x_{\alpha}` is known. We compute it in order to compare the
# estimators to the exact value.
x_alpha_exact = X_dist.computeQuantile(alpha)[0]
print('Exact quantile = ', x_alpha_exact)

# %%
# We generate a sample of the variable.
n = 501
X_sample = X_dist.getSample(n)

# %%
# Now, we assume to know :math:`X` only through the generated sample.
#
# The empirical estimator of :math:`x_{\alpha}` is defined by the
# :math:`k_{emp}` -th order statistics, denoted
# by :math:`X_{(k_{emp})}`,
# where :math:`k_{emp} = (\lceil \sampleSize \alpha \rceil) -1`. The numerotation
# :math:`\lceil x \rceil` designs the smallest integer value that is greater
# than or equal to :math:`x` (we subtract 1 because python numbering starts at 0).
# The value of this order statistics is computed on the sample.
k_emp = int(n * alpha)
empiricalQuantile = X_sample.computeQuantile(alpha)
print('Empirical quantile = ', empiricalQuantile)
print('Empirical order statistics = ', k_emp)

# %%
# Now, we want to get the exact unilateral confidence interval that provides an upper bound of :math:`x_{\alpha}` with the confidence
# :math:`\beta`, which means that :math:`\Prob{X_{(k_{up})} \geq x_{\alpha}} \geq \beta`.
# This can be done using :class:`~openturns.experimental.QuantileConfidence`.
# We can get the order :math:`k_{up}` of the useful statistics. Care that the enumeration begins at 0.
quantConf = otexp.QuantileConfidence(alpha, beta)
k_up = quantConf.computeUnilateralRank(n)

# %%
# We can directly get the order statistics :math:`X_{(k_{up})}` evaluated on the sample.
upper_bound = quantConf.computeUnilateralConfidenceInterval(X_sample)
print('Upper bound of the quantile = ', upper_bound)
print('Upper bound order statistics = ', k_up)

# %%
# To get the exact unilateral confidence interval that provides a lower bound of :math:`x_{\alpha}` with the confidence
# :math:`\beta`, which means that :math:`\Prob{X_{(k_{low})} \leq x_{\alpha}} \geq \beta`.
# This can also be done using :class:`~openturns.experimental.QuantileConfidence`.
# We can get the order :math:`k_{low}` of the useful statistics. Care: the enumeration begins at 0 here!
k_low = quantConf.computeUnilateralRank(n, True)

# %%
# We can directly get the order statistics :math:`X_{(k_{low})}` evaluated on the sample.
lower_bound = quantConf.computeUnilateralConfidenceInterval(X_sample, True)
print('Lower bound of the quantile = ', lower_bound)
print('Lower bound order statistics = ', k_low)

# %%
# In order to draw the distribution of any order statistics of :math:`X`, we first create the distribution
# of the :math:`\sampleSize` order statistics of :math:`X`, denoted by :math:`\vect{X}_{(0:\sampleSize-1)} = (X_{(0)},\dots,X_{(\sampleSize-1)})`.
# This distribution as the following cumulative distribution function:
#
# .. math::
#
#    F_{\vect{X}_{(0:\sampleSize-1)}}(\vect{x}) = F_{\vect{U}_{(0:\sampleSize-1)}}(F(x_1), \dots, F(x_{\sampleSize}-1))
#
#
# and its PDF (if defined) by:
#
# .. math::
#
#    f_{\vect{X}_{(0:\sampleSize-1)}}(\vect{x}) = \sampleSize!\prod_{i=0}^{\sampleSize-1} f(x_i) \,\mathbf{1}_{\cS}(\vect{x})
#
# where :math:`F` and :math:`f` are respectively the cumulative distribution function and
# probability density function of :math:`X` and :math:`\cS \subset \Rset^\sampleSize` is defined by:
#
# .. math::
#
#   \cS=\left\{(x_0, \dots, x_{\sampleSize-1}) \in [0,1]^\sampleSize\,|\,0 \leq x_0 \leq \dots \leq x_{\sampleSize-1} \leq 1 \right\}.
#
# Thus :math:`\vect{X}_{(0:\sampleSize-1)}` is defined as the joint distribution which marginals are all distributed
# as :math:`X` and which core is the order statistics distribution of the Uniform(0,1) distribution.
# The  order statistics distribution of the Uniform(0,1) is created using the class
# :class:`~openturns.UniformOrderStatistics` which only requires to define the dimension of the
# order statistics distribution.
unif_orderStat = ot.UniformOrderStatistics(n)
normal_orderStat = ot.JointDistribution([X_dist] * n, unif_orderStat)

# %%
# Thus, the order statistics :math:`X_{(k_{emp})}`, :math:`X_{(k_{low})}` and :math:`X_{(k_{up})}` are the marginals
# :math:`k_{emp}`, :math:`k_{low}` and :math:`k_{up}` of :math:`\vect{X}_{(0:\sampleSize-1)}`.
X_emp = normal_orderStat.getMarginal(k_emp)
X_up = normal_orderStat.getMarginal(k_up)
X_low = normal_orderStat.getMarginal(k_low)

# %%
# Now we can draw the pdf.
xMin = x_alpha_exact - 0.5
xMax = x_alpha_exact + 1
nPoints = 1001
g = X_emp.drawPDF(xMin, xMax, nPoints)
g.add(X_up.drawPDF(xMin, xMax, nPoints))
g.add(X_low.drawPDF(xMin, xMax, nPoints))

# %%
# We add the exact quantile.
line_exactQuant = ot.Curve([[x_alpha_exact, 0.0], [x_alpha_exact, X_emp.computePDF([x_alpha_exact])]])
line_exactQuant.setLineStyle('dashed')
line_exactQuant.setLineWidth(2)
line_exactQuant.setColor('red')
g.add(line_exactQuant)

# %%
# Before viewing the graph, we want to fill the area that corresponds to the events:
#
# .. math::
#
#     \left \{ X_{(k_{up})} \leq x_{\alpha} \right \} \\
#     \left \{ X_{(k_{low})} \geq x_{\alpha} \right \}
#
# Each event has the probability :math:`1- \beta`.
#
# To do that, we define a function that returns a sample created from a regular grid from xmin to xmax with npoints points.


def linearSample(xmin, xmax, npoints):
    step = (xmax - xmin) / (npoints - 1)
    rg = ot.RegularGrid(xmin, step, npoints)
    vertices = rg.getVertices()
    return vertices


# %%
# Then we use it to define some :class:`~openturns.Polygon` that will be filled. These polygons
# define the area under the pdf line. Thus, the lower vertical bound is 0 and the upper vertical bound is the PDF line.
#
# First the area for the upper bound.
nplot = 100
x_list = linearSample(xMin, x_alpha_exact, nplot)
y_list = X_up.computePDF(x_list)
vLow = [0.0] * nplot
vUp = [y_list[i, 0] for i in range(nplot)]
boundsPoly_upper = ot.Polygon.FillBetween(x_list.asPoint(), vLow, vUp)
boundsPoly_upper.setColor(g.getColors()[1])
g.add(boundsPoly_upper)


# %%
# Then the area for the lower bound.
nplot = 100
x_list = linearSample(x_alpha_exact, xMax, nplot)
y_list = X_low.computePDF(x_list)
vLow = [0.0] * nplot
vUp = [y_list[i, 0] for i in range(nplot)]
boundsPoly_lower = ot.Polygon.FillBetween(x_list.asPoint(), vLow, vUp)
boundsPoly_lower.setColor(g.getColors()[2])
g.add(boundsPoly_lower)

# %%
g.setLegends([r'$X_{emp}$', r'$X_{up} (\beta = $' + str(beta) + ')', r'$X_{low} (\beta = $' + str(beta) + ')', 'exact quantile'])
g.setLegendPosition('topright')

# %%
g.setTitle('Quantile estimation and confidence intervals')
g.setXTitle('x')
view = otv.View(g)

# %%
# Display all the graphs.
otv.View.ShowAll()


# %%
# Now we can conclude that:
#
#  * the empirical estimator is centered on the true quantile,
#  * the :math:`X_{(k_{up})}` estimator has a probability :math:`\beta` to be above the true quantile,
#  * the :math:`X_{(k_{low})}` estimator has a probability :math:`\beta` to be under the true quantile.
#
