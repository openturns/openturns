"""
Create a Point Conditional Distribution
=======================================
"""

# %%
# In this example, we create distributions conditionned by a particular value of their marginals.
#
# We consider the following cases:
#
# - Case 1:  Bivariate distribution with continuous marginals,
# - Case 2:  Bivariate distribution with continuous and discrete marginals,
# - Case 3:  Trivariate distribution with continuous and discrete marginals,
# - Case 4:  Extreme values conditioning.
#
# We illustrate the fact that the range of the conditioned distribution is updated.

# %%
import openturns as ot
import openturns.viewer as otv
import openturns.experimental as otexp

ot.ResourceMap.SetAsString("Contour-DefaultColorMapNorm", "rank")

# %%
# Case 1: Bivariate distribution with continuous marginals
# --------------------------------------------------------
#
# Here, we consider the bivariate distribution of :math:`(X_0, X_1)` defined by:
#
# ===========      ==================================================  ===============================
# Variable         Distribution                                        Parameter
# ===========      ==================================================  ===============================
# :math:`X_0`      :class:`~openturns.Gamma` (:math:`k, \lambda`)      :math:`(k, \lambda) = (5, 0.5)`
# :math:`X_1`      :class:`~openturns.Student` (:math:`\nu`)           :math:`\nu = 5`
# Copula           :class:`~openturns.ClaytonCopula` (:math:`\theta`)  :math:`\theta = 2.5`
# ===========      ==================================================  ===============================
#
# We condition the marginal :math:`X_1` to be equal to its quantiles of order
# 0.05, 0.5 and then 0.95. Then, we draw the resulting conditioned distribution of:
#
# .. math::
#
#    X_0 | X_1 = x_1
#
# We define initial distribution of :math:`(X_0, X_1)`.
coll_marg = [ot.Gamma(5.0, 0.5), ot.Student(5.0)]
cop = ot.ClaytonCopula(2.5)
dist_X = ot.JointDistribution(coll_marg, cop)

# %%
# We draw the PDF of :math:`(X_0, X_1)`.
g_X = dist_X.drawPDF()
g_X.setTitle(r"$(X_0, X_1)$: iso-lines PDF")
g_X.setXTitle(r"$x_0$")
g_X.setYTitle(r"$x_1$")

# %%
# We define the conditioned marginal, the list of the conditioning values and we create
# all the conditioned distributions, whose PDF we draw on the same graph.
#
# We also print the updated range of the distribution of :math:`X_0`.
cond_indices = [1]
q_list = [0.05, 0.5, 0.95]
cond_value_list = [coll_marg[1].computeQuantile(q)[0] for q in q_list]

g_cond = coll_marg[0].drawPDF()
g_cond.setLegends([r"dist of $X_0$"])
for index, cond_value in enumerate(cond_value_list):
    cond_value = cond_value_list[index]
    cond_dist = otexp.PointConditionalDistribution(dist_X, cond_indices, [cond_value])
    draw = cond_dist.drawPDF().getDrawable(0)
    draw.setLegend(r"$x_1 = q($" + str(q_list[index]) + ")")
    g_cond.add(draw)
    print(
        "Quantile of the cond value = ", q_list[index], "Range : ", cond_dist.getRange()
    )

g_cond.setTitle(r"PDF of $X_0|X_1 = x_1$")
g_cond.setColors(ot.Drawable.BuildDefaultPalette(len(q_list) + 1))

# %%
# We gather both graphs into one grid.
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, g_X)
grid.setGraph(0, 1, g_cond)
# sphinx_gallery_thumbnail_number =  1
view = otv.View(grid)

# %%
# Case 2: Bivariate distribution with continuous and discrete marginals
# ---------------------------------------------------------------------
#
# Here, we consider the bivariate distribution of :math:`(X_0, X_1)` defined by:
#
# ===========      ==================================================  ===============================
# Variable         Distribution                                        Parameter
# ===========      ==================================================  ===============================
# :math:`X_0`      :class:`~openturns.Binomial` (:math:`n,p`)          :math:`(n,p) = (25, 0.2)`
# :math:`X_1`      :class:`~openturns.Poisson` (:math:`\lambda`)       :math:`\lambda = 1`
# Copula           :class:`~openturns.GumbelCopula` (:math:`\theta`)   :math:`\theta = 2`
# ===========      ==================================================  ===============================
#
# We condition the marginal :math:`X_1` to be equal to its quantiles of order
# 0.05, 0.5 and then 0.95. Then, we draw the resulting conditioned distribution of:
#
# .. math::
#
#    X_0 | X_1 = x_1
#
# We go through the same steps as before.
coll_marg = [ot.Binomial(25, 0.2), ot.Poisson(1.0)]
cop = ot.GumbelCopula(2)
dist_X = ot.JointDistribution(coll_marg, cop)

# %%
# We draw the PDF of :math:`(X_0, X_1)`.
g_X = dist_X.drawPDF()
g_X.setTitle(r"$(X_0, X_1)$: iso-lines PDF")
g_X.setXTitle(r"$x_0$")
g_X.setYTitle(r"$x_1$")

# %%
# We define the conditioned marginal, the list of the conditioning values and we create
# all the conditioned distributions, whose PDF we draw on the same graph. To make them all visible,
# we artificially shifted the discrete distributions by a factor 0.1, 0.2 and 0.3.
#
# We also print the updated range of the distribution of :math:`X_0`.
cond_value_list = [coll_marg[1].computeQuantile(q)[0] for q in q_list]

g_cond = coll_marg[0].drawPDF()
g_cond.setLegends([r"dist of $X_0$"])
for index, cond_value in enumerate(cond_value_list):
    cond_value = cond_value_list[index]
    cond_dist = otexp.PointConditionalDistribution(dist_X, cond_indices, [cond_value])
    # Here, we shift the distribution to make it visible!
    draw = ((index / 10.0) + cond_dist).drawPDF().getDrawable(0)
    draw.setLegend(r"$x_1 = q($" + str(q_list[index]) + ")")
    g_cond.add(draw)
    print(
        "Quantile of the cond value = ", q_list[index], "Range : ", cond_dist.getRange()
    )

g_cond.setTitle(r"PDF of $X_0|X_1 = x_1$")
g_cond.setLegendPosition("topleft")
g_cond.setColors(ot.Drawable.BuildDefaultPalette(len(q_list) + 1))

# %%
# We gather both graphs into one grid.
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, g_X)
grid.setGraph(0, 1, g_cond)
view = otv.View(grid)


# %%
# Case 3: Trivariate distribution with continuous and discrete marginals
# ----------------------------------------------------------------------
#
# Here, we consider the trivariate distribution of :math:`(X_0, X_1, X_2)` defined by:
#
# ===========      ==================================================  ===============================
# Variable         Distribution                                        Parameter
# ===========      ==================================================  ===============================
# :math:`X_0`      :class:`~openturns.Normal` (:math:`\mu, \sigma`)    :math:`(\mu, \sigma) = (0,1)`
# :math:`X_1`      :class:`~openturns.Poisson` (:math:`\lambda`)       :math:`\lambda = 1`
# :math:`X_2`      :class:`~openturns.Uniform` (:math:`a,b`)           :math:`(a,b) = (-1, 1)`
# Copula           :class:`~openturns.NormalCopula` (:math:`\mat{R}`)  see below
# ===========      ==================================================  ===============================
#
# where the correlation matrix :math:`\mat{R}` of the normal copula is defined by:
#
# .. math::
#
#   R = \left( \begin{array}{ccc}
#       1 & 0.2 & 0.7 \\
#       0.2 & 1 & -0.5 \\
#       0.7 & -0.5 & 1
#       \end{array} \right)
#
# We condition the marginal :math:`X_1` to be equal to its quantiles of order
# 0.05, 0.5 and then 0.95. Then, we draw the resulting bivariate conditioned distribution of:
#
# .. math::
#
#    (X_0,X_2) | X_1 = x_1
#
# We go through the same steps as before.
coll_marg = [ot.Normal(), ot.Poisson(1.0), ot.Uniform()]
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.2
R[0, 2] = 0.7
R[1, 2] = -0.5
cop = ot.NormalCopula(R)
dist_X = ot.JointDistribution(coll_marg, cop)

# %%
# We draw the distribution of :math:`(X_0, X_2)`.
g_X02 = dist_X.getMarginal([0, 2]).drawPDF([-4.0, -1.0], [4.0, 1.0], [256] * 2)
contour = g_X02.getDrawable(0).getImplementation()
contour.setIsFilled(True)
contour.buildDefaultLevels(50)
g_X02.setDrawable(0, contour)
g_X02.setTitle(r"$(X_0, X_2)$: iso-lines PDF")

# %%
# We draw all the conditioned distributions :math:`(X_0, X_2)|X_1 = x_1`.
cond_value_list = [coll_marg[1].computeQuantile(q)[0] for q in q_list]
graph_list = list()
graph_list.append(g_X02)

for index, cond_value in enumerate(cond_value_list):
    cond_value = cond_value_list[index]
    cond_dist = otexp.PointConditionalDistribution(dist_X, cond_indices, [cond_value])
    g_cond = cond_dist.drawPDF([-4.0, -1.0], [4.0, 1.0], [256] * 2)
    contour = g_cond.getDrawable(0).getImplementation()
    contour.setIsFilled(True)
    contour.buildDefaultLevels(50)
    g_cond.setDrawable(0, contour)
    g_cond.setTitle(r"$(X_0, X_2)|X_1 = q($" + str(q_list[index]) + "): iso-lines PDF")
    g_cond.setXTitle(r"$x_0$")
    g_cond.setYTitle(r"$x_2$")
    graph_list.append(g_cond)
    print("Quantile of the cond value = ", q_list[index])
    print("Range :\n", cond_dist.getRange())

# %%
# We gather both graphs into one grid.
grid = ot.GridLayout(2, 2)
grid.setGraph(0, 0, graph_list[0])
grid.setGraph(0, 1, graph_list[1])
grid.setGraph(1, 0, graph_list[2])
grid.setGraph(1, 1, graph_list[3])
view = otv.View(grid)

# %%
# Case 4: Extreme values conditioning
# -----------------------------------
#
# Here, we show that it is possible to condition one marginal to a value which is outside its numerical range.
# Be careful that the conditioning value still must be in the theoretical range of the distribution!
#
# We consider the bivariate  distribution of :math:`(X_0, X_1)` defined by:
#
# ===========      ==================================================  ===============================
# Variable         Distribution                                        Parameter
# ===========      ==================================================  ===============================
# :math:`X_0`      :class:`~openturns.Normal` (:math:`\mu, \sigma`)    :math:`(\mu, \sigma) = (0,1)`
# :math:`X_1`      :class:`~openturns.Normal` (:math:`\mu, \sigma`)    :math:`(\mu, \sigma) = (0,1)`
# Copula           :class:`~openturns.ClaytonCopula` (:math:`\theta`)  :math:`\theta = 2`
# ===========      ==================================================  ===============================
#
# We condition the marginal :math:`X_1` to be equal to the value :math:`x_1 = -9`, which
# is outside the numerical range of its distribution: :math:`[-7.65063, +7.65063]`,
# but inside the theoretical range which is :math:`\Rset`.
#
# We draw the distribution of:
#
# .. math::
#
#    X_0 | X_1 = -9.0
#
dist_X = ot.JointDistribution([ot.Normal(), ot.Normal()], ot.ClaytonCopula(2.0))
cond_dist = otexp.PointConditionalDistribution(dist_X, [1], [-9.0])

g_cond = cond_dist.drawPDF(-10.0, -8.0, 256)
g_cond.setTitle(r"$X_0|X_1 = -9$: iso-lines PDF")
g_cond.setXTitle(r"$x_0$")
g_cond.setLegendPosition("")
view = otv.View(g_cond)

# %%
# We print the updated numerical range of :math:`X_0| X_1 = -9.0`. We note that it has moved into
# :math:`[-11.76, -0.82]` which is not included in the numerical range of :math:`X_0`.
print("Range of X0|X1 = -9.0 :\n", cond_dist.getRange())


# %%
view.ShowAll()
