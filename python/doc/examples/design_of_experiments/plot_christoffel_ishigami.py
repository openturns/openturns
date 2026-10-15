"""
Christoffel subsampling for the Ishigami function
=================================================
"""

# %%
# Optimal sampling draws the design points from the Christoffel distribution
# associated to the approximation space instead of the reference measure.
# Here the function is the Ishigami model and the reference measure is its
# natural distribution, the independent uniform law on :math:`[-\\pi, \\pi]^3`.
# The approximation space is spanned by tensor Legendre polynomials, orthonormal
# with respect to that measure, enumerated in linear (total-degree) order.
# At equal budget, is the Christoffel subsample more accurate than the natural
# sample? We answer by repeating both fits and comparing the distributions
# of their squared :math:`L^2` errors.
#
# Two practical choices shape the protocol. With an inexhaustible basis the
# experiment works with :math:`m = n` functions, and thinning then costs
# approximately :math:`O(n^5)`; a budget of 200 points costs about a minute
# per design. We use 60 points (under a second per design) instead, which
# already separates the two methods by two orders of magnitude. Likewise the
# squared :math:`L^2` error is estimated on a fixed validation sample drawn
# from the natural distribution, as in the square example: exact quadrature
# of each squared residual costs minutes and is out of reach for 100 fits.

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from openturns.usecases import ishigami_function

ot.RandomGenerator.SetSeed(0)

# %%
# We load the Ishigami model and build the tensor Legendre basis orthonormal
# with respect to its natural distribution. The experiment deduces the space
# dimension from the budget: with this inexhaustible basis it is the budget
# itself. The validation sample is drawn once so that every repetition is
# scored on the same points.
im = ishigami_function.IshigamiModel()
measure = im.distribution
model = im.model
enumerateFunction = ot.LinearEnumerateFunction(measure.getDimension())
basis = ot.OrthogonalBasis(
    ot.OrthogonalProductPolynomialFactory(
        [measure.getMarginal(i) for i in range(measure.getDimension())],
        enumerateFunction,
    )
)
budget = 60
experiment = otexp.ChristoffelSubsampleExperiment(basis, budget)
expansionSize = experiment.getSpaceDimension()
validation = measure.getSample(2000)
reference = model(validation)

# %%
# Each repetition fits the same truncated space twice: once on a Christoffel
# subsample with its density-ratio weights, once on a natural sample with
# uniform weights. Both fits use :class:`~openturns.LeastSquaresExpansion`
# on the first ``expansionSize`` basis functions. At :math:`m = n` the
# thinned design systematically misses the default frame band, so the loop
# runs with warnings silenced; the error distributions below show the design
# still wins by far.
repetitions = 100
christoffelErrors = ot.Sample(repetitions, 1)
naturalErrors = ot.Sample(repetitions, 1)
ot.Log.Show(ot.Log.ERROR)
for repetition in range(repetitions):
    sample, weights = experiment.generateWithWeights()
    observations = model(sample)
    expansion = ot.LeastSquaresExpansion(
        sample, ot.Point(weights), observations, measure, basis, expansionSize
    )
    expansion.run()
    metamodel = expansion.getResult().getMetaModel()
    christoffelErrors[repetition, 0] = (
        (metamodel(validation) - reference).computeRawMoment(2)[0]
    )
    naturalSample = measure.getSample(budget)
    naturalObservations = model(naturalSample)
    naturalExpansion = ot.LeastSquaresExpansion(
        naturalSample, naturalObservations, measure, basis, expansionSize
    )
    naturalExpansion.run()
    naturalMetamodel = naturalExpansion.getResult().getMetaModel()
    naturalErrors[repetition, 0] = (
        (naturalMetamodel(validation) - reference).computeRawMoment(2)[0]
    )
print(f"median Christoffel error={christoffelErrors.computeQuantile(0.5)[0]:.3e}")
print(f"median natural error={naturalErrors.computeQuantile(0.5)[0]:.3e}")
ot.Log.Show(ot.Log.DEFAULT)

# %%
# Squared errors live on :math:`[0, +\\infty)`, so each density is estimated
# with a :class:`~openturns.KernelSmoothing` truncated at the lower bound 0.
# The two densities are drawn on the same log-scale figure: the Christoffel
# errors concentrate two orders of magnitude below the natural ones.
christoffelSmoothing = ot.KernelSmoothing()
christoffelSmoothing.setBoundingOption(ot.KernelSmoothing.LOWER)
christoffelSmoothing.setLowerBound(0.0)
christoffelDensity = christoffelSmoothing.build(christoffelErrors)
naturalSmoothing = ot.KernelSmoothing()
naturalSmoothing.setBoundingOption(ot.KernelSmoothing.LOWER)
naturalSmoothing.setLowerBound(0.0)
naturalDensity = naturalSmoothing.build(naturalErrors)
graph = christoffelDensity.drawPDF()
graph.add(naturalDensity.drawPDF().getDrawable(0))
graph.setLegends(["Christoffel subsample", "natural sample"])
graph.setTitle("Squared L2 error distributions")
graph.setXTitle("squared L2 error")
graph.setLogScale(ot.GraphImplementation.LOGX)
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
