"""
Mixture of experts
==================
"""

# %%
# In this example we are going to approximate a piece wise continuous function using an expert mixture of metamodels.
#
# The metamodels will be represented by the family of functions :math:`f_k \forall \in [1, n_c]`:
#
# .. math::
#      f(\vect{x}) = f_k(\vect{x})
#
# for any :math:`\vect{z} \in \textrm{Class}_k` where the :math:`n_c \in \Nset` classes are defined by the classifier.
#
# Using the supervised mode the classifier partitions the input and output spaces at once:
#
# .. math::
#    \vect{z} = (\vect{x}, f(\vect{x}))
#
# The classifier is :class:`~openturns.MixtureClassifier` based on a :class:`~openturns.Mixture` distribution defined as:
#
# .. math::
#    p(\vect{x}) = \sum_{i=1}^{n_c} w_i p_i(\vect{x})
#
#
# The rule to assign a point to a class is defined as follows: :math:`\vect{x}` is assigned to the class :math:`j = \operatorname{argmax}_j \log w_k p_k(\vect{z})`.
#
# The grade of :math:`\vect{x}` with respect to the class :math:`k` is :math:`\log w_k p_k(\vect{x})`.
#
#

# %%
import openturns as ot
import openturns.viewer as otv
import numpy as np


# %%
dimension = 1

# Define the piecewise model we want to rebuild


def piecewise(X):
    # if x < 0.0:
    #     f = (x+0.75)**2-0.75**2
    # else:
    #     f = 2.0-x**2
    xarray = np.asarray(X)
    return np.piecewise(
        xarray,
        [xarray < 0, xarray >= 0],
        [lambda x: x * (x + 1.5), lambda x: 2.0 - x * x],
    )


f = ot.PythonFunction(1, 1, func_sample=piecewise)

# %%
# Build a metamodel over each segment
degree = 5
samplingSize = 100
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)
adaptiveStrategy = ot.FixedStrategy(
    productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)
)

# %%
# Interval 1: :math:`[-1.0; 0.0]`
d1 = ot.Uniform(-1.0, 0.0)
X1 = d1.getSample(samplingSize)
Y1 = f(X1)
fc1 = ot.FunctionalChaosAlgorithm(X1, Y1, d1, adaptiveStrategy)
fc1.run()
mm1 = fc1.getResult().getMetaModel()
graph = mm1.draw(-1.0, -1e-6)
view = otv.View(graph)

# %%
# Interval 2: :math:`[0.0, 1.0]`
d2 = ot.Uniform(0.0, 1.0)
X2 = d2.getSample(samplingSize)
Y2 = f(X2)
fc2 = ot.FunctionalChaosAlgorithm(X2, Y2, d2, adaptiveStrategy)
fc2.run()
mm2 = fc2.getResult().getMetaModel()
graph = mm2.draw(1e-6, 1.0)
view = otv.View(graph)

# %%
# Define the mixture
R = ot.CorrelationMatrix(2)
d1 = ot.Normal([-1.0, -1.0], [1.0] * 2, R)  # segment 1
d2 = ot.Normal([1.0, 1.0], [1.0] * 2, R)  # segment 2
weights = [1.0] * 2
atoms = [d1, d2]
mixture = ot.Mixture(atoms, weights)

# %%
# Create the classifier based on the mixture
classifier = ot.MixtureClassifier(mixture)

# %%
# Create local experts using the metamodels
experts = ot.Basis([mm1, mm2])

# %%
# Create a mixture of experts
evaluation = ot.ExpertMixture(experts, classifier)
moe = ot.Function(evaluation)

# %%
# Draw the mixture of experts
graph = moe.draw(-1.0, 1.0)
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
