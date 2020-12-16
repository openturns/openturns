"""
Kolmogorov-Smirnov : understand the statistics
==============================================
"""
# %%

# %%
# In this example, we illustrate how the Kolmogorov-Smirnov statistics is computed.
#
# * We generate a sample from a gaussian distribution.
# * We create a Uniform distribution which parameters are estimated from the sample.
# * The Kolmogorov-Smirnov statistics is computed and plot on the empirical cumulated distribution function.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# The computeKSStatisticsIndex function computes the Kolmogorov-Smirnov distance between the sample and the distribution. Furthermore, it returns the index which achieves the maximum distance in the sorted sample. The following function is for teaching purposes only: use `FittingTest` for real applications.

# %%
def computeKSStatisticsIndex(sample, distribution):
    sample = ot.Sample(sample.sort())
    print("Sorted")
    print(sample)
    n = sample.getSize()
    D = 0.0
    index = -1
    D_previous = 0.0
    for i in range(n):
        F = distribution.computeCDF(sample[i])
        S1 = abs(F - float(i) / n)
        S2 = abs(float(i + 1) / n - F)
        print(
            "i=%d, x[i]=%.4f, F(x[i])=%.4f, S1=%.4f, S2=%.4f"
            % (i, sample[i, 0], F, S1, S2)
        )
        D = max(S1, S2, D)
        if D > D_previous:
            print("D max!")
            index = i
            D_previous = D
    observation = sample[index]
    return D, index, observation


# %%
# The drawKSDistance function plots the empirical distribution function of the sample and the Kolmogorov-Smirnov distance at point x. The empirical CDF is a staircase function and is discontinuous at each observation. The computeEmpiricalCDF() method computes the probability P(X <= x), but this only takes into account for half the extreme values of the CDF. The other half is P(X < x) which is approximated by P(X <= x - delta) where delta is close to zero.

# %%
def drawKSDistance(
    sample, distribution, observation, D, distFactory, delta_x=ot.Point([1.0e-6])
):
    graph = ot.Graph("KS Distance = %.4f" % (D), "X", "CDF", True, "topleft")
    # Vertical line at point x
    ECDF_index = sample.computeEmpiricalCDF(observation)
    ECDF_index_shifted = sample.computeEmpiricalCDF(observation - delta_x)
    CDF_index = distribution.computeCDF(observation)
    curve = ot.Curve(
        [observation[0], observation[0], observation[0]],
        [ECDF_index, ECDF_index_shifted, CDF_index],
    )
    curve.setColor("green")
    curve.setLegend("KS Statistics")
    curve.setLineWidth(4.0 * curve.getLineWidth())
    graph.add(curve)
    # Empirical CDF
    empiricalCDF = ot.UserDefined(sample).drawCDF()
    empiricalCDF.setColors(["blue"])
    empiricalCDF.setLegends(["Empirical DF"])
    graph.add(empiricalCDF)
    #
    distname = distFactory.getClassName()
    distribution = distFactory.build(sample)
    cdf = distribution.drawCDF()
    cdf.setLegends([distname])
    graph.add(cdf)
    return graph


# %%
# We generate a sample from a standard gaussian distribution.

# %%
N = ot.Normal()
n = 10
sample = N.getSample(n)

# %%
# Compute the index which achieves the maximum Kolmogorov-Smirnov distance.

# %%
# We then create a Uniform distribution which parameters are estimated from the sample. This way, the K.S. distance is large enough to being graphically significant.

# %%
distFactory = ot.UniformFactory()
distribution = distFactory.build(sample)
distribution

# %%
# Compute the index which achieves the maximum Kolmogorov-Smirnov distance.

# %%
D, index, observation = computeKSStatisticsIndex(sample, distribution)
print("D=", D, ", Index=", index, ", Obs.=", observation)

# %%
graph = drawKSDistance(sample, distribution, observation, D, distFactory)
view = viewer.View(graph)
plt.show()

# %%
# We see that the K.S. statistics is acheived where the distance between the empirical distribution function of the sample and the candidate distribution is largest.
