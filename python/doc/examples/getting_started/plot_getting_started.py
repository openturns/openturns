"""
Getting started
===============
"""

# %%
# The goal of this example is to highlight the main features of the library.
# It assumes a basic knowledge of the few concepts of the uncertainty methodology (inference, probabilistic modelling, simulation, sensitivity).

# %%
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv


# %%
# Inference of the input distribution
# -----------------------------------

# %%
# Load a sample of the input variables from the cantilever beam use-case
cb = cantilever_beam.CantileverBeam()
data = cb.data
print(data[:5])

# %%
# Infer marginals from most common 1-d parametric distributions
marginal_factories = [ot.NormalFactory(), ot.BetaFactory(), ot.UniformFactory(), ot.LogNormalFactory(),
                      ot.TriangularFactory(), ot.WeibullMinFactory(), ot.WeibullMaxFactory()]
estimated_marginals = []
for index in range(data.getDimension()):
    best_model, _ = ot.FittingTest.BestModelBIC(data[:, index], marginal_factories)
    print(best_model)
    estimated_marginals.append(best_model)
print(estimated_marginals)

# %%
# Assess the goodness of fit of the second marginal
graph = ot.VisualTest.DrawQQplot(data[:, 1], estimated_marginals[1])
_ = otv.View(graph)

# %%
# Infer the copula from common n-d parametric copulas in the ranks space
# If the copula is known it can be provided directly through :class:`~openturns.NormalCopula` for example
copula_factories = [ot.IndependentCopulaFactory(), ot.NormalCopulaFactory(), ot.StudentCopulaFactory()]
copula_sample = ot.Sample(data.getSize(), data.getDimension())
for index in range(data.getDimension()):
    copula_sample[:, index] = estimated_marginals[index].computeCDF(data[:, index])
estimated_copula, _ = ot.FittingTest.BestModelBIC(copula_sample, copula_factories)
print(estimated_copula)

# %%
# Assemble the joint distribution from marginals and copula
estimated_distribution = ot.JointDistribution(estimated_marginals, estimated_copula)
print(estimated_distribution)

# %%
# Uncertainty propagation
# -----------------------

# %%
# Creation of the model


def fpython(X):
    E, F, L, II = X
    Y = F * L**3 / (3 * E * II)
    return [Y]


model = ot.PythonFunction(4, 1, fpython)


# %%
# The distribution can also be given directly when known
distribution = cb.independentDistribution

# %%
# Propagate the input distribution through the model
# Here the function evaluation can benefit parallelization depending on its type, see also n_cpus option from :class:`~openturns.PythonFunction`.
sample_x = distribution.getSample(1000)
sample_y = model(sample_x)

# %%
# Estimate a non-parametric distribution (see :class:`~openturns.KernelSmoothing`) of the output variable
ks = ot.KernelSmoothing().build(sample_y)
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, ks.drawPDF())
grid.setGraph(0, 1, ks.drawCDF())
_ = otv.View(grid)


# %%
# Build a metamodel with polynomial chaos expansion
# -------------------------------------------------
algo = ot.LeastSquaresExpansion(sample_x, sample_y, distribution)
algo.run()
metamodel_result = algo.getResult()
metamodel = metamodel_result.getMetaModel()
test_x = distribution.getSample(1000)
test_y = model(test_x)
predictions = metamodel(test_x)
validation = ot.MetaModelValidation(test_y, predictions)
graph = validation.drawValidation()
graph.setTitle(graph.getTitle() + f" R2={validation.computeR2Score()}")
_ = otv.View(graph)

# %%
# Sensitivity analysis
# ----------------------

# %%
# For simplicity we can use a method that does not impose special requirements on the design of experiments
sobol_x = distribution.getSample(5000)
sobol_y = metamodel(sobol_x)
algo = otexp.RankSobolSensitivityAlgorithm(sobol_x, sobol_y)
print(algo.getFirstOrderIndices())

# %%
# Plot the sensitivity indices
# The most influential variable is `F`.
graph = algo.draw()
_ = otv.View(graph)

# %%
otv.View.ShowAll()
