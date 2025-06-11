"""
Create a maximum entropy order statistics distribution
======================================================
"""

# %%
# Context
# -------
#
# In this example we illustrate how to build the joint distribution of the
# random vector :math:`\inputRV = (X_1, \dots, X_n)` which marginals are given
# and such that:
#
# .. math::
#    :label: orderConstraintEx
#
#    X_1 \leq \dots \leq X_n \quad \mbox{almost surely}.
#
# The class :class:`~openturns.MaximumEntropyOrderStatisticsDistribution` implements the joint
# distribution of :math:`\inputRV` that maximizes the entropy of  :math:`\inputRV`.
# See [fischer2017]_ for more details.
#
# The distribution of :math:`\inputRV` is the distribution of the :math:`n` order statistics of a
# distribution :math:`Y`. We can write:
#
# .. math::
#   (X_1, \dots, X_n) \sim (Y_{(1)}, \dots, Y_{(n)})
#
# In that case, we do not know  :math:`Y` but we fix the marginals of its order
# statistics distribution. If the :math:`X_i \sim \beta(i, n-i+1)`, then :math:`\inputRV` is the
# joint distribution of the :math:`n` order statistics of :math:`Y \sim \cU([0,1])`.
#
# The marginals of  :math:`\inputRV` must be compatible with the order constraint :eq:`orderConstraint`.
# The library checks this compatibility by default. It requires that if :math:`F_i`
# is the CDF of :math:`X_i`, then they must verify:
#
# .. math::
#    :label: orderConstraintCDFEx
#
#    F_1 \geq F_2 \geq \dots \geq F_n
#
# which implies that if :math:`[a_i, b_i]` is the numerical range of :math:`X_i`, then:
#
# .. math::
#    a_i \leq a_{i+1}\\
#    b_i \leq b_{i+1}

# %%
import openturns as ot
import openturns.viewer as otv

ot.ResourceMap.SetAsString("Contour-DefaultColorMapNorm", "rank")


# %%
# Create the distributions :math:`X_i`:
coll = [ot.Beta(1.5, 1.7, 0.0, 1.0), ot.Beta(2.0, 2.3, 0.5, 1.2)]

# %%
# Create the joint order statistics distribution with maximum entropy: the creation is a
# success, which means that the marginals are compatible with the order constraint
# :eq:`orderConstraintEx` and fulfill :eq:`orderConstraintCDFEx`.
jointDist = ot.MaximumEntropyOrderStatisticsDistribution(coll)

# %%
# Draw a sample: the components are well ordered.
x_sample = jointDist.getSample(10)
print(x_sample)

# %%
# Draw PDF
graph = jointDist.drawPDF()
contour = graph.getDrawable(0).getImplementation()
contour.setIsFilled(True)
contour.buildDefaultLevels(50)
graph.setDrawable(0, contour)
graph.setTitle("PDF of the Maximum Entropy Order Statistics Distribution")
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
