"""
Apply a transform or inverse transform on your polynomial chaos
===============================================================
"""
# %%
#
# Introduction
# ------------
#
# In this document we present the transformation involved in the creation of a polynomial chaos. Indeed, the polynomial chaos expansion is never directly applied to the input random variable of a model :math:`g`. Instead, the expansion is expressed based on the associated standard random variable. In this example,  we show how to transform a Log-Normal random variable into its standardized variable with the class `DistributionTransformation`.

# %%
# Probabilistic transform
# -----------------------
#
# Let :math:`T` be the probabilistic transform which maps the physical random variable :math:`X` to the standardized random variable :math:`\xi`:
#
# .. math::
#    \xi = T(X).
#
#
# Let :math:`F_X` be the Cumulative Distribution Function (CDF) associated with the random variable :math:`X` and let :math:`F_\xi` be the CDF associated with the random variable :math:`\xi`. Therefore, :math:`F_\xi` is the CDF associated with the orthogonal polynomials involved in the polynomial chaos expansion. In this case, the transform is:
#
# .. math::
#    \xi = F_\xi^{-1} \left( F_X(X) \right),
#
#
# for any :math:`X \in \mathbb{R}`.

# %%
# Example
# -------
#
# We want to use the Hermite orthogonal polynomials to expand a Log-Normal random variable:
#
# * let :math:`X` follow the Log-Normal distribution with the following parameters: Lognormal(:math:`\mu=3 \times 10^4`, :math:`\sigma=9\times 10^3`),
# * let :math:`Z=\xi` follow the Normal distribution with zero mean and unit standard deviation (the letter :math:`Z` is often used for standard Normal random variables).
#
# Let :math:`F_{LN}` be the CDF of the Log-Normal distribution associated with :math:`X` and let :math:`\Phi` be the CDF of the standard Normal distribution.
#
# Therefore,
#
# .. math::
#    Z = T(X) = \Phi^{-1}(F_{LN}(X)),
#
#
# for any :math:`X \in \mathbb{R}`.

# %%
import openturns as ot

# %%
# In the first step, we define the LogNormal distribution. Its parameters - mean and standard deviation - have been selected so that there is no ambiguity with the standard Normal distribution. This parametrization can be used thanks to the `ParametrizedDistribution` class.

# %%
Xparam = ot.LogNormalMuSigma(3.0e4, 9.0e3, 15.0e3)  # in N
X = ot.ParametrizedDistribution(Xparam)

# %%
# Then we generate a 5-point sample.

# %%
sampleX = X.getSample(5)
sampleX

# %%
# In the second step, we define the random variable :math:`Z` with standard Normal distribution.

# %%
Z = ot.Normal()

# %%
# We also generate a sample from it.

# %%
sampleZ = Z.getSample(5)
sampleZ

# %%
# In the third step, we create the transform T which maps X to Z.

# %%
T = ot.DistributionTransformation(X, Z)

# %%
# We apply this transform on the sample generated from `X`.

# %%
T(sampleX)

# %%
# The inverse transform maps :math:`Z` into :math:`X`.

# %%
Tinverse = T.inverse()

# %%
Tinverse(sampleZ)

# %%
# Conclusion
# ----------
#
# The `DistributionTransformation` class is rarely used directly because the polynomial chaos classes perform the transformation automatically. However, understanding how this transform is done clarifies why the coefficients of the chaos expansion cannot be related to the input random variable :math:`X`: the expansion is based on the standard variables :math:`\xi`. Hence, the absolute values of the corresponding coefficients have no straightforward interpretation, even though squaring them yields the part of the global variance associated with each coefficient.
