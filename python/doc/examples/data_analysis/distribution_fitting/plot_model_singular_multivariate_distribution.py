"""
Model a singular multivariate distribution
==========================================
"""

# %%
# From time to time we need to model singular :math:`n_D` distributions
# (e.g. the joint distribution of Karhunen Loeve coefficients for curves resulting from the transport of a low dimensional random vector).
# A way to do that is to use an :class:`~openturns.EmpiricalBernsteinCopula` with a bin number equal to the sample size
# (also called the empirical beta copula in this case).

import openturns as ot
import openturns.viewer as viewer
import math as m

ot.RandomGenerator.SetSeed(0)

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
# LogLikelihood criteria. We get :math:`m=1`, which
# means that one cell is created: the built copula is diffuse in :math:`[0,1]^2`. The estimated copula is
# the independent copula.
empBern_copula = ot.BernsteinCopulaFactory().buildAsEmpiricalBernsteinCopula(sample_Y)
print('bin number computed m = ', empBern_copula.getBinNumber())
marginals = [
    ot.KernelSmoothing().build(sample_Y.getMarginal(j)) for j in range(sample_Y.getDimension())
]
y_empBern = ot.JointDistribution(marginals, empBern_copula)
view = viewer.View(draw(y_empBern, sample_Y))

# %%
# Now, we specify a bin number equal to the sample size: :math:`m = N` so that the built copula is very close to the sample.
# With this parametrization, the empirical Bernstein copula is the *Beta copula* in the sens of [segers2016]_.
# In that case, it manages to reproduce its specific feature.
empBern_copula = ot.BernsteinCopulaFactory().build(sample_Y, N)
y_empBern = ot.JointDistribution(marginals, empBern_copula)
view = viewer.View(draw(y_empBern, sample_Y))
viewer.View.ShowAll()
