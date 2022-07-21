"""
Create a polynomial chaos for the Ishigami function: a quick start guide to polynomial chaos
============================================================================================
"""
# %%
#
# In this example, we create a polynomial chaos for the :ref:`Ishigami function<use-case-ishigami>`. We create a sparse polynomial with maximum total degree equal to 8.
#

# %%
# Define the model
# ----------------

# %%
from openturns.usecases import ishigami_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import numpy as np
ot.Log.Show(ot.Log.NONE)
ot.RandomGenerator.SetSeed(0)

# %%
# We load the Ishigami model :
im = ishigami_function.IshigamiModel()

# %%
# The `IshigamiModel` data class contains the input distribution :math:`X=(X_1, X_2, X_3)` in `im.distributionX` and the Ishigami function in `im.model`.
# We also have access to the input variable names with
input_names = im.distributionX.getDescription()


# %%
# Draw the function
# -----------------

# %%
# Create a training sample

# %%
N = 1000
inputSample = im.distributionX.getSample(N)
outputSample = im.model(inputSample)


# %%
def plotXvsY(sampleX, sampleY, figsize=(15, 3)):
    import pylab as pl
    import openturns.viewer
    dimX = sampleX.getDimension()
    inputdescr = sampleX.getDescription()
    fig = pl.figure(figsize=figsize)
    for i in range(dimX):
        ax = fig.add_subplot(1, dimX, i+1)
        graph = ot.Graph('', inputdescr[i], 'Y', True, '')
        cloud = ot.Cloud(sampleX[:, i], sampleY)
        graph.add(cloud)
        _ = ot.viewer.View(graph, figure=fig, axes=[ax])
    return None


plotXvsY(inputSample, outputSample)

# %%
graph = ot.HistogramFactory().build(outputSample).drawPDF()
view = viewer.View(graph)

# %%
# We see that the distribution of the output has two modes.

# %%
# Create the polynomial chaos model
# ---------------------------------

# %%
# Create a training sample

# %%
N = 100
inputTrain = im.distributionX.getSample(N)
outputTrain = im.model(inputTrain)

# %%
# Create the chaos.
#
# We could use only the input and output training samples: in this case, the distribution of the input sample is computed by selecting the distribution that has the best fit.

# %%
chaosalgo = ot.FunctionalChaosAlgorithm(inputTrain, outputTrain)

# %%
# Since the input distribution is known in our particular case, we instead create the multivariate basis from the distribution.

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory(
    [im.X1, im.X2, im.X3])
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(
    inputTrain, outputTrain, selectionAlgorithm)
totalDegree = 8
enumfunc = multivariateBasis.getEnumerateFunction()
P = enumfunc.getStrataCumulatedCardinal(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, P)
chaosalgo = ot.FunctionalChaosAlgorithm(
    inputTrain, outputTrain, im.distributionX, adaptiveStrategy, projectionStrategy)

# %%
chaosalgo.run()
result = chaosalgo.getResult()
metamodel = result.getMetaModel()

# %%
# In order to validate the metamodel, we generate a test sample.

# %%
n_valid = 1000
inputTest = im.distributionX.getSample(n_valid)
outputTest = im.model(inputTest)
val = ot.MetaModelValidation(inputTest, outputTest, metamodel)
Q2 = val.computePredictivityFactor()[0]
Q2

# %%
# The Q2 is very close to 1: the metamodel is excellent.

# %%
graph = val.drawValidation()
graph.setTitle("Q2=%.2f%%" % (Q2*100))
view = viewer.View(graph)

# %%
# The metamodel has a good predictivity, since the points are almost on the first diagonal.

# %%
# Compute and print Sobol' indices
# --------------------------------

# %%
chaosSI = ot.FunctionalChaosSobolIndices(result)
print(chaosSI.summary())

# %%
# We notice the a coefficient with marginal degree equal to 6 has a significant impact on the output variance. Hence, we cannot get a satisfactory polynomial chaos with total degree less that 6.

# %%
# Draw Sobol' indices

# %%
dim_input = im.distributionX.getDimension()
first_order = [chaosSI.getSobolIndex(i) for i in range(dim_input)]
total_order = [chaosSI.getSobolTotalIndex(i) for i in range(dim_input)]
input_names = im.model.getInputDescription()
graph = ot.SobolIndicesAlgorithm.DrawSobolIndices(
    input_names, first_order, total_order)
view = viewer.View(graph)


# %%
# The variable which has the largest impact on the output is, taking interactions into account, X1.
#
# We see that X1 has interactions with other variables, since the first order indice is less than the total order indice.
#
# At first order, X3 has no interactions with other variables since its first order indice is close to zero.

# %%
# Computing the accuracy
# ----------------------

# %%
# The interesting point with the Ishigami function is that the exact Sobol' indices are known. We can use that property in order to compute the absolute error on the Sobol' indices for the polynomial chaos.
#
# The following function computes the exact mean, variance and Sobol' indices for this function.

# %%
def ishigamiSA(a, b):
    '''Exact sensitivity indices of the Ishigami function for given a and b.'''
    var = 1.0/2 + a**2/8 + b*np.pi**4/5 + b**2*np.pi**8/18
    S1 = (1.0/2 + b*np.pi**4/5+b**2*np.pi**8/50)/var
    S2 = (a**2/8)/var
    S3 = 0
    S13 = b**2*np.pi**8/2*(1.0/9-1.0/25)/var
    exact = {
        'expectation': a/2,
        'variance': var,
        'S1': (1.0/2 + b*np.pi**4/5+b**2*np.pi**8.0/50)/var,
        'S2': (a**2/8)/var,
        'S3': 0,
        'S12': 0,
        'S23': 0,
        'S13': S13,
        'S123': 0,
        'ST1': S1 + S13,
        'ST2': S2,
        'ST3': S3 + S13
    }
    return exact


# %%
a = 7.
b = 0.1
exact = ishigamiSA(a, b)
exact

# %%
# To make the comparisons simpler, we gather the results into a list.

# %%
S_exact = [exact["S1"], exact["S2"], exact["S3"]]
ST_exact = [exact["ST1"], exact["ST2"], exact["ST3"]]

# %%
# Then we perform a loop over the input dimension and compute the absolute error on the Sobol' indices.

# %%
for i in range(im.dim):
    absoluteErrorS = abs(first_order[i]-S_exact[i])
    absoluteErrorST = abs(total_order[i]-ST_exact[i])
    print("X%d, Abs.Err. on S=%.1e, Abs.Err. on ST=%.1e" %
          (i+1, absoluteErrorS, absoluteErrorST))

plt.show()
# %%
# We see that the indices are correctly estimated with a low accuracy even if we have use only 100 function evaluations. This shows the good performance of the polynomial chaos in this case.
