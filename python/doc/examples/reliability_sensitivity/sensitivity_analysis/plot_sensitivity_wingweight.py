"""
Example of sensitivity analyses on the wing weight model
=========================================================
"""
import openturns as ot
import openturns.viewer as otv
from openturns.usecases.wingweight_function import WingWeightModel
from matplotlib import pylab as plt
import numpy as np
ot.Log.Show(ot.Log.NONE)

# %%
#
# This example is a brief overview of the use of main sensitivity analysis techniques and how to call them.
# In this example, the following sensitivity classes are used:
#
# - PCC: Partial Correlation Coefficients
# - PRCC: Partial Rank Correlation Coefficients 
# - SRC: Standard Regression Coefficients
# - SRRC: Standard Rank Regression Coefficients
# - Pearson coefficients
# - Spearman coefficients
# - Taylor expansion importance factors
# - Sobol' indices
# - HSIC : Hilbert-Schmidt Independence Criterion
#
# We present the methods on the :ref:`WingWeight function<use-case-wingweight>`.

# %%
# Definition of the model
# -----------------------
#
# We load the model from the usecases module.
# 
# For a quick reminder, this usecase is a ten dimensional model, with the following inputs:
#
# - :math:`S_w`: Wing area (ft^2), defined according to :math:`\mathcal{U}(150, 200)`
# - :math:`W_{fw}`: Weight of fuel in the wing (lb), defined according to :math:`\mathcal{U}(220, 300)`
# - :math:`A`: Aspect ratio (-), defined according to :math:`\mathcal{U}(6, 10)`
# - :math:`\Lambda`: Quarter chord sweep (deg), defined according to :math:`\mathcal{U}(-10, 10)`
# - :math:`q`: Dynamic pressure at cruise (lb/ft^2), defined according to :math:`\mathcal{U}(16, 45)`
# - :math:`l`: Taper ratio (-), defined according to :math:`\mathcal{U}(0.5, 1)`
# - :math:`t_c`: Airfoil thickness to chord ratio (-), defined according to :math:`\mathcal{U}(0.08, 0.18)`
# - :math:`N_z`: Ultimate load factor (-), defined according to :math:`\mathcal{U}(2.5, 6)`
# - :math:`W_{dg}`: Flight design gross weight (lb), defined according to :math:`\mathcal{U}(1700, 2500)`
# - :math:`W_p`: Paint weight (lb/ft^2), defined according to :math:`\mathcal{U}(0.025, 0.08)`
m = WingWeightModel()

# %%
# Cross cuts of the function
# --------------------------
#
# Let's have a look on 2D cross cuts of the wing weight function.
# For each 2D cross cut, the other variables are fixed to the input distribution mean values.
# This graph allows to have a first idea of the variations of the function in pair of dimensions.
# The colors of each contour plot are comparable. The number of contour levels are related to the amount of variation of the function in the corresponding coordinates.

fig = plt.figure(figsize=(12, 12))
lowerBound = m.distributionX.getRange().getLowerBound()
upperBound = m.distributionX.getRange().getUpperBound()

# Definition of number of meshes in x and y axes for the 2D cross cut plots
nX = 20
nY = 20
for i in range(m.dim):
    for j in range(i):
        crossCutIndices = []
        crossCutReferencePoint = []
        for k in range(m.dim):
            if k != i and k != j:
                crossCutIndices.append(k)
                # Definition of the reference point
                crossCutReferencePoint.append(m.distributionX.getMean()[k])
        # Definition of 2D cross cut function
        crossCutFunction = ot.ParametricFunction(
            m.model, crossCutIndices, crossCutReferencePoint)
        crossCutLowerBound = [lowerBound[j], lowerBound[i]]
        crossCutUpperBound = [upperBound[j], upperBound[i]]
        # Definition of the mesh
        inputData = ot.Box([nX, nY]).generate()
        inputData *= (ot.Point(crossCutUpperBound)-ot.Point(crossCutLowerBound))
        inputData += ot.Point(crossCutLowerBound)
        meshX = np.array(inputData)[:,0].reshape(nX+2,nY+2)
        meshY = np.array(inputData)[:,1].reshape(nX+2,nY+2)
        data = crossCutFunction(inputData)
        meshZ = np.array(data).reshape(nX+2,nY+2)
        levels = [(150 + 3*i) for i in range(101)]
        graph = ot.Graph()
        
        # Creation of the contour        
        index = 1 + i * m.dim + j

        ax = fig.add_subplot(m.dim, m.dim, index)
        ax.contour(meshX, meshY,meshZ,levels,cmap='hsv')
        ax.set_xticks([])
        ax.set_yticks([])        
        
        # Creation of axes title
        if j==0:
            ax.set_ylabel(m.distributionX.getDescription()[i])            
        if i ==9:
            ax.set_xlabel(m.distributionX.getDescription()[j])

# %%       
# We can see that the variables :math:`t_c, N_z, A, W_{dg}` seem to be influent on the wing weight whereas :math:`l, W_p, W_{fw}` do not have influence on the function.
            

# %%
# Data generation 
# ---------------
#
# We create the input and output data for the estimation of the different sensitivity coefficients and we get the input variables description:

inputNames = m.distributionX.getDescription()

size = 500
inputDesign = m.distributionX.getSample(size)
outputDesign = m.model(inputDesign)

# %%
# Let's estimate the PCC, PRCC, SRC, SRRC, Pearson and Spearman coefficients, display and analyze them.
# We create a `CorrelationAnalysis` model.

corr_analysis = ot.CorrelationAnalysis(inputDesign, outputDesign)

# %%
# PCC coefficients
# ----------------
# We compute here `PCC` coefficients using the `CorrelationAnalysis`.

# %%
pcc_indices = corr_analysis.computePCC()
print(pcc_indices)

# %%
#

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    pcc_indices, inputNames, "PCC coefficients - Wing weight")
view = otv.View(graph)

# %%
# PRCC coefficients
# -----------------
# We compute here `PRCC` coefficients using the `CorrelationAnalysis`.

# %%
prcc_indices = corr_analysis.computePRCC()
print(prcc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    prcc_indices, inputNames, "PRCC coefficients - Wing weight")
view = otv.View(graph)

# %%
# SRC coefficients
# -------------------
# We compute here `SRC` coefficients using the `CorrelationAnalysis`.

# %%
src_indices = corr_analysis.computeSRC()
print(src_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    src_indices, inputNames, 'SRC coefficients - Wing weight')
view = otv.View(graph)

# %%
# Normalized squared SRC coefficients (coefficients are made to sum to 1) :

# %%
squared_src_indices = corr_analysis.computeSquaredSRC(True)
print(squared_src_indices)

# %%
# And its associated graph:

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    squared_src_indices, inputNames, 'Squared SRC coefficients - Wing weight')
view = otv.View(graph)

# %%
#

# %%
# SRRC coefficients
# --------------------
# We compute here `SRRC` coefficients using the `CorrelationAnalysis`.

# %%
srrc_indices = corr_analysis.computeSRRC()
print(srrc_indices)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(
    srrc_indices, inputNames, 'SRRC coefficients - Wing weight')
view = otv.View(graph)

# %%
# Pearson coefficients
# -----------------------
# We compute here the Pearson :math:`\rho` coefficients using the `CorrelationAnalysis`.

# %%
pearson_correlation = corr_analysis.computePearsonCorrelation()
print(pearson_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(pearson_correlation,
                                                             inputNames,
                                                             "Pearson correlation coefficients - Wing weight")
view = otv.View(graph)

# %%
# Spearman coefficients
# -----------------------
# We compute here the Spearman :math:`\rho_s` coefficients using the `CorrelationAnalysis`.

# %%
spearman_correlation = corr_analysis.computeSpearmanCorrelation()
print(spearman_correlation)

# %%
graph = ot.SobolIndicesAlgorithm.DrawCorrelationCoefficients(spearman_correlation,
                                                             inputNames,
                                                             "Spearman correlation coefficients - Wing weight")
view = otv.View(graph)
plt.show()

# %%
#
# The different computed correlation estimators show that the variables :math:`S_w, A, N_z, t_c` seem to be the most correlated with the wing weight.
# Pearson and Spearman coefficients do not reveal any linear nor monotonic correlation as no coefficients are equal to +/- 1.
# Coefficients about :math:`t_c` are negative revealing a negative correlation with the wing weight, that is consistent with the model expression.


# %%
# Taylor expansion importance factors
# -----------------------------------
# We compute here the Taylor expansion importance factors using `TaylorExpansionMoments`.

# %%

# %%
# We create a distribution-based RandomVector.
X = ot.RandomVector(m.distributionX)

# %%
# We create a composite RandomVector Y from X and m.model.
Y = ot.CompositeRandomVector(m.model, X)

# %%
# We create a Taylor expansion method to approximate moments.
taylor = ot.TaylorExpansionMoments(Y)

# %%
# We get the importance factors.
print(taylor.getImportanceFactors())

# %%
# We draw the importance factors
graph = taylor.drawImportanceFactors()
graph.setTitle('Taylor expansion imporfance factors - Wing weight')
view = otv.View(graph)

# %%
#
# The Taylor expansion importance factors is consistent with the previous estimators as :math:`S_w, A, N_z, t_c` seem to be the most influent variables.
# To analyze the relevance of the previous indices, a Sobol' analysis is now carried out.

# %%
# Sobol' indices
# --------------
# We compute the Sobol' indices from both sampling approach and  Polynomial Chaos Expansion.

# %%
sizeSobol = 1000
sie = ot.SobolIndicesExperiment(m.distributionX, sizeSobol)
inputDesignSobol = sie.generate()
inputNames = m.distributionX.getDescription()
inputDesignSobol.setDescription(inputNames)
inputDesignSobol.getSize()

# %%
# We see that 12000 function evaluations are required to estimate the first order and total Sobol' indices.

# %%
# Then, we evaluate the outputs corresponding to this design of experiments.

# %%
outputDesignSobol = m.model(inputDesignSobol)

# %%
# We estimate the Sobol' indices with the `SaltelliSensitivityAlgorithm`.

# %%
sensitivityAnalysis = ot.SaltelliSensitivityAlgorithm(
    inputDesignSobol, outputDesignSobol, sizeSobol)

# %%
# The `getFirstOrderIndices` and `getTotalOrderIndices` methods respectively return estimates of all first order and total Sobol' indices.

# %%
print('First order indices:', sensitivityAnalysis.getFirstOrderIndices())

# %%
print('Total order indices:',sensitivityAnalysis.getTotalOrderIndices())


# %%
# The `draw` method produces the following graph. The vertical bars represent the 95% confidence intervals of the estimates.

# %%
graph = sensitivityAnalysis.draw()
graph.setTitle('Sobol indices with Saltelli - wing weight')
view = otv.View(graph)

# %%
# We see that several Sobol' indices are negative, that is inconsistent with the theory. Therefore, a larger number of samples is required to get consistent indices
sizeSobol = 10000
sie = ot.SobolIndicesExperiment(m.distributionX, sizeSobol)
inputDesignSobol = sie.generate()
inputNames = m.distributionX.getDescription()
inputDesignSobol.setDescription(inputNames)
inputDesignSobol.getSize()
outputDesignSobol = m.model(inputDesignSobol)

sensitivityAnalysis = ot.SaltelliSensitivityAlgorithm(
    inputDesignSobol, outputDesignSobol, sizeSobol)
    
sensitivityAnalysis.getFirstOrderIndices()
sensitivityAnalysis.getTotalOrderIndices()

graph = sensitivityAnalysis.draw()
graph.setTitle('Sobol indices with Saltelli - wing weight')
view = otv.View(graph)

# %%
# It improves the accuracy of the estimation but, for very low indices, Saltelli scheme is not accurate since several confidence intervals provide negative lower bounds.

# %%
# Now, we estimate the Sobol' indices using Polynomial Chaos Expansion.
# We first create a Functional Chaos Expansion.
sizePCE = 200
inputDesignPCE = m.distributionX.getSample(sizePCE)
outputDesignPCE = m.model(inputDesignPCE)

multivariateBasis = ot.OrthogonalProductPolynomialFactory(
    [m.distributionX.getMarginal(i) for i in range(m.dim)])
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(inputDesignPCE, outputDesignPCE, selectionAlgorithm)
totalDegree = 4
enumfunc = multivariateBasis.getEnumerateFunction()
P = enumfunc.getStrataCumulatedCardinal(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, P)
algo = ot.FunctionalChaosAlgorithm(
    inputDesignPCE, outputDesignPCE, m.distributionX, adaptiveStrategy, projectionStrategy)

algo.run()
result = algo.getResult()
print(result.getResiduals())
print(result.getRelativeErrors())

# %%
# The relative errors are very low that indicate the good accuracy of the PCE model.

# %%
# Then, we exploit the surrogate model to compute the Sobol' indices.
sensitivityAnalysis = ot.FunctionalChaosSobolIndices(result)
print(sensitivityAnalysis.summary())
firstOrder = [sensitivityAnalysis.getSobolIndex(i) for i in range(m.dim)]
totalOrder = [sensitivityAnalysis.getSobolTotalIndex(
    i) for i in range(m.dim)]
graph = ot.SobolIndicesAlgorithm.DrawSobolIndices(
    inputNames, firstOrder, totalOrder)
graph.setTitle('Sobol indices by Polynomial Chaos Expansion - wing weight')
view = otv.View(graph)

# %%
#
# The Sobol' indices confirm the previous analyses, in terms of ranking of the most influent variables. We also see that five variables have a quasi null total Sobol' indices, that indicates almost no influence on the wing weight.
# There is no discrepancy between first order and total Sobol' indices, that indicates no or very low interaction between the variables in the variance of the output.


# %%
# HSIC indices
# ------------

# %%
# We then estimate the HSIC indices using a data-driven approach by exploiting the design of experiments used to train the PCE model.
inputDesignHSIC = inputDesignPCE
outputDesignHSIC = outputDesignPCE

covarianceModelCollection = []

# %%
for i in range(m.dim):
    Xi = inputDesignHSIC.getMarginal(i)
    inputCovariance = ot.SquaredExponential(1)
    inputCovariance.setScale(Xi.computeStandardDeviation())
    covarianceModelCollection.append(inputCovariance)

# %%
# We define a covariance kernel associated to the output variable.
outputCovariance = ot.SquaredExponential(1)
outputCovariance.setScale(outputDesignHSIC.computeStandardDeviation())
covarianceModelCollection.append(outputCovariance)

#%%
#
# In this paragraph, we perform the analysis on the raw data: that is
# the global HSIC estimator.

estimatorType = ot.HSICUStat()

# %%
# We now build the HSIC estimator:
globHSIC = ot.HSICEstimatorGlobalSensitivity(
    covarianceModelCollection, inputDesignHSIC, outputDesignHSIC, estimatorType)

# %%
# We get the R2-HSIC indices:
R2HSICIndices = globHSIC.getR2HSICIndices()
print("\n Global HSIC analysis")
print("R2-HSIC Indices: ", R2HSICIndices)

# %%
# and the HSIC indices:
HSICIndices = globHSIC.getHSICIndices()
print("HSIC Indices: ", HSICIndices)

# %%
# The p-value by permutation.
pvperm = globHSIC.getPValuesPermutation()
print("p-value (permutation): ", pvperm)

# %%
# We have an asymptotic estimate of the value for this estimator.
pvas = globHSIC.getPValuesAsymptotic()
print("p-value (asymptotic): ", pvas)

# %%
# We vizualise the results.
graph1 = globHSIC.drawHSICIndices()
view1 = otv.View(graph1)

graph2 = globHSIC.drawPValuesAsymptotic()
view2 = otv.View(graph2)

graph3 = globHSIC.drawR2HSICIndices()
view3 = otv.View(graph3)

graph4 = globHSIC.drawPValuesPermutation()
view4 = otv.View(graph4)

# %%
#
# The HSIC indices go in the same way as the other estimators in terms the most influent variables. The variables :math:`W_{fw}, q, l, W_p` seem to be independent to the output as the corresponding p-values are high. We can also see that the asymptotic p-values and p-values estimated by permutation are quite similar.
