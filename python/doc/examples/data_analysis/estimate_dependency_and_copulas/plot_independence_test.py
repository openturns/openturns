"""
Sample independence test
========================
"""
# %%
# In this example we are going to perform tests to assess whether two 1-d samples are independent or not.
#
# The following tests are available:
#
# - the ChiSquared test: it tests if both scalar samples (discrete ones only) are independent.
#   If :math:`n_{ij}` is the number of values of the sample :math:`i=(1,2)` in the modality :math:`1 \leq j \leq m`, :math:`\displaystyle n_{i.} = \sum_{j=1}^m n_{ij}` :math:`\displaystyle n_{.j} = \sum_{i=1}^2 n_{ij}`, and the ChiSquared test evaluates the decision variable:
#
# .. math::
#    D^2 = \sum_{i=1}^2 \sum_{j=1}^m \frac{( n_{ij} - \frac{n_{i.} n_{.j}}{n} )^2}{\frac{n_{i.} n_{.j}}{n}}
#
# which tends towards the :math:`\chi^2(m-1)` distribution. The hypothesis of independence is rejected if :math:`D^2` is too high (depending on the p-value threshold).
#
# - the Pearson test: it tests if there exists a linear relation between two scalar samples which form a gaussian vector (which is equivalent to have a linear correlation coefficient not equal to zero).
#   If both samples are :math:`\underline{x} = (x_i)_{1 \leq i \leq n}` and :math:`\underline{y} = (y_i)_{1 \leq i \leq n}`, and :math:`\bar{x} = \displaystyle \frac{1}{n}\sum_{i=1}^n x_i` and :math:`\bar{y} = \displaystyle \frac{1}{n}\sum_{i=1}^n y_i`, the Pearson test evaluates the decision variable:
#
#  .. math::
#      D = \frac{\sum_{i=1}^n (x_i - \bar{x})(y_i - \bar{y})}{\sqrt{\sum_{i=1}^n (x_i - \bar{x})^2\sum_{i=1}^n (y_i - \bar{y})^2}}
#
# The variable :math:`D` tends towards a :math:`\chi^2(n-2)`, under the hypothesis of normality of both samples. The hypothesis of a linear coefficient equal to 0 is rejected (which is equivalent to the independence of the samples) if D is too high (depending on the p-value threshold).
#
# - the Spearman test: it tests if there exists a monotonous relation between two scalar samples.
#   If both samples are :math:`\underline{x} = (x_i)_{1 \leq i \leq n}` and :math:`\underline{y}= (y_i)_{1 \leq i \leq n}`,, the Spearman test evaluates the decision variable:
#
# .. math::
#      D = 1-\frac{6\sum_{i=1}^n (r_i - s_i)^2}{n(n^2-1)}
#
# where :math:`r_i = rank(x_i)` and  :math:`s_i = rank(y_i)`. :math:`D` is such that :math:`\sqrt{n-1}D` tends towards the gaussian (0,1) distribution.
#

# %%
from __future__ import print_function
import openturns as ot
ot.Log.Show(ot.Log.NONE)

# %%
# **continuous samples**

# %%
# Create continuous samples
sample1 = ot.Normal().getSample(100)
sample2 = ot.Normal().getSample(100)

# %%
# Using the Pearson test
ot.HypothesisTest.Pearson(sample1, sample2, 0.10)

# %%
# Using the Spearman test
ot.HypothesisTest.Spearman(sample1, sample2, 0.10)

# %%
# **discrete samples**

# %%
# Create discrete samples
sample1 = ot.Poisson(0.2).getSample(100)
sample2 = ot.Poisson(0.2).getSample(100)

# %%
# Using the Chi2 test
ot.HypothesisTest.ChiSquared(sample1, sample2, 0.10)
