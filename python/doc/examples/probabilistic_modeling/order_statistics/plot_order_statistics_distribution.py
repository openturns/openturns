"""
Compute the joint distribution of order statistics
==================================================
"""

# %%
# Context
# -------
#
# In this example, we define the joint distribution of the :math:`n` order statistics
# of a given distribution :math:`X`.
#
# We detail the following cases:
#
# - Case 1: The Min - Max order statistics,
# - Case 2: Two order statistics that bound the :math:`\alpha` -quantile of :math:`X` with the confidence
#   level :math:`1-\beta`.
#
# Let :math:`X` be a random variable and :math:`\vect{X}_{(1:n)} = (X_{(1)},\dots,X_{(n)})`
# be the random vector of all its order statistics  of size :math:`n`.
# Let :math:`f` be its PDF and  :math:`F` its CDF. Then the random vector
# of its  :math:`n` order statistics  is distributed as the random vector
# :math:`(F^{-1}(U_{(1)}), \dots, F^{-1}(U_{(n)}))` where the :math:`U_{(i)}`
# are the order statistics of the Uniform distribution over :math:`[0,1]`:
#
# .. math::
#     (X_{(1)},\dots,X_{(n)}) \sim (F^{-1}(U_{(1))}), \dots, F^{-1}(U_{(n)}))
#
# Then the CDF of :math:`\vect{X}_{(1:n)}` is defined by:
#
# .. math::
#     F_{\vect{X}_{(1:n)}}(\vect{x}) = F_{\vect{U}_{(1:n)}}(F(x_1), \dots, F(x_n))
#
# and its PDF (if defined) by:
#
# .. math::
#    f_{\vect{X}_{(1:n)}}(\vect{x}) = n!\prod_{i=1}^n f(x_i) \,\mathbf{1}_{\cS}(\vect{x})
#    \quad \vect{x} \in \Rset^n
#
# Thus, to get the joint distribution of :math:`\vect{X}_{(1:n)}`, we use the
# :class:`~openturns.JointDistribution` whose all marginals are  :math:`X`
# and whose core is a :class:`~openturns.experimental.UniformOrderStatistics`.

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
import math as m

ot.ResourceMap.SetAsString("Contour-DefaultColorMapNorm", "rank")
# sphinx_gallery_thumbnail_number = 2

# %%
# We create the joint distribution of the order statistics from the distribution of :math:`X`
# and the size of the order statistics:
n = 100
dist = ot.Normal()
orderStat_dist = ot.JointDistribution([dist] * n, otexp.UniformOrderStatistics(n))


# %%
# Case 1: The Min - Max order statistics
# --------------------------------------
#
# We extract the first and last marginals of the order statistics distribution:
min_max_dist = orderStat_dist.getMarginal([0, n - 1])


# %%
# We search the mode of the joint distribution. To get it, we search the point that maximizes the
# log-PDF of the distribution. We use a :class:`~openturns.OptimizationProblem`.
optimPb = ot.OptimizationProblem(min_max_dist.getLogPDF())
optimPb.setMinimization(False)
optimPb.setBounds(min_max_dist.getRange())

algo = ot.Cobyla(optimPb)
algo.setStartingPoint(min_max_dist.getMean())
algo.run()

mode = algo.getResult().getOptimalPoint()
print("Mode of the (Min-Max) joint distribution : ", mode)

# %%
# We draw its PDF.
g = min_max_dist.drawPDF()
contour = g.getDrawable(0).getImplementation()
contour.buildDefaultLevels(50)
contour.setIsFilled(True)
g.setDrawable(0, contour)

mode_cloud = ot.Cloud([mode])
mode_cloud.setPointStyle("bullet")
mode_cloud.setColor("red")
mode_cloud.setLegend("mode")
g.add(mode_cloud)

g.setTitle(r"Joint PDF of $(X_{(1)}, X_{(n)})$")
g.setXTitle(r"$X_{(1)}$")
g.setYTitle(r"$X_{(n)}$")
view = otv.View(g)

# %%
# Case 2: Two order statistics that bound the :math:`\alpha`-quantile with the confidence level :math:`1-\beta`.
# ---------------------------------------------------------------------------------------------------------------
#
# We want to bound the  :math:`\alpha`-quantile :math:`q_\alpha` of :math:`X` with two particular order statisctics among the :math:`n` ones.
# We choose these order statisctics  such that they bound  :math:`q_\alpha` with a confidence level equal to
# :math:`1-\beta`.
# For example, we want to bound the quantile of order :math:`90\%` with a confidence  :math:`95\%`.
#
# It is possible to use some particular order statistics to bound :math:`q_\alpha`: [delmas2006]_
# (see proposition 11.1.3) shows that if :math:`i_n` and :math:`j_n` are two indices defined by:
#
# .. math::
#     i_n  = [n \alpha - \sqrt{n} a_{\beta} \sqrt{\alpha(1-\alpha)}]\\
#     j_n  = [n \alpha + \sqrt{n} a_{\beta} \sqrt{\alpha(1-\alpha)}]
#
# where :math:`a_{\beta}` is the quantile of order :math:`(1-\beta/2)` of the normal
# distribution with zero mean and unit variance, then we have:
#
# .. math::
#     \lim_{n \rightarrow +\infty} \Prob{q_\alpha \in [X_{(i_n)}, X_{(j_n)}]} = 1 - \beta
#
# It means that any realization of the joint distribution of :math:`(X_{(i_n)}, X_{(j_n)})`, for :math`n` large enough,
# forms an interval that contains
# :math:`q_\alpha` with a confidence equal to :math:`1-\beta`.
alpha = 0.90
beta = 0.05
a_beta = ot.Normal().computeQuantile(1 - beta / 2)[0]
delta = m.sqrt(n * alpha * (1 - alpha)) * a_beta
i_n = int(n * alpha - delta)
j_n = int(n * alpha + delta)
print("Chosen order statistics (in, jn) = ", i_n, j_n)

# %%
# Be careful that the indices of the order statistics begins at 0.
ic_beta_dist = orderStat_dist.getMarginal([i_n - 1, j_n - 1])

# %%
# We search the mode of the joint distribution. To get it, we search the point that maximizes the
# log-PDF of the distribution. We use a :class:`~openturns.OptimizationProblem`.
optimPb = ot.OptimizationProblem(ic_beta_dist.getLogPDF())
optimPb.setMinimization(False)
optimPb.setBounds(ic_beta_dist.getRange())

algo = ot.Cobyla(optimPb)
algo.setStartingPoint([1.1, 2.1])
algo.run()

mode = algo.getResult().getOptimalPoint()
print("Mode of the joint distribution : ", mode)

# %% We draw the iso-lines of the PDF and the mode.
g = ic_beta_dist.drawPDF()
contour = g.getDrawable(0).getImplementation()
contour.buildDefaultLevels(50)
contour.setIsFilled(True)
g.setDrawable(0, contour)

mode_cloud = ot.Cloud([mode])
mode_cloud.setPointStyle("bullet")
mode_cloud.setColor("red")
mode_cloud.setLegend("mode")
g.add(mode_cloud)

g.setTitle(r"Joint PDF of $(X_{(i_n)}, X_{(j_n)})$")
g.setXTitle(r"$X_{(i_n)}$")
g.setYTitle(r"$X_{(j_n)}$")
view = otv.View(g)

# %%
# Display all figures
otv.View.ShowAll()
