"""
Create the distribution of the maximum of distributions
=======================================================
"""

# %%
# Context
# -------
#
# In this example, we define how to build a distribution defined as the maximum of some other ones.
#
# We detail some interesting cases:
#
# - Case 1: The maximum of correlated scalar distributions,
# - Case 2: The maximum of independent scalar distributions,
# - Case 3: The maximum of independent and identically distributed scalar distributions.

# %%
import openturns as ot
import openturns.viewer as otv

ot.Log.Show(ot.Log.NONE)

# %%
# Case 1: The maximum correlated scalar distributions
# ---------------------------------------------------
#
# We consider :math:`d=3` scalar distributions :math:`X_i` that form a random vector
# :math:`(X_1, X_2, X_3)` which copula is denoted by :math:`C`.
#
# We assume that:
#
# .. math::
#    X_1 \sim \cU(2.5, 3.5)\\
#    X_2 \sim \mbox{Log}\cU(1.0, 1.2)\\
#    X_3 \sim \cT(2.0, 3.0, 4.0)\\
#    C \sim \mbox{NormalCopula}(\mat{M})
#
# where :math:`\mat{M}` is a correlation matix.
#
# We want to define the distribution of:
#
# .. math::
#    Y_1 = \max(X_1, X_2, X_3).

# %%
# We create the distribution of :math:`(X_1, X_2, X_3)`:
dist_X1 = ot.Uniform(2.5, 3.5)
dist_X2 = ot.LogUniform(1.0, 1.2)
dist_X3 = ot.Triangular(2.0, 3.0, 4.0)
coll = [dist_X1, dist_X2, dist_X3]
cor_mat = ot.CorrelationMatrix([[1.0, -0.9, 0.0], [-0.9, 1.0, 0.3], [0.0, 0.3, 1.0]])
cop = ot.NormalCopula(cor_mat)
dist_X = ot.JointDistribution(coll, cop)

# %%
# We create the distribution of :math:`Y_1`:
dist_Y_1 = ot.MaximumDistribution(dist_X)
print(dist_Y_1)

# %%
# We draw its PDF:
graph = dist_Y_1.drawPDF()
graph.setTitle(r" Distribution of $Y_1 = max(X_1, X_2, X_3)$")
graph.setXTitle(r"$Y_1$")
graph.setLegendPosition("")
view = otv.View(graph)

# %%
# Case 2: The maximum of independent scalar distributions
# -------------------------------------------------------
#
# Here, we define :math:`(\tilde{X}_1,\tilde{X}_2, \tilde{X}_3)` the random vector where the
# :math:`\tilde{X}_i` are distributed as :math:`X_i` and are independent.
#
# We want to define the distribution of:
#
# .. math::
#    Y_2 = \max(\tilde{X}_1,\tilde{X}_2, \tilde{X}_3).

# %%
# We create the distribution of :math:`Y_2`:
dist_Y_2 = ot.MaximumDistribution(coll)

# %%
# We could have written:
dist_X_tilde = ot.JointDistribution(coll)
dist_Y_2 = ot.MaximumDistribution(dist_X_tilde)

# %%
# We draw its PDF:
graph = dist_Y_2.drawPDF()
graph.setTitle(r" Distribution of $Y_2 = max(\tilde{X}_1,\tilde{X}_2, \tilde{X}_3)$")
graph.setXTitle(r"$Y_2$")
graph.setLegendPosition("")
view = otv.View(graph)

# %%
# Case 3: The maximum of a independent and identically distributed scalar distributions
# -------------------------------------------------------------------------------------
#
# Here we suppose that :math:`X_4` and :math:`X_5` are independent and identically distributed
# as :math:`X_1`.
#
# We want to define the distribution of:
#
# .. math::
#    Y_3 = \max(X_1, X_4, X_5).

# %%
# We create the distribution of :math:`Y_3`:
dist_Y_3 = ot.MaximumDistribution(dist_X1, 3)

# %%
# We draw its PDF:
graph = dist_Y_3.drawPDF()
graph.setTitle(r" Distribution of $Y_3 = max(X_1, X_4, X_5)$")
graph.setXTitle(r"$Y_3$")
graph.setLegendPosition("")
view = otv.View(graph)

# %%
otv.View.ShowAll()
