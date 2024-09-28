"""
Create a mixture of distributions
=================================
"""
# %%
#
# Introduction
# ------------
#
# In this example, we show how to build a distribution which is a Mixture from a collection of
# distributions of the same dimension :math:`d \geq 1`. We denote by :math:`\inputRV`
# the random vector with such a distribution.
#
# Let :math:`(\cL_1, \dots, \cL_N)` a collection of distributions and :math:`(\alpha_1, \dots,
# \alpha_N)` a collection of positive weights such that :math:`\sum_{i=1}^N \alpha_i = 1`.
# We denote by :math:`\inputRV_i` the random vector whose distribution is :math:`\cL_i`.
#
# In the case where all the :math:`\cL_i` have a probability density function :math:`\mu_i`,
# then the mixture has a probability density function :math:`\inputMeasure` defined by:
#
# .. math::
#    \inputMeasure(\vect{x}) =  \sum_{i=1}^N \alpha_i \mu_i(\vect{x})
#
# In the case where all the :math:`\cL_i` are discrete, then the mixture is discrete and its
# probability distribution function is defined by:
#
# .. math::
#    \Prob{\inputRV = \vect{x}} =  \sum_{i=1}^N \alpha_i \Prob{\vect{X}_i = \vect{x}}
#
# In the case where some of the :math:`\cL_i`  have a probability density function and
# the other ones are discrete, then the mixture is not discrete and does not have a
# probability density function. Its cumulated distribution function is defined by:
#
# .. math::
#    F_\inputRV(\vect{x}) =  \sum_{i=1}^N \alpha_i F_{\vect{X}_i}(\vect{x})
#
# We illustrate the following particular cases:
#
# - Case 1a: Mixture of continuous distributions,
# - Case 1b: Mixture of copulas,
# - Case 1c: Mixture of a Histogram and a Generalized Pareto Distribution,
# - Case 2: Mixture of discrete distributions,
# - Case 3: Mixture of discrete and continuous distributions.

# %%
import openturns as ot
import openturns.viewer as otv


# %%
# Case 1a: Mixture of continuous distributions
# --------------------------------------------
# In this case, we build the mixture of the following continuous distributions:
#
# - a :class:`~openturns.Triangular`,
# - a :class:`~openturns.Normal`,
# - a :class:`~openturns.Uniform`.
#
# The weigths are automatically normalized.

# %%
# We define the collection of distributions and the associated weights.
distributions = [
    ot.Triangular(1.0, 2.0, 4.0),
    ot.Normal(-1.0, 1.0),
    ot.Uniform(5.0, 6.0),
]
weights = [0.4, 1.0, 0.2]

# %%
# We create the mixture.
distribution = ot.Mixture(distributions, weights)
print(distribution)

# %%
# We can draw the probability density function.
graph = distribution.drawPDF()
graph.setTitle('Mixture of Triangular, Normal, Uniform')
graph.setXTitle('x')
graph.setLegendPosition('')
view = otv.View(graph)

# %%
# We can draw the cumulated distribution function.
graph = distribution.drawCDF()
graph.setTitle('Mixture of Triangular, Normal, Uniform')
graph.setXTitle('x')
graph.setLegendPosition('')
view = otv.View(graph)

# %%
# Case 1b: Mixture of copulas
# ---------------------------
# In this case, we build the mixture of the following copulas:
#
# - a :class:`~openturns.GumbelCopula`,
# - a :class:`~openturns.ClaytonCopula`.

# %%
# We define the collection of copulas and the associated weights.
copulas = [ot.GumbelCopula(4.5), ot.ClaytonCopula(2.3)]
weights = [0.2, 0.8]

# %%
# We create a mixture of copulas.
distribution = ot.Mixture(copulas, weights)
print(distribution)

# %%
# We can draw the probability density function.
graph = distribution.drawPDF()
graph.setTitle('Mixture of Gumbel copula, Clayton copula')
graph.setXTitle(r'$x_0$')
graph.setYTitle(r'$x_1$')
graph.setLegendPosition('')
view = otv.View(graph)

# %%
# We can draw the cumulated distribution function.
graph = distribution.drawCDF()
graph.setTitle('Mixture of Gumbel copula, Clayton copula')
graph.setXTitle(r'$x_0$')
graph.setYTitle(r'$x_1$')
view = otv.View(graph)


# %%
# Case 1c: Mixture of a Histogram and a Generalized Pareto Distribution
# ---------------------------------------------------------------------
# We want to create the scalar distribution of :math:`X` such that:
#
# .. math::
#    X|X \leq x_0 & \sim \mathcal{L}_1 \\
#    X|X \geq x_0 & \sim \mathcal{L}_2
#
# where:
#
# - :math:`\mathcal{L}_1` is a Histogram,
# - :math:`\mathcal{L}_2` is a Generalized Pareto distribution (GPD),
# - :math:`x_0` is a quantile of high level of :math:`X`.
#
# We denote by:
#
# .. math::
#    w = \Prob{X \leq x_0}
#
# We assume that we only have a sample from :math:`X`.

# %%
# In this example, we consider a Normal distribution with zero mean and unit variance.
# We generate a sample of size :math:`n`.
n = 5000
X_dist = ot.Normal()
sample_X = X_dist.getSample(n)

# %%
# We build the whole histogram from the sample.
hist_dist = ot.HistogramFactory().build(sample_X)
g_hist = hist_dist.drawPDF()
g_hist.setTitle(r'Empirical distribution of $X$')
g_hist.setXTitle('x')
g_hist.setYTitle('pdf')
g_hist.setLegends(['histogram'])
view = otv.View(g_hist)

# %%
# We estimate the extreme empirical quantile of level 0.95.
w = 0.95
x0 = hist_dist.computeQuantile(w)[0]

# %%
# We start by truncating the initial histogram on the interval :math:`]-\infty, x_0]`.
# We visualize it!
hist_trunc = ot.TruncatedDistribution(hist_dist, x0, ot.TruncatedDistribution.UPPER)
g_hist_trunc = hist_trunc.drawPDF()
g_hist_trunc.setTitle(r'Empirical distribution of $X|X \leq $' + "%.3g" % (x0))
g_hist_trunc.setXTitle('x')
g_hist_trunc.setYTitle('pdf')
g_hist_trunc.setLegends(['truncated histogram'])
view = otv.View(g_hist_trunc)

# %%
# Then we model :math:`X|X \geq x_0` by a Generalized Pareto distribution (GPD). We start by
# extracting from the sample all the values greater than :math:`x_0` to build the upper
# sample. We get about :math:`n(1-w)` points.
sample_X_upper = ot.Sample(0,1)
for i in range(len(sample_X)):
    if sample_X[i,0] > x0:
        sample_X_upper.add(sample_X[i])

print('Excess number = ', sample_X_upper.getSize())
print('n(1-w) = ', int(n*(1-w)))

# %%
# Then we fit a GPD parameterized by :math:`(\sigma, \xi, x_0)`: the
# threshold is fixed to :math:`x_0`.
# We use the estimator that maximizes the likelihood.
# To help the optimisation problem to converge, we start by estimating the 3 parameters
# :math:`(\sigma, \xi, u)` from the upper sample. Then we fix the threshold to :math:`x_0`
# and we estimate the remaining parameters :math:`(\sigma, \xi)` using the previous values of
# :math:`(\sigma, \xi)` as a starting point to the optimization problem.
# We visualize the pdf of the GPD.
gpd_first = ot.GeneralizedParetoFactory().build(sample_X_upper)
mlFact = ot.MaximumLikelihoodFactory(gpd_first)

# we fix the threshold to :math:`x_0`.
mlFact.setKnownParameter([x0], [2])
gpd_estimated = mlFact.build(sample_X_upper)
print('estimated gpd = ', gpd_estimated)

g_gpd = gpd_estimated.drawPDF()
g_gpd.setTitle(r'Distribution of $X|X \geq $' + "%.3g" % (x0))
g_gpd.setXTitle('x')
g_gpd.setYTitle('pdf')
g_gpd.setLegends(['GPd'])
view = otv.View(g_gpd)

# %%
# Then we can create the mixture using the truncated Histogram distribution below :math:`x_0`
# and the GPD over :math:`x_0` weighted by :math:`w` and :math:`(1-w)`.
mixt_dist = ot.Mixture([hist_trunc, gpd_estimated], [w, 1-w])
g_hist.add(mixt_dist.drawPDF())

ord_Max = max(hist_dist.getImplementation().getHeight())
line = ot.Curve([x0, x0], [0.0, ord_Max])
line.setColor('red')
line.setLineStyle('dashed')
g_hist.add(line)

draw_ref = X_dist.drawPDF().getDrawable(0)
draw_ref.setLineStyle('dashed')
g_hist.add(draw_ref)

g_hist.setLegends(['histogram', 'mixture', '', 'exact dist'])
g_hist.setTitle(r'Distribution of $X$: Mixture')

# sphinx_gallery_thumbnail_number = 8
view = otv.View(g_hist)

# We draw here only the mixture distribution to make the comparison easier.
g_mixt = mixt_dist.drawPDF()
g_mixt.setTitle(r'Mixture distribution of $X$')
g_mixt.setXTitle('x')
g_mixt.setYTitle('pdf')
g_mixt.setLegendPosition('')
view = otv.View(g_mixt)


# %%
# Case 2: Mixture of discrete distributions
# -----------------------------------------
# In this case, we build the mixture of the following distributions:
#
# - a :class:`~openturns.Poisson`,
# - a :class:`~openturns.Geometric`.
#
# The weigths are automatically normalized.

# %%
# We define the collection of distributions and the associated weights.
distributions = [
    ot.Poisson(1.2),
    ot.Geometric(0.7)
]
weights = [0.4, 1.0]

# %%
# We create the mixture.
distribution = ot.Mixture(distributions, weights)
print(distribution)

# %%
# We can draw the probability distribution function.
graph = distribution.drawPDF()
graph.setTitle('Mixture of Poisson, Geometric')
graph.setXTitle('x')
graph.setLegendPosition('')
view = otv.View(graph)

# %%
# We can draw the cumulated distribution function.
graph = distribution.drawCDF()
graph.setTitle('Mixture of Poisson, Geometric')
graph.setXTitle('x')
graph.setLegendPosition('')
view = otv.View(graph)


# %%
# Case Case 3: Mixture of discrete and continuous distributions
# -------------------------------------------------------------
# In this case, we build the mixture of the following distributions:
#
# - a :class:`~openturns.Normal`,
# - a :class:`~openturns.Poisson`.
#
# The resulting distribution is not continuous nor discrete. It not possible to
# draw the pdf ...

# %%
# We define the collection of distributions and the associated weights.
distributions = [
    ot.Normal(),
    ot.Poisson(0.7)
]
weights = [0.4, 1.0]

# %%
# We create the mixture.
distribution = ot.Mixture(distributions, weights)
print(distribution)

# %%
# We can not draw the probability distribution function as it is not defined.
# But, we can draw the cumulated distribution function.
graph = distribution.drawCDF()
graph.setTitle('Mixture of Normal, Poisson')
graph.setXTitle('x')
graph.setLegendPosition('')
view = otv.View(graph)

# %%
# Reset ResourceMap
ot.ResourceMap.Reload()

# %%
# Show all the graphs.
view.ShowAll()
