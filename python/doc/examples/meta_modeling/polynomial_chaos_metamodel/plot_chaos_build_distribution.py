"""
Fit a distribution from an input sample
=======================================
"""
# %%
#
# In this example we show how to use the
# :meth:`~openturns.MetaModelAlgorithm.BuildDistribution` function to fit a distribution to an
# input sample.
# This function is used by the :class:`~openturns.FunctionalChaosAlgorithm`
# class when we want to create a polynomial chaos metamodel and we have a
# design of experiments which have been computed beforehand.
# In this case, we have to identify the distributions which best fit to the
# input sample in order to define the input probabilistic model.
# This is, in turn, used by in the polynomial chaos to create the orthogonal basis.
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# We first create the function `model`.

# %%
ot.RandomGenerator.SetSeed(0)
dimension = 2
input_names = ["x1", "x2"]
formulas = ["cos(x1 + x2)", "(x2 + 1) * exp(x1)"]
model = ot.SymbolicFunction(input_names, formulas)

# %%
# Then we create a sample `x` and compute the corresponding output sample `y`.

# %%
distribution = ot.Normal(dimension)
samplesize = 1000
inputSample = distribution.getSample(samplesize)
outputSample = model(inputSample)

# %%
# Create a functional chaos model.
# The algorithm used by :meth:`~openturns.MetaModelAlgorithm.BuildDistribution`
# fits a distribution on the input sample.
# This is done with the Lilliefors test.
# Please read :ref:`kolmogorov_smirnov_test` for more details on this topic.

# %%
# The Lilliefors test is based on sampling the distribution of the
# Kolmogorov-Smirnov statistics.
# The sample size corresponding to this algorithm is configured
# by the `"FittingTest-LillieforsMaximumSamplingSize"` :class:`~openturns.ResourceMap`
# key.
# In order to get satisfactory results, the default value of this
# key is relatively large.

# %%
ot.ResourceMap.GetAsUnsignedInteger("FittingTest-LillieforsMaximumSamplingSize")

# %%
# In order to speed this example up, let us reduce this value.

# %%
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-LillieforsMaximumSamplingSize", 100)

# %%
# Then we fit the distribution.
distribution = ot.FunctionalChaosAlgorithm.BuildDistribution(inputSample)

# %%
# Let us explore the distribution with its fitted parameters.

# %%
distribution

# %%
# We can also analyse its properties in more details.

# %%
for i in range(dimension):
    marginal = distribution.getMarginal(i)
    marginalname = marginal.getImplementation().getClassName()
    print("Marginal #", i, ":", marginalname)
distribution.getCopula()

# %%
# The previous call to :meth:`~openturns.MetaModelAlgorithm.BuildDistribution` is what is done internally by the
# following constructor of `FunctionalChaosAlgorithm`.

# %%
algo = ot.FunctionalChaosAlgorithm(inputSample, outputSample)

# %%
# The previous constructor is the main topic of the example
# :doc:`Create a polynomial chaos metamodel from a data set </auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos>`.

# %%
ot.ResourceMap.Reload()  # reset default settings
