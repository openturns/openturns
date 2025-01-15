"""
Estimate a confidence interval of a quantile
============================================
"""

# %%
# In this example, we introduce two methods to estimate a confidence interval of the
# :math:`\alpha` level quantile (:math:`\alpha \in [0,1]`) of the distribution of
# a scalar random
# variable :math:`X`. Both methods use the order statistics to estimate:
#
# - an asymptotic confidence interval with confidence level :math:`\beta \in [0,1]`,
# - an exact upper bounded confidence interval with confidence level :math:`\beta \in [0,1]`.
#
# In this example, we consider the quantile of level :math:`\alpha = 95\%`,
# with a confidence level of :math:`\beta = 90\%`.

# %%
import openturns as ot
import openturns.experimental as otexp
import math as m

ot.Log.Show(ot.Log.NONE)

# %%
# We consider a random vector which is the output of a model and an input distribution.
model = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2"])
R = ot.CorrelationMatrix(2)
R[0, 1] = -0.6
inputDist = ot.Normal([0.0, 0.0], R)
inputDist.setDescription(["X1", "X2"])
inputVector = ot.RandomVector(inputDist)

# %%
# Create the output random vector
output = ot.CompositeRandomVector(model, inputVector)

# %%
# We define the level :math:`\alpha` of the quantile and the confidence level :math:`\beta`.
alpha = 0.95
beta = 0.90

# %%
# We generate a sample of the variable.
n = 10**4
sample = output.getSample(n)

# %%
# We get the empirical estimator of the :math:`\alpha` level quantile which is the
# :math:`\lfloor \sampleSize \alpha \rfloor` -th order statistics evaluated on
# the sample.
empiricalQuantile = sample.computeQuantile(alpha)
print(empiricalQuantile)

# %%
# The asymptotic confidence interval of level :math:`\beta` is :math:`\left[ X_{(i_n)}, X_{(j_n)}\right]`
# such that:
#
# .. math::
#
#     i_\sampleSize & = \left\lfloor \sampleSize \alpha - \sqrt{\sampleSize} \; z_{\frac{1+\beta}{2}} \; \sqrt{\alpha(1 - \alpha)} \right\rfloor\\
#     j_\sampleSize & = \left\lfloor \sampleSize \alpha + \sqrt{\sampleSize} \; z_{\frac{1+\beta}{2}} \;  \sqrt{\alpha(1 - \alpha)} \right\rfloor
#
# where  :math:`z_{\frac{1+\beta}{2}}` is the :math:`\frac{1+\beta}{2}` level quantile of the standard normal distribution (see [delmas2006]_ proposition 11.1.13).
#
# Then we have:
#
# .. math::
#
#     \lim\limits_{\sampleSize \rightarrow +\infty} \Prob{x_{\alpha} \in \left[ X_{(i_\sampleSize,\sampleSize)}, X_{(j_\sampleSize,\sampleSize)}\right]} = \beta
#
a_beta = ot.Normal(1).computeQuantile((1.0 + beta) / 2.0)[0]
i_n = int(n * alpha - a_beta * m.sqrt(n * alpha * (1.0 - alpha)))
j_n = int(n * alpha + a_beta * m.sqrt(n * alpha * (1.0 - alpha)))
print(i_n, j_n)

# %%
# Get the sorted sample
sortedSample = sample.sort()

# %%
# Get the asymptotic confidence interval :math:`\left[ X_{(i_n)}, X_{(j_n)}\right]`
# Care: the index in the sorted sample is :math:`i_n-1` and :math:`j_n-1`
infQuantile = sortedSample[i_n - 1]
supQuantile = sortedSample[j_n - 1]
print(infQuantile, empiricalQuantile, supQuantile)

# %%
# This can be done using :class:`~openturns.experimental.QuantileConfidence`
# when the ranks :math:`i_n` and :math:`j_n` are directly given in [0, n-1]
algo = otexp.QuantileConfidence(alpha, beta)
i_n, j_n = algo.computeAsymptoticBilateralRank(n)
ci = algo.computeAsymptoticBilateralConfidenceInterval(sample)
print(f"asymptotic ranks={[i_n, j_n]} CI={ci}")

# %%
# The empirical quantile was estimated with the :math:`\lfloor \sampleSize\alpha \rfloor` -th order statistics evaluated on
# the sample of size :math:`\sampleSize`.
# We define :math:`i = \sampleSize-\lfloor \sampleSize\alpha \rfloor` and we evaluate the minimum sample size :math:`\tilde{\sampleSize}` that
# ensures that the :math:`(\tilde{\sampleSize}-i)` order statistics is greater than :math:`x_{\alpha}` with the confidence :math:`\beta`.
i = n - int(n * alpha)
minSampleSize = algo.computeUnilateralMinimumSampleSize(i, True)
print(minSampleSize)

# %%
# Now we can evaluate the upper bounded confidence interval:
# we generate a sample with the previous minimum size and extract the empirical quantile of order :math:`(\tilde{\sampleSize}-i)`.
sample = output.getSample(minSampleSize)
upperBoundQuantile = sample.sort()[-i - 1]
print(upperBoundQuantile)

# %%
# We can also find this rank back from the size
k = algo.computeUnilateralRank(minSampleSize, True)
print(k, minSampleSize - i - 1)

# %%
# The quantile bound is given in the same manner from the sample and the rank
ci = algo.computeUnilateralConfidenceInterval(sample, True)
print(ci)
