"""
Polynomial chaos over database
==============================
"""
# %%
# In this example we are going to create a global approximation of a model response using functional chaos over a design of experiment.
#
# You will need to specify the distribution of the input parameters.
# If not known, statistical inference can be used to select a possible candidate, and fitting tests can validate such an hypothesis.

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Create a function R^n --> R^p
# For example R^4 --> R
myModel = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["1+x1*x2 + 2*x3^2+x4^4"])

# Create a distribution of dimension n
# for example n=3 with indpendent components
distribution = ot.ComposedDistribution(
    [ot.Normal(), ot.Uniform(), ot.Gamma(2.75, 1.0), ot.Beta(2.5, 1.0, -1.0, 2.0)]
)

# %%
# Prepare the input/output samples
sampleSize = 250
X = distribution.getSample(sampleSize)
Y = myModel(X)
dimension = X.getDimension()

# %%
# build the orthogonal basis
coll = [
    ot.StandardDistributionPolynomialFactory(distribution.getMarginal(i))
    for i in range(dimension)
]
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(coll, enumerateFunction)

# %%
# create the algorithm
degree = 6
adaptiveStrategy = ot.FixedStrategy(
    productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)
)
projectionStrategy = ot.LeastSquaresStrategy()
algo = ot.FunctionalChaosAlgorithm(
    X, Y, distribution, adaptiveStrategy, projectionStrategy
)
algo.run()

# %%
# get the metamodel function
result = algo.getResult()
metamodel = result.getMetaModel()

# %%
# Print residuals
result.getResiduals()
