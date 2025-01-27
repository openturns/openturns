"""
Sampling from an unscaled probability density
=============================================
"""

# %%
# In this example, we show different ways to sample a distribution which PDF is known up to a normalization constant:
#
# - Case 1: computing the normalization factor and using a :class:`~openturns.PythonDistribution`,
# - Case 2: using the Ratio of Uniforms algorithm,
# - Case 3: using some Metropolis-Hastings algorithms.
#
# Consider a distribution whose
# probability density function :math:`p` is known up to a normalization factor:
# let :math:`f` be a function such that
# :math:`p = cf` with :math:`c \in \Rset^+_*`.
#
# We illustrate the case with:
#
# .. math::
#
#     f(x) = \frac{1}{2} (2 + \sin(x)^2) \exp \left[- \left(2 + \cos(3x)^3 + \sin(2x)^3 \right) x
#     \right]  \mathbf{1}_{[0, 2 \pi]}(x).
#
# First, we draw the unscaled probability density function.

import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from math import pi
from time import monotonic

ot.RandomGenerator.SetSeed(1)
f = ot.SymbolicFunction(
    "x", "0.5 * (2 + sin(x)^2) * exp( -( 2 + cos(3*x)^3 + sin(2*x)^3) * x )"
)
lower_bound = 0.0
upper_bound = 2.0 * pi
rangePDF = ot.Interval(lower_bound, upper_bound)
graph = f.draw(lower_bound, upper_bound, 512)
graph.setTitle("Christian Robert tough density")
graph.setXTitle("x")
graph.setYTitle(r"$f(x)$")
view = otv.View(graph)


# %%
# Case 1: Computation of the normalization factor
# -----------------------------------------------
# The best thing to do is to compute the normalization factor thanks to the integration algorithms of the library.
#
# We show how to compute the normalization factor using a :class:`~openturns.GaussKronrod` quadrature formula.
norm_fact = ot.GaussKronrod().integrate(f, rangePDF)[0]
print('normalization factor = ', norm_fact)

# %%
# Thus, we can define the exact PDF expression:
#
# .. math::
#
#   p(x) = \dfrac{f(x)}{\int_0^{2\pi} f(u)\, du}
#
exact_PDF = ot.LinearCombinationFunction([f], [1.0 / norm_fact])

# %%
# Then we define a :class:`~openturns.PythonDistribution` which:
#
# - *computePDF* is computed from the exact expression,
# - *computeCDF* is computed using an integration algorithm on the *computePDF*.
#
# Doing that way, we use the generic sampler of the distribution, based on the CDF inversion method,
# implemented in the *getSample* method.


class NewDistribution(ot.PythonDistribution):
    def __init__(self):
        super(NewDistribution, self).__init__(1)
        self.PDF_ = exact_PDF
        self.logPDF_ = ot.ComposedFunction(ot.SymbolicFunction('x', 'log(x)'), self.PDF_)
        self.rangeLow_ = 0.0
        self.rangeUp_ = 2.0 * pi

    def getRange(self):
        return ot.Interval(self.rangeLow_, self.rangeUp_)

    def computeLogPDF(self, X):
        if X[0] < self.rangeLow_ or X[0] >= self.rangeUp_:
            return -ot.SpecFunc.Infinity
        return self.logPDF_(X)[0]

    def computePDF(self, X):
        if X[0] < self.rangeLow_ or X[0] >= self.rangeUp_:
            return 0.0
        return self.PDF_(X)[0]

    def computeCDF(self, X):
        if X[0] < self.rangeLow_:
            return 0.0
        if X[0] >= self.rangeUp_:
            return 1.0
        return ot.GaussLegendre([32]).integrate(self.PDF_, ot.Interval(self.rangeLow_, X[0]))[0]


# %%
# We create the new distribution that used the generic sampler:
newDist_generic = ot.Distribution(NewDistribution())

# %%
# We can change the sampler of that new distribution by implementing the method *getSample* and *getRealization* as follows:


class NewDistribution_RoU(ot.PythonDistribution):
    def __init__(self):
        super(NewDistribution_RoU, self).__init__(1)
        self.PDF_ = exact_PDF
        self.logPDF_ = ot.ComposedFunction(ot.SymbolicFunction('x', 'log(x)'), self.PDF_)
        self.rangeLow_ = 0.0
        self.rangeUp_ = 2.0 * pi
        self.sampler_ = otexp.RatioOfUniforms(self.logPDF_, self.getRange())

    def getRange(self):
        return ot.Interval(self.rangeLow_, self.rangeUp_)

    def computeLogPDF(self, X):
        if X[0] < self.rangeLow_ or X[0] >= self.rangeUp_:
            return -ot.SpecFunc.Infinity
        return self.logPDF_(X)[0]

    def computePDF(self, X):
        if X[0] < self.rangeLow_ or X[0] >= self.rangeUp_:
            return 0.0
        return self.PDF_(X)[0]

    def computeCDF(self, X):
        if X[0] < self.rangeLow_:
            return 0.0
        if X[0] >= self.rangeUp_:
            return 1.0
        return ot.GaussLegendre([32]).integrate(self.PDF_, ot.Interval(self.rangeLow_, X[0]))[0]

    def getRealization(self):
        return self.sampler_.getRealization()

    def getSample(self, n):
        return self.sampler_.getSample(n)

# %%
# We create the new distribution that used the Ratio of Uniforms sampler:


newDist_RoU = ot.Distribution(NewDistribution_RoU())

# %%
# We compare the time for sampling the distribution with the Ratio of Uniforms algorithm to
# the time with the generic sampler. The Ratio of Uniforms algorithm proves to be much more rapid
# than the generic method.
sizeRoU = 10000
sizeGeneric = 100

t0 = monotonic()
sample_RoU = newDist_RoU.getSample(sizeRoU)
t1 = monotonic()
sample_generic = newDist_generic.getSample(sizeGeneric)
t2 = monotonic()

print('Ratio of Uniforms speed = ', sizeRoU / (t1 - t0), 'rng/s')
print('Generic algorithm speed = ', sizeGeneric / (t2 - t1), 'rng/s')

# %%
# Case 2: Direct use the Ratio of Uniforms algorithm
# --------------------------------------------------
# In that case, we want to use the :class:`~openturns.experimental.RatioOfUniforms` algorithm to
# sample :math:`p`. We need to compute the function :math:`\log f` and its range.
#
# We create the function :math:`\log f` and the :class:`~openturns.experimental.RatioOfUniforms`:
log_UnscaledPDF = ot.ComposedFunction(ot.SymbolicFunction('x', 'log(x)'), f)
ratioAlgo = otexp.RatioOfUniforms(log_UnscaledPDF, rangePDF)

# %%
# We can draw the exact PDF and the PDF estimated from a sample generated by the Ratio of Uniforms algorithm.

# %%
# sphinx_gallery_thumbnail_number = 2
size = 100000
sample = ratioAlgo.getSample(size)

ks_algo = ot.KernelSmoothing()
ks_algo.setBoundaryCorrection(True)
ks_algo.setLowerBound(0.0)
ks_algo.setUpperBound(2.0 * pi)
ks_pdf = ks_algo.build(sample)

g = ks_pdf.drawPDF()
draw = exact_PDF.draw(0.0, 2.0 * pi, 1001).getDrawable(0)
draw.setLineWidth(2)
g.add(draw)
g.setLegends(['Ratio of Uniforms', 'exact pdf'])
g.setLegendPosition('topright')
g.setXTitle('x')
view = otv.View(g)

# %%
# By default, the parameter :math:`r=1`. We can get the associated acceptation ratio:
print('Acceptance ratio = ', ratioAlgo.getAcceptanceRatio())

# %%
# We can change the :math:`r` parameter and check the associated acceptance ratio:
r_new = 0.5
ratioAlgo.setR(r_new)
print('New acceptance ratio = ', ratioAlgo.getAcceptanceRatio())

# %%
# Case 3(a): Use of Independent Metropolis-Hastings
# -------------------------------------------------
# Let us use a mixture distribution to approximate the target distribution.
#
# This approximation will serve as the instrumental distribution
# in the independent Metropolis-Hastings algorithm.

exp = ot.Exponential(1.0)
unif = ot.Normal(5.3, 0.4)
instrumentalDistribution = ot.Mixture([exp, unif], [0.9, 0.1])

# %%
# Compare the instrumental density to the target density.
graph = f.draw(lower_bound, upper_bound, 100)
graph.setTitle("Instrumental PDF")
graph.setXTitle("")
graph.setYTitle("")
graph.add(instrumentalDistribution.drawPDF(lower_bound, upper_bound, 512))
graph.setLegendPosition("upper right")
graph.setLegends(["Unnormalized target density", "Instrumental PDF"])
_ = otv.View(graph)

# %%
# :class:`~openturns.MetropolisHastings` and derived classes can work directly with the logarithm of the unscaled
# target density.

log_density = ot.ComposedFunction(ot.SymbolicFunction("x", "log(x)"), f)

initialState = ot.Point([3.0])  # not important in this case
support = ot.Interval([lower_bound], [upper_bound])
independentMH = ot.IndependentMetropolisHastings(
    log_density, support, initialState, instrumentalDistribution, [0]
)

# %%
# Get a sample

sampleSize = 500
sample = independentMH.getSample(sampleSize)


# %%
# Plot the PDF of the sample to compare it to the target density

kernel = ot.KernelSmoothing()
posterior = kernel.build(sample)
graph = ot.Graph(
    "Independent Metropolis-Hastings sample: {} points".format(sampleSize),
    "",
    "",
    True,
    "upper right",
)
graph.setBoundingBox(ot.Interval([lower_bound, 0.0], [upper_bound, f([0.0])[0]]))
graph.add(f.draw(lower_bound, upper_bound, 512))
graph.add(posterior.drawPDF(lower_bound, upper_bound, 512))
graph.setLegends(["Unscaled target density", "Sample PDF"])
view = otv.View(graph)

# %%
# Even with very few sampling points (500),
# independent Metropolis-Hastings
# (with a judiciously chosen instrumental distribution)
# manages to capture the main features of the target density.

# %%
# Case 3(b): Use of Random walk Metropolis-Hastings
# -------------------------------------------------
# Let us use a normal instrumental distribution.

instrumentalDistribution = ot.Normal(0.0, 2.5)
randomwalkMH = ot.RandomWalkMetropolisHastings(
    log_density, support, initialState, instrumentalDistribution, [0]
)

# %%
# Get a sample
sample = randomwalkMH.getSample(sampleSize)

# %%
# Plot the PDF of the sample to compare it to the target density

kernel = ot.KernelSmoothing()
posterior = kernel.build(sample)
graph = ot.Graph(
    "Random walk Metropolis-Hastings sample: {} points".format(sampleSize),
    "",
    "",
    True,
    "upper right",
)
graph.setBoundingBox(ot.Interval([lower_bound, 0.0], [upper_bound, f([0.0])[0]]))
graph.add(f.draw(lower_bound, upper_bound, 512))
graph.add(posterior.drawPDF(lower_bound, upper_bound, 512))
graph.setLegends(["Unscaled target density", "Sample PDF"])
view = otv.View(graph)


# %%
view.ShowAll()

# %%
# References
# -----------
# [1] Marin , J.M. & Robert, C.P. (2007). *Bayesian Core: A Practical Approach to Computational Bayesian Statistics*. Springer-Verlag, New York
