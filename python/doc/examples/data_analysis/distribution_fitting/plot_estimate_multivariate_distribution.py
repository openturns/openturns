"""
Estimate a multivariate distribution
====================================
"""
# %%
# In this example we are going to estimate a joint distribution from a multivariate sample by fitting marginals and finding a set of copulas.
#
# While the estimation of marginals is quite straightforward, the estimation of the dependency structure takes several steps:
#
# - find the dependent components
# - estimate a copula on each dependent bloc
# - assemble the estimated copulas
#

# %%
from __future__ import print_function
import openturns as ot
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# generate some multivariate data to estimate, with correlation
cop1 = ot.AliMikhailHaqCopula(0.6)
cop2 = ot.ClaytonCopula(2.5)
copula = ot.ComposedCopula([cop1, cop2])
marginals = [ot.Uniform(5.0, 6.0), ot.Arcsine(), ot.Normal(-40.0, 3.0), ot.Triangular(100.0, 150.0, 300.0)]
distribution = ot.ComposedDistribution(marginals, copula)
sample = distribution.getSample(10000).getMarginal([0, 2, 3, 1])

# %%
# estimate marginals
dimension = sample.getDimension()
marginalFactories = []
for factory in ot.DistributionFactory.GetContinuousUniVariateFactories():
    if str(factory).startswith('Histogram'):
        # ~ non-parametric
        continue
    marginalFactories.append(factory)
estimated_marginals = [ot.FittingTest.BestModelBIC(sample.getMarginal(i), marginalFactories)[0] for i in range(dimension)]
estimated_marginals


# %%
# Find connected components of a graph defined from its adjacency matrix

# %%
def find_neighbours(head, covariance, to_visit, visited):
    N = covariance.getDimension()
    visited[head] = 1
    to_visit.remove(head)
    current_component = [head]
    for i in to_visit:
        # If i is connected to head and has not yet been visited
        if covariance[head, i] > 0:
            # Add i to the current component
            component = find_neighbours(i, covariance, to_visit, visited)
            current_component += component
    return current_component

def connected_components(covariance):
    N = covariance.getDimension()
    to_visit = list(range(N))
    visited = [0] * N
    all_components = []
    for head in range(N):
        if visited[head] == 0:
            component = find_neighbours(head, covariance, to_visit, visited)
            all_components.append(sorted(component))
    return all_components


# %%
# Estimate the copula
# -------------------
#
# First find the dependent components : we compute the `Spearman` correlation

# %%
C = sample.computeSpearmanCorrelation()
print(C)

# %%
# We filter and consider only significantly non-zero correlations.

# %%
epsilon = 1.0 / m.sqrt(sample.getSize())
for j in range(dimension):
    for i in range(j):
        C[i, j] = 1.0 if abs(C[i, j]) > epsilon else 0.0
print(C)

# %%
# Note that we can apply the `HypothesisTest.Spearman` test. As the null hypothesis of the test is the `independence`, we must take the complementary of the binary measure as follow:
#
# >>>   M = ot.SymmetricMatrix(dimension)
# >>>   for i in range(dimension):
# >>>       M[i,i] = 1
# >>>       for j in range(i):
# >>>           M[i, j] = 1 - ot.HypothesisTest.Spearman(sample[:,i], sample[:,j]).getBinaryQualityMeasure()
#

# %%
# Now we find the independent blocs:

# %%
blocs = connected_components(C)
blocs

# %%
# For each dependent block, we estimate the most accurate non parameteric copula.
#
# To do this, we first need to transform the sample in such a way as to keep the copula intact but make all marginal samples follow the uniform distribution on [0,1].

# %%
copula_sample = ot.Sample(sample.getSize(), sample.getDimension())
copula_sample.setDescription(sample.getDescription())
for index in range(sample.getDimension()):
    copula_sample[:, index] = estimated_marginals[index].computeCDF(sample[:, index])

# %%
copulaFactories = []
for factory in ot.DistributionFactory.GetContinuousMultiVariateFactories():
    if not factory.build().isCopula():
        continue
    if factory.getImplementation().getClassName()=='BernsteinCopulaFactory':
        continue
    copulaFactories.append(factory)
estimated_copulas = [ot.FittingTest.BestModelBIC(copula_sample.getMarginal(bloc), copulaFactories)[0] for bloc in blocs]
estimated_copulas

# %%
# Finally we assemble the copula

# %%
estimated_copula_perm = ot.ComposedCopula(estimated_copulas)

# %%
# Take care of the order of each bloc vs the order of original components !

# %%
permutation = []
for bloc in blocs:
    permutation.extend(bloc)
inverse_permutation = [-1] * dimension
for i in range(dimension):
    inverse_permutation[permutation[i]] = i
estimated_copula = estimated_copula_perm.getMarginal(inverse_permutation)
estimated_copula

# %%
# We build joint distribution from marginal distributions and dependency structure:

# %%
estimated_distribution = ot.ComposedDistribution(estimated_marginals, estimated_copula)
estimated_distribution
