"""
Model a singular multivariate distribution
==========================================
"""

# %%
# The objective of the example is to show how to fit a singular copula
# on a sample of dimension :math:`n_\inputDim` which might be high. For example,
# this is the case when we want to fit the joint distribution of the :math:`n_\inputDim` Karhunen Loeve
# coefficients in the Karhunen Loeve modelisation of a process.
#
# A way to do that is to use the  empirical Bernstein copula implemented in
# :class:`~openturns.EmpiricalBernsteinCopula` through its factory
# class:`~openturns.BernsteinCopulaFactory`.
# This copula is parameterized by the number :math:`m` of cells. This set of cells
# partitions the sample: all the points of the same cell are grouped into a single *global* point.
#
# The empirical Bernstein copula is a mixture of products of Beta laws centered on the global point of each cell.
# We denote by :math:`\sampleSize` the sample size. The number of global points varies according to the number of
# cells considered (math:`m`):
#
# - :math:`m = N` means that all the sample has been retained: we create one cell around
#   each point of the sample,
# - :math:`m = 1` means the sample has been grouped into one single global point: we get the independent copula,
# - :math:`1 < m < N` means that we create :math:`m` cells to group :math:`N` points.
#
# When  :math:`m = \sampleSize`, the empirical Bernstein copula is the *Beta copula* in the sens of [segers2016]_.
#
# For the  empirical Bernstein copula defined in this way to be a copula,  :math:`m`  must divide :math:`\sampleSize`.
# Thus, if this is not the case, part of the sample is set aside in order to check this condition (see
# :class:`~openturns.EmpiricalBernsteinCopula` for more details).
#
# A point's *zone of influence* is its cell. The larger :math:`m`, the smaller its zone of influence:
# the final copula thus adheres strongly to the sample. The smaller :math:`m`, the larger
# its zone of influence, and the more the final copula will be able to fill a large
# area around it.
#
# So, if we know that the final copula is diffuse, we recommend to let the
# :class:`~openturns.BernsteinCopulaFactory` automatically calculate the best number of cells to take. If
# we know that the final copula is singular, we recommend to specify the value of :math:`m` to be equal
# to the sample size :math:`\sampleSize`.
#
# We illustrate the influence of  :math:`m` on a singular copula.

import openturns as ot
import openturns.viewer as viewer
import math as m

# %%
# Routine to draw a distribution cloud and a sample.


def draw(dist, Y):
    g = ot.Graph()
    g.setAxes(True)
    g.setGrid(True)
    c = ot.Cloud(dist.getSample(10000))
    c.setColor("red")
    c.setPointStyle("bullet")
    g.add(c)
    c = ot.Cloud(Y)
    c.setColor("black")
    c.setPointStyle("bullet")
    g.add(c)
    g.setBoundingBox(
        ot.Interval(
            Y.getMin() - 0.5 * Y.computeRange(), Y.getMax() + 0.5 * Y.computeRange()
        )
    )
    return g


# %%
# We consider the function :math:`f: \Rset^3 \rightarrow \Rset` defined by:
#
# .. math::
#
#      f(u, v_1, v_2) = (y_1, y_2)
#
# where:
#
# .. math::
#
#      y_1 & = \sin(u) / (1 + \cos(u)^2) + 0.05 * v_1 \\
#      y_2 & = \sin(u) \cos(u) / (1 + \cos(u)^2) + 0.05 * v_2
#
# We define the following input random vector:
#
# .. math::
#
#      U  \sim \cU(-0.85\pi, 0.85\pi) \\
#      (V_1, V_2)  \sim \cN(\vect{\mu} = \vect{0}, \vect{\sigma} = \vect{1}, Id_2)\\
#
# with :math:`U` and :math:`\vect{V})` independent.
#
# We define the output random vector :math:`\vect{Y}` as:
#
# .. math::
#
#      \vect{Y} = f(U, V_1, V_2)

f = ot.SymbolicFunction(
    ["U", "v1", "v2"],
    ["sin(U)/(1+cos(U)^2)+0.05*v1", "sin(U)*cos(U)/(1+cos(U)^2)+0.05*v2"],
)
U = ot.Uniform(-0.85 * m.pi, 0.85 * m.pi)
V = ot.Normal(2)
X = ot.BlockIndependentDistribution([U, V])

# %%
# We generate a sample of the output random vector :math:`\vect{Y}` of size :math:`N`.
N = 200
sample_Y = f(X.getSample(N))

# %%
# We estimate the distribution of the output random vector :math:`\vect{Y}` by multivariate kernel smoothing.
y_multi_ks = ot.KernelSmoothing().build(sample_Y)
view = viewer.View(draw(y_multi_ks, sample_Y))

# %%
# Now, we estimate the distribution of :math:`\vect{Y}` splitting the estimation of the marginals
# from the estimation of the copula:
#
# - the marginals are fitted by kernel smoothing,
# - the copula is fitted using the Bernstein copula factory :class:`~openturns.BernsteinCopulaFactory` that builds
#   an empirical Bernstein copula.
#
# First, we do not specify the bin number :math:`m`. It is equal to the value computed by the default method, which is the
# LogLikelihood criteria.
empBern_copula = ot.BernsteinCopulaFactory().buildAsEmpiricalBernsteinCopula(sample_Y)
print("bin number computed m = ", empBern_copula.getBinNumber())
marginals = [
    ot.KernelSmoothing().build(sample_Y.getMarginal(j))
    for j in range(sample_Y.getDimension())
]
y_empBern = ot.JointDistribution(marginals, empBern_copula)
view = viewer.View(draw(y_empBern, sample_Y))

# %%
# We see that the optimal number of cells is :math:`m = 1`: it means that one single cell is created.
# The built copula is diffuse
# in :math:`[0,1]^2` and the estimated copula is the independent copula.


# %%
# Now, we specify a bin number equal to the sample size: :math:`m = N` so that the built copula is very close to the sample.
# In that case, it manages to reproduce its specific feature.
empBern_copula = ot.BernsteinCopulaFactory().build(sample_Y, N)
y_empBern = ot.JointDistribution(marginals, empBern_copula)
view = viewer.View(draw(y_empBern, sample_Y))
viewer.View.ShowAll()
