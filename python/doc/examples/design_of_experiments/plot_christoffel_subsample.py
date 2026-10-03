"""
Christoffel subsampling on the square
=====================================
"""

# sphinx_gallery_thumbnail_number = 2

# %%
# Optimal sampling draws the design points from the Christoffel distribution
# associated to the approximation space instead of the reference measure.
# On the square with a tensor Legendre basis everything is explicit, so this
# example reproduces the textbook picture: the optimal density concentrates
# at the exiting corners, and the subsampled weighted least squares needs far
# fewer evaluations than uniform sampling. The uniform design is then recycled
# as a given pool for a second subsampling, and the same case is repeated with
# a Fourier basis to show the workflow does not depend on the polynomial
# family. See Cohen-Dolbeault (Fig. 1, Sec. 6).

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

# %%
# Reference measure, tensor Legendre basis and total-degree space, degree 3
measure = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 2)
basis = ot.OrthogonalBasis(ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)] * 2))
spaceDimension = 10
christoffel = otexp.ChristoffelDistribution(basis, spaceDimension)
target = ot.SymbolicFunction(['x0', 'x1'], ['exp(-(x0^2 + x1^2))'])

# %%
# Optimal density heatmap: k_m / m concentrates at the corners
nX = nY = 80
mesh = ot.Box([nX - 2, nY - 2], ot.Interval([-1.0] * 2, [1.0] * 2)).generate()
density = christoffel.computeChristoffel(mesh)
density /= spaceDimension
abscissae = ot.Sample([[-1.0 + 2.0 * i / (nX - 1)] for i in range(nX)])
contour = ot.Contour(abscissae, abscissae, density)
graph = ot.Graph("Christoffel density", "x0", "x1")
graph.add(contour)
view = otv.View(graph)

# %%
# Subsampled design against a uniform design with the same budget
budget = 231
experiment = otexp.ChristoffelSubsampleExperiment(basis, budget)
sample, weights = experiment.generateWithWeights()
uniformSample = measure.getSample(budget)


def fit(design, designWeights, expansionBasis=basis, expansionMeasure=measure, expansionSize=spaceDimension):
    observations = target(design)
    weights = ot.Point(designWeights)
    expansion = ot.LeastSquaresExpansion(design, weights, observations, expansionMeasure, expansionBasis, expansionSize)
    expansion.run()
    return expansion.getResult().getMetaModel()


metamodel = fit(sample, weights)
uniformMetamodel = fit(uniformSample, ot.Point(budget, 1.0))

# %%
# Validation error and Gramian conditioning on both designs
validation = measure.getSample(2000)
reference = target(validation)
norm = reference.computeRawMoment(2)[0] ** 0.5
error = ((metamodel(validation) - reference).computeRawMoment(2)[0]) ** 0.5 / norm
uniformError = ((uniformMetamodel(validation) - reference).computeRawMoment(2)[0]) ** 0.5 / norm
print(f"subsampled error={error:.3e} uniform error={uniformError:.3e}")


def condition(design, designWeights, designExperiment=experiment):
    eigenvalues = sorted(designExperiment.computeDesignEigenvalues(design, ot.Point(designWeights)))
    return eigenvalues[-1] / eigenvalues[0]


print(f"subsampled conditioning={condition(sample, weights):.3f} uniform conditioning={condition(uniformSample, [1.0] * budget):.3f}")

# %%
# Subsample an a priori given sample: recycle the uniform design as pool
givenSample, givenWeights = experiment.generateWithWeights(uniformSample)
givenMetamodel = fit(givenSample, list(givenWeights))
givenError = ((givenMetamodel(validation) - reference).computeRawMoment(2)[0]) ** 0.5 / norm
print(f"given-pool error={givenError:.3e} conditioning={condition(givenSample, list(givenWeights)):.3f}")

# %%
# The same test case with a tensor Fourier basis: its family is orthonormal
# on [-pi, pi], so the experiment draws its design on that wider square and
# the expansion needs more modes to reach the same accuracy
fourierBasis = ot.OrthogonalBasis(ot.OrthogonalProductFunctionFactory([ot.FourierSeriesFactory()] * 2))
fourierMeasure = fourierBasis.getMeasure()
fourierExperiment = otexp.ChristoffelSubsampleExperiment(fourierBasis, budget)
fourierSample, fourierWeights = fourierExperiment.generateWithWeights()
fourierMetamodel = fit(fourierSample, fourierWeights, fourierBasis, fourierMeasure, 50)
fourierValidation = fourierMeasure.getSample(2000)
fourierReference = target(fourierValidation)
fourierNorm = fourierReference.computeRawMoment(2)[0] ** 0.5
fourierError = ((fourierMetamodel(fourierValidation) - fourierReference).computeRawMoment(2)[0]) ** 0.5 / fourierNorm
print(f"fourier error={fourierError:.3e} conditioning={condition(fourierSample, fourierWeights, fourierExperiment):.3f}")

# %%
# The thinned designs cluster where the density is large
cloud = ot.Cloud(sample, "blue", "fsquare", "subsample")
uniformCloud = ot.Cloud(uniformSample, "red", "fcircle", "uniform")
givenCloud = ot.Cloud(givenSample, "green", "plus", "given-pool")
designGraph = ot.Graph("Designs", "x0", "x1")
designGraph.add(cloud)
designGraph.add(uniformCloud)
designGraph.add(givenCloud)
designGraph.setLegends(["subsample", "uniform", "given-pool"])
view = otv.View(designGraph)

# %%
# Display all figures
otv.View.ShowAll()
