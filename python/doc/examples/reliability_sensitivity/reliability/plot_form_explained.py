"""
An illustrated example of a FORM probability estimate
=====================================================
"""

# %%
# Abstract
# --------
#
# In this example we illustrate the different steps of a FORM/SORM analysis on a
# simple example. We focus on the different steps and compare them with an analytic
# computation whenever possible.
#
# See :ref:`FORM <form_approximation>` and :ref:`SORM <sorm_approximation>` and to get more theoretical details.
import openturns as ot
import openturns.viewer as otv
import numpy as np

# %%
# Context
# -------
#
# We consider a bivariate random vector :math:`\inputRV = (X_1, X_2)` with the following independent components that follow:
#
# - the exponential distribution with parameter :math:`\lambda=1`, :math:`X_1 \sim \mathcal{E}(1.0)` ;
# - the standard unit normal distribution :math:`X_2 \sim \mathcal{N}(0,1)`.
#
# The support of the input vector is :math:`[0, +\infty[ \times \mathbb{R}`
dist_X1 = ot.Exponential(1.0)
dist_X2 = ot.Normal()
dist_X = ot.JointDistribution([dist_X1, dist_X2])


# %%
# We can draw the isolines of the PDF of the distribution `dist_X`:
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 8)
graph_PDF = dist_X.drawPDF([0.0, -10.0], [20.0, 10.0])
graph_PDF.setTitle(r"2D-PDF of the input variables $(X_1, X_2)$")
graph_PDF.setXTitle(r"$x_1$")
graph_PDF.setYTitle(r"$x_2$")
graph_PDF.setLegendPosition("lower right")
contours = graph_PDF.getDrawable(0).getImplementation()
contours.setColorMapNorm("log")
graph_PDF.setDrawable(contours, 0)
view = otv.View(graph_PDF, square_axes=True)


# %%
# We consider the model from :math:`\Rset^2` into  :math:`\Rset` defined by:
#
# .. math::
#
#    \model : (x_1, x_2) \mapsto x_1 x_2
#
# We start by drawing the isolines of the model :math:`\model`.
g = ot.SymbolicFunction(["x1", "x2"], ["x1 * x2"])
graph_model = g.draw([0.0, -10.0], [20.0, 10.0])
graph_model.setXTitle(r"$x_1$")
graph_model.setYTitle(r"$x_2$")
graph_model.setTitle(r"Isolines of the model : $g$")
view = otv.View(graph_model, square_axes=True)


# %%
# We consider  the univariate output variable :
#
# .. math::
#
#   Y = \model(\inputRV)
#
# We want to estimate the probability :math:`P_f` of the output variable to be greater than a prescribed threshold :math:`s=10` : this is the failure event.
# This probability is simply expressed for a continuous random vector :math:`\inputRV` as:
#
# .. math::
#    :label: PfDef
#
#    P_f = \Prob{Y \geq s} = \int_{\set{D}} \mathbf{1}_{\set{D}}(x) \pdf d\vect{x}
#
# where:
#
# .. math::
#
#  \set{D} = \{ (x_1, x_2) \in [0,+\infty[ \times \mathbb{R} \, | \,  \model(x_1, x_2) \geq s \}
#
# is the failure domain and :math:`\inputMeasure` is the probability density function (PDF)
# of :math:`\inputRV`.

# %%
# We first define RandomVector objects and the failure event associated to the output random variable.
vector_X = ot.RandomVector(dist_X)
vector_Y = ot.CompositeRandomVector(g, vector_X)
s = 10.0
event = ot.ThresholdEvent(vector_Y, ot.Greater(), s)


# %%
# The boundary of the failure domain can easily be represented as it is a branch of an hyperbole: the
# boundary is the graph of the function defined from :math:`\Rset` into  :math:`\Rset` by:
#
# .. math::
#    :label: defH
#
#    h : x_1 \mapsto x_2 = \frac{s}{x_1}
#
# The boundary of the failure domain is also the isoline of the model :math:`\model` associated to the
# level :math:`s`:
#
# .. math::
#
#    \partial \set{D} =  \{(x_1, x_2)\, |\, \model(x_1, x_2) = s \}
#
# We can draw it with the `draw` method of the function :math:`\model`.

# %%
nb_points = 101
graph_g = g.draw([0.0, -10.0], [20.0, 10.0], [nb_points] * 2)
draw_boundary = graph_g.getDrawable(0)
draw_boundary.setLevels([s])
draw_boundary.setLegend(r'Boundary $\partial \mathcal{D}$')
graph_g.setDrawables([draw_boundary])

# %%
texts = [r" $\mathcal{D} = \{(x_1, x_2)\, |\, g(x_1, x_2) \geq 10 \}$"]
text_graph = ot.Text([[10.0, 3.0]], texts)
text_graph.setTextSize(1)
text_graph.setColor("black")
graph_g.add(text_graph)

# %%
graph_g.setTitle("Failure domain in the physical space")
graph_g.setXTitle(r"$x_1$")
graph_g.setYTitle(r"$x_2$")
graph_g.setLegendPosition('topright')

view = otv.View(graph_g, square_axes=True)

# %%
# We can superimpose the event boundary with the bivariate PDF insolines of the input distribution:
draw_boundary.setColor("black")
graph_PDF.add(draw_boundary)
graph_PDF.setLegendPosition("lower right")
view = otv.View(graph_PDF, square_axes=True)

# %%
# From the previous figure, we observe that in the failure domain, the PDF takes small
# (and even very small) values.
# Consequently the failure probability :math:`P_f` is also expected to be small.
# The FORM/SORM methods estimate the failure probability.
#

# %%
# The FORM/SORM approximations
# ----------------------------
#
# The basic steps of the FORM and SORM algorithms are:
#
# - use an isoprobabilistic transformation to map the input random vector into the standard space;
# - find the design point which is the nearest point to the origin in the standard space;
# - estimate the probability.
#

# %%
# Isoprobabilistic transformation
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# The interest of the  isoprobabilistic transformation is the rotational
# invariance of the distribution in the standard space. This property reduces the dimension
# of the problem to 1. See :ref:`Isoprobabilistic transformation <isoprobabilistic_transformation>` for more theoretical details.
#
# OpenTURNS has several isoprobabilistic transformations, depending on the distribution of the input random vector:
#
# - the Nataf transformation is used if the input distribution has a normal copula,
# - the Generalized Nataf transformation is used if the input distribution has an elliptical copula,
# - the Rosenblatt transformation is used in any other cases.
#
# The Nataf and  Rosenblatt transformations map the input random vector into a vector that follows a
# normal distribution with zero mean and unit variance. The Generalized Nataf transformation maps the
# input random vector into a vector that follows the standard elliptical distribution associated to the
# elliptical copula of the input distribution.
#
# In this example, the input distribution is not elliptical so the isoprobabilistic transformation is the
# Rosenblatt transformation.
#
print("Is Elliptical ? ", dist_X.isElliptical())

# %%
# The Rosenblatt transformation :math:`T` is defined by:
#
# .. math::
#    :label: defT
#
#    T : \vect{x} \mapsto \vect{z}
#
# such that the random vector :math:`\standardRV = T(\inputRV)` follows a bivariate normal distribution
# with zero mean and unit variance. It follows that the components :math:`Z_1` and
# :math:`Z_2` are independent.
#
# We detail the Rosenblatt transform in this simple case where the input random vector :math:`\inputRV`
# has independent components. Then, the Rosenblatt transform is defined by:
#
# .. math::
#
#    z_i = \Phi^{-1} \circ F_i(x_i)
#
# where :math:`F_i` is the cumulative distribution function (CDF) of :math:`X_i` and
# :math:`\Phi` the CDF of the univariate normal distribution with zero mean and unit variance.
# Note that in this example, :math:`\Phi^{-1} \circ F_2 = I_d` as :math:`F_2 = \Phi`.

# The isoprobabilistic transform and its inverse are methods of the distribution :
transformation = dist_X.getIsoProbabilisticTransformation()
inverse_transformation = dist_X.getInverseIsoProbabilisticTransformation()

# %%
# Let us detail this transformation, step by step.
# We draw a realization of the random input vector. This point is said to be in the physical space.
xi = vector_X.getRealization()

# %%
# We build `zi` the mapping of `xi` through the Rosenblatt transformation.
# The point `zi` is said to be in the standard space. Note that the second component remained unchanged.
ui = [dist_X1.computeCDF(xi[0]), dist_X2.computeCDF(xi[1])]
zi = [ot.Normal().computeQuantile(ui[0])[0], ot.Normal().computeQuantile(ui[1])[0]]
print(xi, "->", ui, "->", zi)

# %%
# We also build the isoprobabilistic transform :math:`T_1` and its inverse :math:`T_1^{-1}` for the
# first marginal:
#
# .. math::
#    :label: detT1
#
#    T_1 = \Phi^{-1} \circ F_1
#
transform_X1 = dist_X1.getIsoProbabilisticTransformation()
inverse_transform_X1 = dist_X1.getInverseIsoProbabilisticTransformation()

# %%
# We can implement the transformation using :math:`T_1` on the first components
# directly using :math:`T` on both components `xi`:
zi1D = [transform_X1([xi[0]])[0], xi[1]]
zi2D = transformation(xi)

# %%
# We can check the result of our experiment : we observe the results are the same.
print("zi = ", zi)
print("zi1D = ", zi1D)
print("zi2D = ", zi2D)


# %%
# The model in the standard space is defined by:
#
# .. math::
#
#     \widetilde{\model} = \model \circ T^{-1}
#
# We can define it using the capacities of the composition of functions implemented in the library.
g_tilde = ot.ComposedFunction(g, inverse_transformation)

# %%
# The failure domain in the standard space is defined by:
#
# .. math::
#
#  \set{\widetilde{D}} = \{ (z_1, z_2) \in [0,+\infty[ \times \mathbb{R} \, | \,  \widetilde{\model}(z_1, z_2) \geq s \}
#
# and its boundary is defined by:
#
# .. math::
#
#    \partial \set{\widetilde{D}} = \{ (z_1, z_2) \in [0,+\infty[ \times \mathbb{R} \, | \,
#       \widetilde{\model}(z_1, z_2) = s \}
#

# %%
# We draw the graph of :math:`\widetilde{g}` in the standard space.
graph_standard_space = g_tilde.draw([0.0, 0.0], [7.0, 7.0], [101] * 2)

draw_boundary_stand_space = graph_standard_space.getDrawable(0)
draw_boundary_stand_space.setLevels([s])
draw_boundary_stand_space.setLegend(r"Boundary $\partial \mathcal{\tilde{D}}$")
graph_standard_space.setDrawables([draw_boundary_stand_space])

graph_standard_space.setXTitle(r"$z_1$")
graph_standard_space.setYTitle(r"$z_2$")
graph_standard_space.setTitle('Failure domain in the standard space')

# %%
# We add the origin to the previous graph.
cloud = ot.Cloud([0.0], [0.0])
cloud.setColor("black")
cloud.setPointStyle("fcircle")
cloud.setLegend("origin")
graph_standard_space.add(cloud)

# Some annotations
texts = [r"$\mathcal{\tilde{D}} = \{(z_1, z_2)\, |\, \tilde{g}(z_1, z_2) \geq 10 \}$"]
text_graph = ot.Text([[4.0, 3.0]], texts)
text_graph.setTextSize(1)
text_graph.setColor("black")
graph_standard_space.add(text_graph)

graph_standard_space.setLegendPosition("topright")
view = otv.View(graph_standard_space, square_axes=True)

# %%
# The design point
# ^^^^^^^^^^^^^^^^
#
# Due to the spherical distribution in the standard space, the area that contributes
# the most to the integral defining the probability is the vicinity of the closest point
# of the failure domain to the origin of the standard space.
# Then the second step of the method is to find this point, *the design point*, through a
# minimization problem under constraints.

# %%
# We configure the Cobyla solver that we use for the optimization :
solver = ot.Cobyla()
solver.setMaximumIterationNumber(10000)
solver.setMaximumAbsoluteError(1.0e-3)
solver.setMaximumRelativeError(1.0e-3)
solver.setMaximumResidualError(1.0e-3)
solver.setMaximumConstraintError(1.0e-3)

# %%
# We build the FORM algorithm with its basic constructor. The starting point for the optimization
# algorithm is the mean of the input distribution.
algo_FORM = ot.FORM(solver, event, dist_X.getMean())

# %%
# We are ready to run the algorithm and store the result.
algo_FORM.run()
result = algo_FORM.getResult()

# %%
# The design point can be retrieved in both physical and standard space with respectively the
# `getPhysicalSpaceDesignPoint` and `getStandardSpaceDesignPoint` methods. We denote them respectively
# :math:`\vect{x}^*` and :math:`\vect{z}^*`.
design_point_physical_space = result.getPhysicalSpaceDesignPoint()
design_point_standard_space = result.getStandardSpaceDesignPoint()
print("Design point in physical space : ", design_point_physical_space)
print("Design point in standard space : ", design_point_standard_space)

# %%
# We can get the Hasofer index with the `getHasoferReliabilityIndex` method which is the distance of
# the design point to the origin:
beta_HL = result.getHasoferReliabilityIndex()
print("Hasofer index : ", beta_HL)

# %%
# We visualize the design point on the previous graph.
cloud = ot.Cloud([design_point_standard_space])
cloud.setColor("red")
cloud.setPointStyle("fcircle")
cloud.setLegend(r"design point $z^*$")
graph_standard_space.add(cloud)
graph_standard_space.setGrid(True)
graph_standard_space.setLegendPosition("lower right")
cc = ot.Curve(
    [0.0, design_point_standard_space[0]],
    [0.0, design_point_standard_space[1]],
    r"$\beta_{HL}$ distance",
)
cc.setLineStyle("dashed")
cc.setColor("black")
graph_standard_space.add(cc)
graph_standard_space.setLegendPosition('topright')
view = otv.View(graph_standard_space, square_axes=True)

# %%
# The FORM approximation
# ^^^^^^^^^^^^^^^^^^^^^^
#
# The last step of the FORM algorithm is to replace the failure domain boundary by the hyperplane
# which is tangent to the failure domain at the design point in the standard space.
# To draw this hyperplane :math:`\mathcal{P}_{\vect{z}^*}`, we define the function from
# :math:`\Rset^2` to :math:`\Rset` defined by:
#
# .. math::
#
#    M \rightarrow \scalarproduct{\nabla \widetilde{\model}(\vect{z}^*)}{\vect{Z^*M}}
#
# where :math:`\nabla \vect{\widetilde{\model}(\vect{z}^*)}` is the gradient of the
# function :math:`\widetilde{\model}`
# at the design point :math:`Z^*(\vect{z}^*)`.
# Then, the tangent hyperplane is the isoline associated to the zero level of the previous function :
#
# .. math::
#
#    \mathcal{P}_{z^*} = \{ \vect{z} \in \Rset^2 \, | \,
#                \scalarproduct{\nabla \widetilde{\model}(\vect{z}^*)}{\vect{Z^*M}} = 0\}
#
# We can use the class LinearFunction.
center = design_point_standard_space
grad_design_point = g_tilde.gradient(design_point_standard_space)
constant = [0.0]
linear_mat = ot.Matrix(1, 2)
linear_mat[0, 0] = grad_design_point[0, 0]
linear_mat[0, 1] = grad_design_point[1, 0]
linear_proj = ot.LinearFunction(center, constant, linear_mat)

graph_tangent = linear_proj.getMarginal(0).draw([0.0, 0.0], [7.0, 7.0], [101] * 2)
draw_tangent = graph_tangent.getDrawable(0)
draw_tangent.setLevels([0])
draw_tangent.setLegend(r"$\mathcal{\Pi}_{z^*}$ (FORM)")
draw_tangent.setColor('green')
draw_tangent.setLineStyle('dashed')
graph_standard_space.add(draw_tangent)
graph_standard_space.setLegendPosition('topright')
view = otv.View(graph_standard_space, square_axes=True)

# %%
# Depending on whether the origin of the standard space :math:`\vect{0}` belongs to the failure domain,
# the FORM probability is defined by:
#
# .. math::
#
#    P_{FORM} \approx E(-\beta_{HL}) & \quad  \mbox{if }  \vect{0} \notin \set{\widetilde{D}} \\
#    P_{FORM} \approx E(+\beta_{HL}) & \quad  \mbox{if }  \vect{0} \in \set{\widetilde{D}}
#
# where :math:`E(.)` is the marginal cumulative distribution function along any direction of
# the spherical distribution in the standard space. In this example, this is the normal distribution.
# So we have:
#
isOriginFail = result.getIsStandardPointOriginInFailureSpace()
normal = ot.Normal()
if isOriginFail:
    pf_FORM = normal.computeCDF(beta_HL)
else:
    pf_FORM = normal.computeCDF(-beta_HL)
print("FORM : Pf_FORM = ", pf_FORM)

# %%
# This failure probability is implemented but the FORM algorithm and can be obtained
# with the `getEventProbability` method. We check we have the same result.
pf = result.getEventProbability()
print("Probability of failure (FORM) Pf_FORM  = ", pf)

# %%
# The SORM approximation
# ^^^^^^^^^^^^^^^^^^^^^^
#
# The SORM approximation uses the main curvatures :math:`\kappa_i^0` of the homothetic of the failure domain
# at distance 1 from the origin. These curvatures are calculated  at the design point.
# They are linked to the curvatures :math:`\kappa_i` of the failure domain by:
#
# .. math::
#
#    \kappa_i^0 = \beta_{HL} \kappa_i
#
# The Breitung approximation is valid for :math:`\beta_{HL} \rightarrow +\infty` and is defined by :
#
# .. math::
#
#    P_{SORM, Breitung} \approx E(-\beta_{HL}) \prod_{i=1}^{d-1} \dfrac{1}{\sqrt{1+\kappa_i^0}} &
#                      \mbox{if }  \vect{0} \notin \set{\widetilde{D}} \\
#    P_{SORM, Breitung} \approx E(+\beta_{HL}) \prod_{i=1}^{d-1} \dfrac{1}{\sqrt{1+\kappa_i^0}} &
#                      \mbox{if }  \vect{0} \in \set{\widetilde{D}}
#
# and approximates the boundary by the osculating paraboloid at the design point.
#
# Note that the term :math:`\kappa_i^0` does not depend on :math:`\beta_{HL}`.

# %%
# In this example, we can easily implement the boundary of the failure domain in the
# physical space, using the function :math:`h` defined in :eq:`defH`.
#
# In the standard space, the boundary is defined by the composed function
# :math:`z_1 \mapsto h \circ T_1^{-1}(z_1)`.
failure_boundary_physical_space = ot.SymbolicFunction(["x"], ["10.0 / x"])
failure_boundary_standard_space = ot.ComposedFunction(
    failure_boundary_physical_space, inverse_transform_X1
)
# %%
# We need the value of the second derivative of the failure boundary function
# at the abscissa of the design point in the standard space:
z1_star = [design_point_standard_space[0]]
dz1_star = failure_boundary_standard_space.getGradient().gradient(z1_star)
d2z1_star = failure_boundary_standard_space.getHessian().hessian(z1_star)
print("first component of the design point = ", z1_star[0])
print("second component of the design point = ", failure_boundary_standard_space(z1_star)[0])
print("value of the hessian of the failure boundary at this abscissa= ", d2z1_star[0, 0, 0])


# %%
# In the standard space, the osculating parabola :math:`\mathcal{P}_{\vect{z}^*}`
# at :math:`\vect{z}^*` is the graph of the function defined by:
#
# .. math::
#
#    z_1 \mapsto   h \circ T_1^{-1} (z_1^*) + \frac{d}{du_1} (h \circ T_1^{-1})(z_1^*) (z_1-z_1^*) +
#     \frac{1}{2} \frac{d^2}{dz_1^2} (h \circ T_1^{-1})(z_1^*) (z_1-z_1^*)^2
#
z = np.linspace(1.1, 4.0, 100)
parabola = (
    failure_boundary_standard_space(z1_star)[0]
    + dz1_star[0, 0] * (z - z1_star)
    + 0.5 * d2z1_star[0, 0, 0] * (z - z1_star) ** 2
)
curve_parabola = ot.Curve(z, parabola, r"$\mathcal{P}_{z^*}$ (SORM)")
curve_parabola.setLineStyle("dashed")
curve_parabola.setColor("orange")
graph_standard_space.add(curve_parabola)
graph_standard_space.setLegendPosition('topright')
view = otv.View(graph_standard_space)


# %%
# The next step is to estimate the principal curvatures of the osculating paraboloid.
#
# For any regular function :math:`\ell: \Rset \rightarrow \Rset` the curvature :math:`\kappa(x)` at the point :math:`x` in
# cartesian coordinates reads as:
#
# .. math::
#
#    \kappa(x) = \frac{\ell''(x)}{(1+[\ell'(x)]^2)^{3/2}}.
#
# For the oscillating parabola of concern we use the gradient and hessian previously computed:
#
curvature = (d2z1_star[0, 0, 0]) / (1 + (dz1_star[0, 0]) ** 2) ** (3 / 2)
print("Curvature (analytic formula) = ", curvature)


# %%
# We build the SORM algorithm and run it :
algo_SORM = ot.SORM(solver, event, dist_X.getMean())
algo_SORM.run()

# %%
# The SORM result is obtained with the `getResult` method:
result_SORM = algo_SORM.getResult()

# %%
# The principal curvatures of the osculating paraboloid at the design point is obtained by the
# `getSortedCurvatures` method:
print("Curvature (library) = ", result_SORM.getSortedCurvatures()[1])


# %%
# Once the curvature is computed, there are several ways of approximating the failure probability :math:`P_f`.
# The library implements the Breitung, Hohenbichler and Tvedt estimates.
# We detail here the calculus of the Breitung approximation.
coeff = (1.0 + beta_HL * curvature) ** (-0.5)
if isOriginFail:
    pf_SORM = (normal.computeCDF(beta_HL)) * coeff
else:
    pf_SORM = (normal.computeCDF(-beta_HL)) * coeff
print("SORM : Pf_SORM = ", pf_SORM)

# %%
# We can compare with the different estimators:
pf_Breitung = result_SORM.getEventProbabilityBreitung()
pf_Hohenbichler = result_SORM.getEventProbabilityHohenbichler()
pf_Tvedt = result_SORM.getEventProbabilityTvedt()

print("Probability of failure (SORM Breintung) Pf = ", pf_Breitung)
print("Probability of failure (SORM Hohenbichler) Pf = ", pf_Hohenbichler)
print("Probability of failure (SORM Tvedt) Pf = ", pf_Tvedt)

# %%
# Display all figures
otv.View.ShowAll()

# %%
# Reset default settings
ot.ResourceMap.Reload()
