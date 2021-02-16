"""
Create a polynomial chaos metamodel
===================================
"""
# %%
# In this example we are going to create a global approximation of a model response using functional chaos.
#
# Let :math:`h` be the function defined by:
#
# .. math::
#    g(\mathbf{x}) = \left[\cos(x_1 + x_2), (x_2 + 1) e^{x_1}\right]
# 
#
# for any :math:`\mathbf{x}\in\mathbb{R}^2`. 
#
# We assume that 
#
# .. math::
#    X_1 \sim \mathcal{N}(0,1) \textrm{ and } X_2 \sim \mathcal{N}(0,1)
#
# and that :math:`X_1` and :math:`X_2` are independent.
#
# An interesting point in this example is that the output is multivariate. This is why we are going to use the `getMarginal` method in the script in order to select the output marginal that we want to manage. 

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We first create the function `model`.

# %%
ot.RandomGenerator.SetSeed(0)
dimension = 2
input_names = ['x1', 'x2']
formulas = ['cos(x1 + x2)', '(x2 + 1) * exp(x1)']
model = ot.SymbolicFunction(input_names, formulas)

# %%
# Then we create a sample `inputSample` and compute the corresponding output sample `outputSample`.

# %%
distribution = ot.Normal(dimension)
samplesize = 100
inputSample  = distribution.getSample(samplesize)
outputSample = model(inputSample)

# %%
# Create a functional chaos model. 
# First, we need to fit a distribution on the input sample. We can do this automatically with the Lilliefors test.

# %%
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-LillieforsMaximumSamplingSize", 100)

# %%
algo = ot.FunctionalChaosAlgorithm(inputSample, outputSample)
algo.run()
result = algo.getResult()
metamodel = result.getMetaModel()

# %%
# Plot the second output of our model depending on :math:`x_2` with :math:`x_1=0.5`. In order to do this, we create a `ParametricFunction` and set the value of :math:`x_1`. Then we use the `getMarginal` method to extract the second output (which index is equal to 1). 

# %%
x1index = 0
x1value = 0.5
x2min = -3.
x2max = 3.
outputIndex = 1
metamodelParametric = ot.ParametricFunction(metamodel, [x1index], [x1value])
graph = metamodelParametric.getMarginal(outputIndex).draw(x2min, x2max)
graph.setLegends(["Metamodel"])
modelParametric = ot.ParametricFunction(model, [x1index], [x1value])
curve = modelParametric.getMarginal(outputIndex).draw(x2min, x2max).getDrawable(0)
curve.setColor('red')
curve.setLegend("Model")
graph.add(curve)
graph.setLegendPosition("bottomright")
graph.setXTitle("X2")
graph.setTitle("Metamodel Validation, output #%d" % (outputIndex))
view = viewer.View(graph)

# %%
# We see that the metamodel fits approximately to the model, except perhaps for extreme values of :math:`x_2`. However, there is a better way of globally validating the metamodel, using the `MetaModelValidation` on a validation design of experiment. 

# %%
n_valid = 100
inputTest = distribution.getSample(n_valid)
outputTest = model(inputTest)


# %%
# Plot the corresponding validation graphics.

# %%
val = ot.MetaModelValidation(inputTest, outputTest, metamodel)
Q2 = val.computePredictivityFactor()
graph = val.drawValidation()
graph.setTitle("Metamodel validation Q2="+str(Q2))
view = viewer.View(graph)

# %%
# The coefficient of predictivity is not extremely satisfactory for the first output, but is would be sufficient for a central dispersion study.
# The second output has a much more satisfactory Q2: only one single extreme point is far from the diagonal of the graphics.

# %%
# Compute and print Sobol' indices
# --------------------------------

# %%
chaosSI = ot.FunctionalChaosSobolIndices(result) 
print(chaosSI.summary())

# %%
# Let us analyse the results of this global sensitivity analysis.
#
# * We see that the first output involves significant multi-indices with total degree 4. The contribution of the interactions are very significant in this model. 
# * The second output involves multi-indices with total degrees from 1 to 7, with a significant contribution of multi-indices with total degress 5 and 7. The first variable is especially significant, with a significant contribution of the interactions. 

# %%
# Draw Sobol' indices.

# %%
sensitivityAnalysis = ot.FunctionalChaosSobolIndices(result)
first_order = [sensitivityAnalysis.getSobolIndex(i) for i in range(dimension)]
total_order = [sensitivityAnalysis.getSobolTotalIndex(i) for i in range(dimension)]

# %%
input_names = model.getInputDescription()
graph = ot.SobolIndicesAlgorithm.DrawSobolIndices(input_names, first_order, total_order)
graph.setLegendPosition("center")
view = viewer.View(graph)

# %%
# Testing the sensitivity to the degree
# -------------------------------------
#
# With the specific constructor of `FunctionalChaosAlgorithm` that we use, the `FunctionalChaosAlgorithm-MaximumTotalDegree` in the `ResourceMap` configure the maximum degree explored by the algorithm. This degree is a trade-off. 
#
# * If the maximum degree is too low, the polynomial may miss some coefficients so that the quality is lower than possible.
# * If the maximum degree is too large, the number of coefficients to explore is too large, so that the coefficients might be poorly estimated. 
#
# This is why the following `for` loop explores various degrees to see the sensitivity of the metamodel predictivity depending on the degree.

# %%
# The default value of this parameter is 10.

# %%
ot.ResourceMap.GetAsUnsignedInteger("FunctionalChaosAlgorithm-MaximumTotalDegree")

# %%
# This is why we explore the values from 5 to 15.

# %%
degrees = ot.Sample([[i] for i in range(5, 12)])
numberOfDegrees = degrees.getSize()
coefficientOfPredictivity = ot.Sample(numberOfDegrees,2)
for maximumDegree in range(5, 12):
    ot.ResourceMap.SetAsUnsignedInteger("FunctionalChaosAlgorithm-MaximumTotalDegree",maximumDegree)
    print("Maximum total degree =", maximumDegree)
    algo = ot.FunctionalChaosAlgorithm(inputSample, outputSample)
    algo.run()
    result = algo.getResult()
    metamodel = result.getMetaModel()
    for outputIndex in range(2):
        val = ot.MetaModelValidation(inputTest, outputTest[:,outputIndex], metamodel.getMarginal(outputIndex))
        Q2 = val.computePredictivityFactor()[0]
        coefficientOfPredictivity[maximumDegree-5,outputIndex] = Q2

# %%
graph = ot.Graph("Predictivity","Total degree","Q2",True)
cloud = ot.Cloud(degrees,coefficientOfPredictivity[:,0])
cloud.setLegend("Output #0")
cloud.setPointStyle("bullet")
graph.add(cloud)
cloud = ot.Cloud(degrees,coefficientOfPredictivity[:,1])
cloud.setLegend("Output #1")
cloud.setColor("red")
cloud.setPointStyle("bullet")
graph.add(cloud)
graph.setLegendPosition("topright")
view = viewer.View(graph)
plt.show()

# %%
# We see that a total degree lower than 9 is not sufficient to describe the first output with good predictivity. However, the coefficient of predictivity drops when the total degree gets greater than 12. 
# The predictivity of the second output seems to be much less satisfactory: a little more work would be required to improve the metamodel. 
#
# In this situation, the following methods may be used. 
#
# * Since the distribution of the input is known, we may want to give this information to the `FunctionalChaosAlgorithm`. This prevents the algorithm from trying to fit the distribution which best fit to the data. 
# * We may want to customize the `adaptiveStrategy` by selecting an enumerate function which best fit to this particular situation. In this specific example, the interactions plays a great role so that the linear enumerate function may provide better results than the hyperbolic rule.
# * We may want to customize the `projectionStrategy` by selecting an method to compute the coefficient which improves the estimation. Given that the function is symbolic and fast, it might be interesting to try an integration rule instead of the least squares method.
