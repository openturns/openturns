"""
Visualize sensitivity
=====================
"""
# %%
# The Cobweb graph enables to visualize all the combinations of the input
# variables which lead to a specific range of the output variable. It is a very simple and cheap tool to visualize sensitivity from the raw data.
#
# Let us consider a model :math:`f: \mathbb{R}^n \longrightarrow \mathbb{R}`, where :math:`f(\underline{X}) = Y`.
#
# The graph requires to have an input sample :math:`X_s` and an output sample :math:`Y_s`.
#
# The first figure draws such a graph: each column represents one component
# :math:`X_i` of the input vector :math:`\underline{X}`.
# The last column represents the scalar output variable :math:`Y`.
# For each point :math:`\underline{X}^j`, each component :math:`X_i^j`
# is noted on its respective axe and the last mark is the one which corresponds to
# the associated :math:`Y^j`. A line joins all the marks. Thus, each point of the sample
# corresponds to a particular line on the graph.
#
# The scale of the axes are quantile based: each axe runs between 0 and 1 and each
# value is represented by its quantile with respect to its marginal empirical distribution.
#
# It is interesting to select, among those lines, the ones which correspond to a specific range of the output variable.
# These particular lines are colored differently.
# This specific range is defined in the quantile based scale of :math:`Y` or in its specific scale.
# In that second case, the range is automatically converted into a quantile based scale range.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We load the :ref:`Ishigami model<use-case-ishigami>` from the `usecases` module :
from openturns.usecases.ishigami_function import IshigamiModel
im = IshigamiModel()
# the Ishigami function
model = im.model
# the input distribution
inputDist = im.distributionX

# %%
# We create a random vector from out input distribution :
inputVector = ot.RandomVector(inputDist)

# %%
# And we create the output random vector Y = model(X) :
output = ot.CompositeRandomVector(model, inputVector)

# %%
# We generate an input sample of size :math:`N` :
N = 1000
X = inputVector.getSample(N)

# %%
# We evaluate the associated output sample :
Y = model(X)
Y.setDescription("Y")

# %%
# We display the minimum, maximum and value of the 90% quantile of `Y` :
print(Y.getMin(), Y.getMax(), Y.computeQuantilePerComponent(0.9))

# %%
# Value based scale to describe the Y range
# -----------------------------------------
#
# Say we are interested in the higher values of the output :math:`Y`. A first
# approach is to highlight peculiar lines for which :math:`Y \in [a,b]` with
# the bounds :math:`a` and :math:`b` well chosen. For example, values greater
# than 85% of the maximum :
# 
#  - :math:`a = 0.85 \max(Y)` ;
#  - :math:`b = \max(Y)` ;
#
minValue = 0.85 * Y.getMax()[0]
maxValue = Y.getMax()[0]

# We deactivate the default quantile scale.
quantileScale = False
graphCobweb = ot.VisualTest.DrawCobWeb(X, Y, minValue, maxValue, 'red', quantileScale)
graphCobweb.setLegendPosition('bottomright')
view = viewer.View(graphCobweb)

# %%
# Here we would like to conclude that the highest values of `Y` are obtained from a specific input as the highlighted lines clearly follow one only path.
# However, this approach is too naive and specific to the input sample. Indeed,
# if we set the lower bound to 80% of the maximum :

# %%
minValue = 0.80 * Y.getMax()[0]
maxValue = Y.getMax()[0]
quantileScale = False
graphCobweb = ot.VisualTest.DrawCobWeb(X, Y, minValue, maxValue, 'red', quantileScale)
graphCobweb.setLegendPosition('bottomright')
view = viewer.View(graphCobweb)

# %%
# A new path is then available ! That is the reason why we chose a quantile based ranking as the value based cobweb involves a bit of guessing.


# %%
# Rank based scale to describe the Y range
# ---------------------------------------------------
#
# In this paragraph we use quantile based bounds. We are still interested in
# the highest values of `Y` more specifically the 95% quantile : 

# %%
minValue = 0.95
maxValue = 1.0
# a quantileScale is used, default behaviour
quantileScale = True
graphCobweb = ot.VisualTest.DrawCobWeb(X, Y, minValue, maxValue, 'red', quantileScale)
graphCobweb.setLegendPosition('bottomright')
view = viewer.View(graphCobweb)

# %%
# The cobweb obtained is helpful : we see peculiar values for each marginal.
#
#


# %%
# Recall that the Ishigami model is given by
#
# .. math::
#    f(X) = sin(X_1) + 7sin^2(X_2) + 0.1 X_3^4 sin(X_1)
#
# with each marginal of :math:`X=(X_1, X_2, X_3)` uniform in :math:`]-\pi,\pi[`.

# Then the highest values of :math:`Y=f(X)` are obtained when each term is near its peak :
#
# - the :math:`sin(X_1)` term around :math:`X_1 = \pi / 2` ;
# - the :math:`7sin^2(X_2)` term around :math:`X_2 = -\pi / 2` and :math:`X_2 = \pi / 2` ;
# - the :math:`X_3^4 sin(X_1)` term around :math:`X_1 = \pi / 2` and :math:`X_3 = \{ -\pi, \pi}`.
#
# These values can be seen on the cobweb as for each marginal there is a cluster around respectively 1, 2 and 2 values for :math`X_1`, :math`X_2` and :math`X_3`.
# This amounts to 4 different values 'realizing' the maximum and we can observe 4 distinct paths on the cobweb as well.
#

# %%
# We can also guess the independence of marginals when looking at paths between
# :math:`X_2` and :math:`X_3`. For any given cluster value of :math:`X_2` on the
# graph there are as many paths to a high value of :math:`X_3` as to a small value.
# A dependence between these two marginals would have presented unbalanced paths.

# %%
# When the cobweb brings nothing
# ------------------------------
#
# To conclude our tour on the cobweb graph we look at the 50% quantile : that is
# values around the mean :

# %%
minValue = 0.48
maxValue = 0.52
quantileScale = True
graphCobweb = ot.VisualTest.DrawCobWeb(X, Y, minValue, maxValue, 'red', quantileScale)
graphCobweb.setLegendPosition('topright')
view = viewer.View(graphCobweb)

# %%
# We cannot extract any useful information from this cobweb. In fact it is the expected behaviour as mean values should be attained from various combinations of# the input variables.
# The cobweb graph is a cheap tool and highly useful to explore more extreme values !

# %%
# Display figures
plt.show()
