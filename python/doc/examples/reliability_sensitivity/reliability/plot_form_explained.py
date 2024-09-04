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
import openturns as ot
import openturns.viewer as otv
import numpy as np

# %%
# Context
# -------
#
# We consider a bivariate random vector :math:`X = (X_1, X_2)` with the following independent marginals:
#
# - an exponential distribution with parameter :math:`\lambda=1`, :math:`X_1 \sim \mathcal{E}(1.0)` ;
# - a standard unit gaussian :math:`X_2 \sim \mathcal{N}(0,1)`.
#
# The support of the input vector is :math:`[0, +\infty[ \times \mathbb{R}`
#

# %%
distX1 = ot.Exponential(1.0)
distX2 = ot.Normal()
distX = ot.JointDistribution([distX1, distX2])


# %%
# We can draw the bidimensional PDF of the distribution `distX` over :math:`[0,-10] \times [10,10]`:
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 8)
graphPDF = distX.drawPDF([0, -10], [10, 10])
graphPDF.setTitle(r"2D-PDF of the input variables $(X_1, X_2)$")
graphPDF.setXTitle(r"$x_1$")
graphPDF.setYTitle(r"$x_2$")
graphPDF.setLegendPosition("lower right")
contours = graphPDF.getDrawable(0).getImplementation()
contours.setColorMapNorm("log")
graphPDF.setDrawable(contours, 0)
view = otv.View(graphPDF, square_axes=True)


# %%
# We consider the model :math:`f : (x_1, x_2) \mapsto x_1 x_2` which maps the random input vector :math:`\vect{X}` to the output variable :math:`Y=f(\vect{x}) \in \mathbb{R}`.
# We also draw the isolines of the model `f`.
#
f = ot.SymbolicFunction(["x1", "x2"], ["x1 * x2"])
graphModel = f.draw([0.0, -10.0], [10.0, 10.0])
graphModel.setXTitle(r"$x_1$")
graphModel.setXTitle(r"$x_2$")
graphModel.setTitle(r"Isolines of the model : $Y = f(X)$")
view = otv.View(graphModel, square_axes=True)


# %%
# We want to estimate the probability :math:`P_f` of the output variable to be greater than a prescribed threshold :math:`s=10` : this is the failure event.
# This probability is simply expressed as an integral:
#
# .. math::
#
#    P_f = \int_{\mathcal{D}} \mathbf{1}_{\mathcal{D}}(x) df_{X_1,X_2}(x)
#
# where :math:`\mathcal{D} = \{ (x_1, x_2) \in [0,+\infty[ \times \mathbb{R} / f(x_1, x_2) = x_1 x_2 \geq s \}` is the failure domain.

# %%
# We first define RandomVector objects and the failure event associated to the output random variable.
vectorX = ot.RandomVector(distX)
vectorY = ot.CompositeRandomVector(f, vectorX)
s = 10.0
event = ot.ThresholdEvent(vectorY, ot.Greater(), s)


# %%
# This event can easily be represented with a 1D curve as it is a branch of an hyperbole.
# If :math:`y =  x_1 x_2 = s`, then the boundary of the domain of failure is the curve :
#
# .. math::
#    :label:`functionh`
#
#   h : x_1 \mapsto \frac{s}{x_1}
#
# This event is also the isoline of the model :math:`f` associated to the
# level :math:`s`. We can draw it with the 'draw' method of the model
# :math:`f`.

# %%
nbPoints = 101
myGraph = f.draw([0.0, -10.0], [10.0, 10.0], [nbPoints]*2)
draw_frontier = myGraph.getDrawable(0)
draw_frontier.setLevels([s])
draw_frontier.setLegend(r'$Y = $ ' + str(s))
myGraph.setDrawables([draw_frontier])

# %%
texts = [r" Event : $\mathcal{D} = \{Y \geq $" + str(s) + "$\}$"]
myText = ot.Text([[4.0, 3.0]], texts)
myText.setTextSize(1)
myText.setColor("black")
myGraph.add(myText)

# %%
myGraph.setTitle("Representation of the failure domain")
myGraph.setXTitle(r"$X_1$")
myGraph.setYTitle(r"$X_2$")
myGraph.setLegendPosition('topright')

view = otv.View(myGraph, square_axes=True)

# %%
# We can superimpose the event boundary with the 2D-PDF ot the input variables :
#
draw_frontier.setColor("black")
graphPDF.add(draw_frontier)
graphPDF.setLegendPosition("lower right")
view = otv.View(graphPDF, square_axes=True)

# %%
# From the previous figure we observe that in the failure domain the PDF takes small (and even very small) values.
# Consequently the probability of the failure, the integral :math:`P_f` is also expected to be small.
# The FORM/SORM methods estimate the failure probability.
#

# %%
# The FORM approximation
# ----------------------
#
# The basic steps of the FORM (or SORM) algorithm are :
#
# - use an isoprobabilistic transformation to map the input random vecyor into the standard space;
# - find the design point which is the nearest point wrt the origin in the standard space;
# - estimate the probability.
#
# Isoprobabilistic transformation
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#

# %%
# The interest of such the  isoprobabilistic transformation is the rotational
# invariance of the
# distribution in the standard space.
#
# OpenTURNS has several isoprobabilistic transformations, depending on the distribution of the input random vector:
#
# - the Nataf transformation is used if the input distribution has a normal copula, 
# - the Generalized Nataf transformation is used if the input distribution has an elliptical copula,
# - the Rosenblatt transformation is used in any cases.
#
# The Nataf and  Rosenblatt transformations map the input random vector into a vector that follows a
# normal distribution with zero mean and unit variance. The Generalized Nataf transformation maps the
# input random vector into a vector that follows the standard elliptical distribution associated to the
# elliptical copula.
#
# In this case the input distribution is not elliptical so the isoprobabilistic transformation is the
# Rosenblatt transformation.
#
print("Is Elliptical ? ", distX.isElliptical())

# %%
# The Rosenblatt transformation :math:`T` is defined as:
#
# .. math::
#    T : \vect{X} \mapsto \vect{Z}
#
# where the components of :math:`\vect{Z}` are independent normal
# distributions with zero mean and unit variance.
#
# The isoprobabilistic transform and its inverse are methods of the distribution `distX` :
transformation = distX.getIsoProbabilisticTransformation()
inverseTransformation = distX.getInverseIsoProbabilisticTransformation()

# %%
# We detail the Rosenblatt transform in this simple case.
# In this example, we consider independent variables so the transform is simpler as it is a marginal
# transformation: 
#
# .. math::
#    Z_i = \Phi^{-1} \circ F_i(X_i)
#
# where :math:`F_i` is the cumulated distribution function of :math:`X_i` and
# :math:`\Phi` the one of the standard normal distribution. Note that in this example,
# :math:`\Phi^{-1} \circ F_2 = Id`.
#

# %%
# We draw a realization of the random input vector. This point is said to be in the physical space.
# We shall focus on the first component.
xi = vectorX.getRealization()

# %%
# We build `zi` the mapping of `xi`. The point `zi` is said to be in the standard space.
ui = [distX1.computeCDF(xi[0]), xi[1]]
zi = [-ot.Normal().computeInverseSurvivalFunction(ui[0])[0], ui[1]]
print(xi, "->", zi)

# %%
# We also build the isoprobabilistic transform :math:`T_1` and its inverse :math:`T_1^{-1}` for the
# first marginal :
transformX1 = distX1.getIsoProbabilisticTransformation()
inverseTransformX1 = distX1.getInverseIsoProbabilisticTransformation()

# %%
# We can check the result of our experiment using the transformation implemented by the distribution :
# and observe the results are the same.
zi1D = [transformX1([xi[0]])[0], xi[1]]
zi2D = transformation(xi)
print("zi = ", zi)
print("zi1D = ", zi1D)
print("zi2D = ", zi2D)


# %%
# The model on the standard space is defined by:
#
# .. math::
#    g = f \circ T^{-1}
#
# We can define it using the capacities of functions composition of the library.
g = ot.ComposedFunction(f, inverseTransformation)
graphStandardSpace = g.draw([0.0, 0.0], [7.0, 7.0], [101]*2)

draw_frontier_stand_space = graphStandardSpace.getDrawable(0)
draw_frontier_stand_space.setLevels([s])
draw_frontier_stand_space.setLegend(r"Boundary of the event $\partial \mathcal{D}$")
draw_frontier_stand_space.setColor("blue")
graphStandardSpace.setDrawables([draw_frontier_stand_space])

graphStandardSpace.setXTitle(r"$u_1$")
graphStandardSpace.setYTitle(r"$u_2$")
graphStandardSpace.setTitle('Failure event in the standard space')

# %%
# We add the origin to the previous graph.
cloud = ot.Cloud([0.0], [0.0])
cloud.setColor("black")
cloud.setPointStyle("fcircle")
cloud.setLegend("origin")
graphStandardSpace.add(cloud)

# Some annotation
s = [r"Event : $\mathcal{D} = \{Y \geq $" + str(s) + "$\}$"]
myText = ot.Text([[4.0, 3.0]], texts)
myText.setTextSize(1)
myText.setColor("black")
graphStandardSpace.add(myText)

graphStandardSpace.setLegendPosition("lower right")
view = otv.View(graphStandardSpace, square_axes=True)

# %%
# The design point
# ^^^^^^^^^^^^^^^^
#
# Due to the spherical distribution in the standard space, the area that contributes
# the most to the integral defining the probability is the vicinity of the closest point
# of the failure domain to the origin of the standard space.
# Then the second step of the method is to find this point, *the design point*, through a
# minimization problem under constraints.
#

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
# algorithm is the mean of the input variables.
algoFORM = ot.FORM(solver, event, distX.getMean())

# %%
# We are ready to run the algorithm and store the result.
algoFORM.run()
result = algoFORM.getResult()

# %%
# The design point can be retrieved in both physical and standard space with respectively the
# `getPhysicalSpaceDesignPoint` and `getStandardSpaceDesignPoint` methods.
designPointPhysicalSpace = result.getPhysicalSpaceDesignPoint()
designPointStandardSpace = result.getStandardSpaceDesignPoint()
print("Design point in physical space : ", designPointPhysicalSpace)
print("Design point in standard space : ", designPointStandardSpace)


# %%
# We can get the Hasofer index with the `getHasoferReliabilityIndex` method which is the distance of the design point to the origin:
betaHL = result.getHasoferReliabilityIndex()
print("Hasofer index : ", betaHL)

# %%
# We visualize it on the previous graph.
cloud = ot.Cloud([designPointStandardSpace])
cloud.setColor("red")
cloud.setPointStyle("fcircle")
cloud.setLegend("design point")
graphStandardSpace.add(cloud)
graphStandardSpace.setGrid(True)
graphStandardSpace.setLegendPosition("lower right")
cc = ot.Curve(
    [0.0, designPointStandardSpace[0]],
    [0.0, designPointStandardSpace[1]],
    r"$\beta_{HL}$ distance",
)
cc.setLineStyle("dashed")
cc.setColor("black")
graphStandardSpace.add(cc)
graphStandardSpace.setLegendPosition('topright')
view = otv.View(graphStandardSpace, square_axes=True)

# %%
# Estimate the failure probability
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# The last step of the FORM algorithm is to replace the domain of integration by the half-space
# delimited by the tangent at the design point in the standard space.
# To draw the tangent, we define the function:
#
# .. math::
#    M(x,y) \rightarrow \leftangle \nabla g(U^*), U^*M \rightangle
#
# Then, the tangent half-space is the isoline associated to the zero level.
# %%
center = designPointStandardSpace
grad_design_point = g.gradient(designPointStandardSpace)
constant = [0.0]
linearMat = ot.Matrix(1,2)
linearMat[0,0] = grad_design_point[0,0]
linearMat[0,1] = grad_design_point[1,0]
linearProj = ot.LinearFunction(center, constant, linearMat)

graph_tangent = linearProj.getMarginal(0).draw([0.0, 0.0], [7.0, 7.0], [101]*2)
draw_tangent = graph_tangent.getDrawable(0)
draw_tangent.setLevels([0])
draw_tangent.setLegend(r"$\mathcal{\Pi}_{u_0}$ (FORM)")
draw_tangent.setColor('green')
draw_tangent.setLineStyle('dashed')
graphStandardSpace.add(draw_tangent)
graphStandardSpace.setLegendPosition('topright')
view = otv.View(graphStandardSpace, square_axes=True)

# %%
# In the standard space, the FORM probability is approximated by:
#
# .. math::
#
#    P_{FORM} \approx E(-\beta_{HL}),
#
# where :math:`E(.)` is the marginal cumulative distribution function along any direction of
# the spherical distribution in the standard space. In this example, this is the normal distribution.
#
pf = ot.Normal().computeCDF(-betaHL)
print("FORM : Pf = ", pf)

# %%
# This probability of failure is the one computed in the FORMResult and obtained with the `getEventProbability` method:
pf = result.getEventProbability()
print("Probability of failure (FORM) Pf = ", pf)

# %%
# The SORM approximation
# ----------------------
#
# The SORM approximation uses the curvatures of the domain at the design point: 
#
# .. math::
#
#    P_{SORM} \approx E(-\beta_{HL}) \prod_{i=1}^{d-1} \dfrac{1}{\sqrt{1+\kappa_i \beta_{HL}}} 
#
# and approximates the frontier by the osculating paraboloid at the design point.


# %%
# In this example, we can easily implement the frontier of the event in the 
# physical space:
#
# .. math::
#
#  h : x_1 \mapsto \dfrac{s}{x_2}
#
# In the standard space, the frontier is defined by the composed function
# :math:`h \circ T^{-1}`.
failureBoundaryPhysicalSpace = ot.SymbolicFunction(["x"], ["10.0 / x"])
failureBoundaryStandardSpace = ot.ComposedFunction(
    failureBoundaryPhysicalSpace, inverseTransformX1
)
# %%
# We need the value of the second derivative of the failure boundary function
# at the design point in the standard space :
u0 = [designPointStandardSpace[0]]
du0 = failureBoundaryStandardSpace.getGradient().gradient(u0)
d2u0 = failureBoundaryStandardSpace.getHessian().hessian(u0)
print("abscissa of the design point u0  = ", u0[0])
print("value of the failure boundary at u0 = ", failureBoundaryStandardSpace(u0)[0])
print("value of the hessian of the failure boundary at u0 = ", d2u0[0, 0, 0])


# %%
# In the standard space the equation of the osculating parabola :math:`\mathcal{P}_{u_0}(x)` at :math:`u_0` is given by
#
# .. math::
#
#    \mathcal{P}_{u_0}(x) = h \circ T^{-1} (u_0) + \frac{d}{dx} (h \circ T^{-1})(u_0) (x-u_0) + \frac{1}{2} \frac{d^2}{dx^2} (h \circ T^{-1})(u_0) (x-u_0)^2
#
x = np.linspace(1.1, 4.0, 100)
parabola = (
    failureBoundaryStandardSpace(u0)[0]
    + du0[0, 0] * (x - u0)
    + 0.5 * d2u0[0, 0, 0] * (x - u0) ** 2
)
curveParabola = ot.Curve(x, parabola, r"$\mathcal{P}_{u_0}$ (SORM)")
curveParabola.setLineStyle("dashed")
curveParabola.setColor("orange")
graphStandardSpace.add(curveParabola)
graphStandardSpace.setLegendPosition('topright')
view = otv.View(graphStandardSpace)


# %%
# The next step is to estimate the principal curvatures of the osculating paraboloid.
#
# For any regular function :math:`g` the curvature :math:`\kappa(x_0)` at the point :math:`x_0` in cartesian coordinates reads as
#
# .. math::
#
#    \kappa(x_0) = \frac{g''(x_0)}{(1+[g'(x_0)]^2)^{3/2}}.
#
# For the oscillating parabola of concern we use the gradient and hessian previously computed :
#
curvature = (d2u0[0, 0, 0]) / (1 + (du0[0, 0]) ** 2) ** (3 / 2)
print("Curvature (analytic formula) = ", curvature)


# %%
# We build the SORM algorithm and run it :
algoSORM = ot.SORM(solver, event, distX.getMean())
algoSORM.run()

# %%
# The SORM result is obtained with the `getResult` method :
resultSORM = algoSORM.getResult()

# %%
# The principal curvatures of the osculating paraboloid at the design point is obtained by the
# `getSortedCurvatures` method :
print("Curvature (estimated) = ", resultSORM.getSortedCurvatures()[1])


# %%
# Once the curvature is obtained there are several ways of approximating the failure probability :math:`P_f`.
# The library implements the Breitung, Hohenbichler and Tvedt estimates.
#
# For instance, the Breitung approximation gives
#
# .. math::
#
#    P_f \approx E(\beta_{HL}) \frac{1}{\sqrt{1+\beta_{HL}\kappa}}
#
coeff = (1.0 + betaHL * curvature) ** (-0.5)
pf = (1.0 - ot.Normal().computeCDF(betaHL)) * coeff
print("SORM : Pf = ", pf)

# %%
# We can compare with the different estimators:
pfBreitung = resultSORM.getEventProbabilityBreitung()
pfHohenbichler = resultSORM.getEventProbabilityHohenbichler()
pfTvedt = resultSORM.getEventProbabilityTvedt()

print("Probability of failure (SORM Breintung) Pf = ", pfBreitung)
print("Probability of failure (SORM Hohenbichler) Pf = ", pfHohenbichler)
print("Probability of failure (SORM Tvedt) Pf = ", pfTvedt)

# %%
# Display all figures
otv.View.ShowAll()

# %%
# Reset default settings
ot.ResourceMap.Reload()
