"""
Calibration of the flooding model-v2
====================================
"""
# %% 
#
# Model
# -----
#
# The simulator predicts the water height :math:`H` depending on the flowrate :math:`Q`.
#
# We consider the following four variable:
#
# * :math:`Q` : the river flowrate (:math:`m^3/s`)
# * :math:`Ks` : the Strickler coefficient (:math:`m^{1/3}/s`)
# * :math:`Z_v` : the downstream riverbed level (m)
# * :math:`Z_m` : the upstream riverbed level (m)
#
# When the Strickler coefficient increases, the riverbed generates less friction to the water flow.
#
# Parameters
# ----------
#
# We consider the following parameters:
#
# * the length of the river :math:`L` = 5000 (m),
# * the width of the river :math:`B` = 300 (m).
#
# Outputs
# -------
#
# We make the hypothesis that the slope of the river is nonpositive and close to zero, which implies: 
#
# .. math::
#    \alpha = \frac{Z_m - Z_v}{L},
# 
#
# if :math:`Z_m \geq Z_v`. 
# The height of the river is:
#
# .. math::
#    H = \left(\frac{Q}{K_s B \sqrt{\alpha}}\right)^{0.6},
# 
#
# for any :math:`K_s, Q>0`.
#
# <img src="river_section_adjusted_light.png" width="400" />
#
# Distribution
# ------------
#
# We assume that the river flowrate has the following truncated Gumbel distribution:
#
# ======== ===============================
# Variable Distribution
# ======== ===============================
# Q        Gumbel(scale=558, mode=1013)>0
# ======== ===============================
#
# Parameters to calibrate
# -----------------------
#
# The vector of parameters to calibrate is: 
#
# .. math::
#    \theta = (K_s,Z_v,Z_m).
# 
#
# The variables to calibrate are :math:`(K_s,Z_v,Z_m)` and are set to the following values:
#
# .. math::
#    K_s = 30, \qquad Z_v = 50, \qquad Z_m = 55.
# 
#
# Observations
# ------------
#
# In this section, we describe the statistical model associated with the :math:`n` observations.
# The errors of the water heights are associated with a gaussian distribution with a zero mean and a standard variation equal to:
# 
# .. math::
#    \sigma=0.1.
# 
#
# Therefore, the observed water heights are:
# 
# .. math::
#    H_i = G(Q_i,K_s,Z_v,Z_m) + \epsilon_i
# 
#
# for :math:`i=1,...,n` where
# 
# .. math::
#    \epsilon \sim \mathcal{N}(0,\sigma^2)
# 
#
# and we make the hypothesis that the observation errors are independent.
# We consider a sample size equal to:
# 
# .. math::
#    n=100.
# 
#
# The observations are the couples :math:`\{(Q_i,H_i)\}_{i=1,...,n}`, i.e. each observation is a couple made of the flowrate and the corresponding river height.
#
# Analysis
# --------
#
# In this model, the variables :math:`Z_m` and :math:`Z_v` are not identifiables, since only the difference :math:`Z_m-Z_v` matters. Hence, calibrating this model requires some regularization.

# %%
# Generate the observations
# -------------------------

# %%
import numpy as np
import openturns as ot
ot.ResourceMap.SetAsUnsignedInteger('Normal-SmallDimension', 1)
import openturns.viewer as viewer
from matplotlib import pylab as plt

import sys
sys.path.append('./use_cases')
from use_cases import use_case_flood_model
fm = use_case_flood_model.flood_model()

# %%
# We define the model :math:`g` which has 4 inputs and one output H.
#
# The nonlinear least squares does not take into account for bounds in the parameters. Therefore, we ensure that the output is computed whatever the inputs. The model fails into two situations:
#
# * if :math:`K_s<0`,
# * if :math:`Z_v-Z_m<0`.
#
# In these cases, we return an infinite number, so that the optimization algorithm does not get trapped.

# %%
def functionFlooding(X) :
    L = 5.0e3
    B = 300.0
    Q, K_s, Z_v, Z_m = X
    alpha = (Z_m - Z_v)/L
    if alpha < 0.0 or K_s <= 0.0:
        H = np.inf
    else:
        H = (Q/(K_s*B*np.sqrt(alpha)))**(3.0/5.0)
    return [H]


# %%
g = ot.PythonFunction(4, 1, functionFlooding) 
g = ot.MemoizeFunction(g)
g.setOutputDescription(["H (m)"])

# %%
# Create the input distribution for :math:`Q`.

# %%
Q = ot.Gumbel(558.0, 1013.0)
Q = ot.TruncatedDistribution(Q,ot.TruncatedDistribution.LOWER)
Q.setDescription(["Q (m3/s)"])
Q

# %%
# Set the parameters to be calibrated.

# %%
K_s = ot.Dirac(30.0)
Z_v = ot.Dirac(50.0)
Z_m = ot.Dirac(55.0)
K_s.setDescription(["Ks (m^(1/3)/s)"])
Z_v.setDescription(["Zv (m)"])
Z_m.setDescription(["Zm (m)"])

# %%
# Create the joint input distribution.

# %%
inputRandomVector = ot.ComposedDistribution([Q, K_s, Z_v, Z_m])

# %%
# Create a Monte-Carlo sample of the output H.

# %%
nbobs = 100
inputSample = inputRandomVector.getSample(nbobs)
outputH = g(inputSample)

# %%
# Observe the distribution of the output H.

# %%
graph = ot.HistogramFactory().build(outputH).drawPDF()
view = viewer.View(graph)

# %%
# Generate the observation noise and add it to the output of the model.

# %%
sigmaObservationNoiseH = 0.1 # (m)
noiseH = ot.Normal(0.,sigmaObservationNoiseH)
sampleNoiseH = noiseH.getSample(nbobs)
Hobs = outputH + sampleNoiseH

# %%
# Plot the Y observations versus the X observations.

# %%
Qobs = inputSample[:,0]

# %%
graph = ot.Graph("Observations","Q (m3/s)","H (m)",True)
cloud = ot.Cloud(Qobs,Hobs)
graph.add(cloud)
view = viewer.View(graph)
#graph

# %%
# A visualisation class
# ---------------------

# %%
"""
A graphics class to analyze the results of calibration.
"""

import pylab as pl
import openturns as ot
import openturns.viewer as otv

class CalibrationAnalysis:
    def __init__(self,calibrationResult,model,inputObservations,outputObservations):
        """
        Plots the prior and posterior distribution of the calibrated parameter theta.
    
        Parameters
        ----------
        calibrationResult : :class:`~openturns.calibrationResult`
            The result of a calibration.
    
        model : 2-d sequence of float
            The function to calibrate.
    
        inputObservations : :class:`~openturns.Sample`
            The sample of input values of the model linked to the observations.
    
        outputObservations : 2-d sequence of float
            The sample of output values of the model linked to the observations.
        """
        self.model = model
        self.outputAtPrior = None
        self.outputAtPosterior = None
        self.calibrationResult = calibrationResult
        self.observationColor = "blue"
        self.priorColor = "red"
        self.posteriorColor = "green"
        self.inputObservations = inputObservations
        self.outputObservations = outputObservations
        self.unitlength = 6 # inch
        # Compute yAtPrior
        meanPrior = calibrationResult.getParameterPrior().getMean()
        model.setParameter(meanPrior)
        self.outputAtPrior=model(inputObservations)
        # Compute yAtPosterior
        meanPosterior = calibrationResult.getParameterPosterior().getMean()
        model.setParameter(meanPosterior)
        self.outputAtPosterior=model(inputObservations)
        return None

    def drawParameterDistributions(self):
        """
        Plots the prior and posterior distribution of the calibrated parameter theta.
        """
        thetaPrior = self.calibrationResult.getParameterPrior()
        thetaDescription = thetaPrior.getDescription()
        thetaPosterior = self.calibrationResult.getParameterPosterior()
        thetaDim = thetaPosterior.getDimension()
        fig = pl.figure(figsize=(12, 4))
        for i in range(thetaDim):
            graph = ot.Graph("",thetaDescription[i],"PDF",True,"topright")
            # Prior distribution
            thetaPrior_i = thetaPrior.getMarginal(i)
            priorPDF = thetaPrior_i.drawPDF()
            priorPDF.setColors([self.priorColor])
            priorPDF.setLegends(["Prior"])
            graph.add(priorPDF)
            # Posterior distribution
            thetaPosterior_i = thetaPosterior.getMarginal(i)
            postPDF = thetaPosterior_i.drawPDF()
            postPDF.setColors([self.posteriorColor])
            postPDF.setLegends(["Posterior"])
            graph.add(postPDF)
            '''
            If the prior is a Dirac, set the vertical axis bounds to the posterior. 
            Otherwise, the Dirac set to [0,1], where the 1 can be much larger 
            than the maximum PDF of the posterior.
            '''
            if (thetaPrior_i.getName()=="Dirac"):
                # The vertical (PDF) bounds of the posterior
                postbb = postPDF.getBoundingBox()
                pdf_upper = postbb.getUpperBound()[1]
                pdf_lower = postbb.getLowerBound()[1]
                # Set these bounds to the graph
                bb = graph.getBoundingBox()
                graph_upper = bb.getUpperBound()
                graph_upper[1] = pdf_upper
                bb.setUpperBound(graph_upper)
                graph_lower = bb.getLowerBound()
                graph_lower[1] = pdf_lower
                bb.setLowerBound(graph_lower)
                graph.setBoundingBox(bb)
            # Add it to the graphics
            ax = fig.add_subplot(1, thetaDim, i+1)
            _ = otv.View(graph, figure=fig, axes=[ax])
        return fig
    
    def drawObservationsVsPredictions(self):
        """
        Plots the output of the model depending 
        on the output observations before and after calibration.
        """
        
        ySize = self.outputObservations.getSize()
        yDim = self.outputObservations.getDimension()
        graph = ot.Graph("","Observations","Predictions",True,"topleft")
        # Plot the diagonal
        if (yDim==1):
            graph = self._drawObservationsVsPredictions1Dimension(self.outputObservations,self.outputAtPrior,self.outputAtPosterior)
        elif (ySize==1):
            outputObservations = ot.Sample(self.outputObservations[0],1)
            outputAtPrior = ot.Sample(self.outputAtPrior[0],1)
            outputAtPosterior = ot.Sample(self.outputAtPosterior[0],1)
            graph = self._drawObservationsVsPredictions1Dimension(outputObservations,outputAtPrior,outputAtPosterior)
        else:
            fig = pl.figure(figsize=(self.unitlength*yDim, self.unitlength))
            for i in range(yDim):
                outputObservations = self.outputObservations[:,i]
                outputAtPrior = self.outputAtPrior[:,i]
                outputAtPosterior = self.outputAtPosterior[:,i]
                graph = self._drawObservationsVsPredictions1Dimension(outputObservations,outputAtPrior,outputAtPosterior)
                ax = fig.add_subplot(1, yDim, i+1)
                _ = otv.View(graph, figure=fig, axes=[ax])

        return graph

    def _drawObservationsVsPredictions1Dimension(self,outputObservations,outputAtPrior,outputAtPosterior):
        """
        Plots the output of the model depending 
        on the output observations before and after calibration.
        Can manage only 1D samples.
        """
        yDim = outputObservations.getDimension()
        ydescription = outputObservations.getDescription()
        xlabel = "%s Observations" % (ydescription[0])
        ylabel = "%s Predictions" % (ydescription[0])
        graph = ot.Graph("",xlabel,ylabel,True,"topleft")
        # Plot the diagonal
        if (yDim==1):
            curve = ot.Curve(outputObservations, outputObservations)
            curve.setColor(self.observationColor)
            graph.add(curve)
        else:
            raise TypeError('Output observations are not 1D.')
        # Plot the predictions before
        yPriorDim = outputAtPrior.getDimension()
        if (yPriorDim==1):
            cloud = ot.Cloud(outputObservations, outputAtPrior)
            cloud.setColor(self.priorColor)
            cloud.setLegend("Prior")
            graph.add(cloud)
        else:
            raise TypeError('Output prior predictions are not 1D.')
        # Plot the predictions after
        yPosteriorDim = outputAtPosterior.getDimension()
        if (yPosteriorDim==1):
            cloud = ot.Cloud(outputObservations, outputAtPosterior)
            cloud.setColor(self.posteriorColor)
            cloud.setLegend("Posterior")
            graph.add(cloud)
        else:
            raise TypeError('Output posterior predictions are not 1D.')
        return graph
    
    def drawResiduals(self):
        """
        Plot the distribution of the residuals and 
        the distribution of the observation errors. 
        """    
        ySize = self.outputObservations.getSize()
        yDim = self.outputObservations.getDimension()
        yPriorSize = self.outputAtPrior.getSize()
        yPriorDim = self.outputAtPrior.getDimension()
        if (yDim==1):
            observationsError = self.calibrationResult.getObservationsError()
            graph = self._drawResiduals1Dimension(self.outputObservations,self.outputAtPrior,self.outputAtPosterior,observationsError)
        elif (ySize==1):
            outputObservations = ot.Sample(self.outputObservations[0],1)
            outputAtPrior = ot.Sample(self.outputAtPrior[0],1)
            outputAtPosterior = ot.Sample(self.outputAtPosterior[0],1)
            observationsError = self.calibrationResult.getObservationsError()
            # In this case, we cannot draw observationsError ; just 
            # pass the required input argument, but it is not actually used.
            graph = self._drawResiduals1Dimension(outputObservations,outputAtPrior,outputAtPosterior,observationsError)
        else:
            observationsError = self.calibrationResult.getObservationsError()
            fig = pl.figure(figsize=(self.unitlength*yDim, self.unitlength))
            for i in range(yDim):
                outputObservations = self.outputObservations[:,i]
                outputAtPrior = self.outputAtPrior[:,i]
                outputAtPosterior = self.outputAtPosterior[:,i]
                observationsErrorYi = observationsError.getMarginal(i)
                graph = self._drawResiduals1Dimension(outputObservations,outputAtPrior,outputAtPosterior,observationsErrorYi)
                ax = fig.add_subplot(1, yDim, i+1)
                _ = otv.View(graph, figure=fig, axes=[ax])
        return graph

    def _drawResiduals1Dimension(self,outputObservations,outputAtPrior,outputAtPosterior,observationsError):
        """
        Plot the distribution of the residuals and 
        the distribution of the observation errors. 
        Can manage only 1D samples.
        """    
        ydescription = outputObservations.getDescription()
        xlabel = "%s Residuals" % (ydescription[0])
        graph = ot.Graph("Residuals analysis",xlabel,"Probability distribution function",True,"topright")
        yDim = outputObservations.getDimension()
        yPriorDim = outputAtPrior.getDimension()
        yPosteriorDim = outputAtPrior.getDimension()
        if (yDim==1) and (yPriorDim==1) :
            posteriorResiduals = outputObservations - outputAtPrior
            kernel = ot.KernelSmoothing()
            fittedDist = kernel.build(posteriorResiduals)
            residualPDF = fittedDist.drawPDF()
            residualPDF.setColors([self.priorColor])
            residualPDF.setLegends(["Prior"])
            graph.add(residualPDF)
        else:
            raise TypeError('Output prior observations are not 1D.')
        if (yDim==1) and (yPosteriorDim==1) :
            posteriorResiduals = outputObservations - outputAtPosterior
            kernel = ot.KernelSmoothing()
            fittedDist = kernel.build(posteriorResiduals)
            residualPDF = fittedDist.drawPDF()
            residualPDF.setColors([self.posteriorColor])
            residualPDF.setLegends(["Posterior"])
            graph.add(residualPDF)
        else:
            raise TypeError('Output posterior observations are not 1D.')
        # Plot the distribution of the observation errors
        if (observationsError.getDimension()==1):
            # In the other case, we just do not plot
            obserrgraph = observationsError.drawPDF()
            obserrgraph.setColors([self.observationColor])
            obserrgraph.setLegends(["Observation errors"])
            graph.add(obserrgraph)
        return graph

    def drawObservationsVsInputs(self):
        """
        Plots the observed output of the model depending 
        on the observed input before and after calibration.
        """
        xSize = self.inputObservations.getSize()
        ySize = self.outputObservations.getSize()
        xDim = self.inputObservations.getDimension()
        yDim = self.outputObservations.getDimension()
        xdescription = self.inputObservations.getDescription()
        ydescription = self.outputObservations.getDescription()
        # Observations
        if (xDim==1) and (yDim==1):
            graph = self._drawObservationsVsInputs1Dimension(self.inputObservations,self.outputObservations,self.outputAtPrior,self.outputAtPosterior)
        elif (xSize==1) and (ySize==1):
            inputObservations = ot.Sample(self.inputObservations[0],1)
            outputObservations = ot.Sample(self.outputObservations[0],1)
            outputAtPrior = ot.Sample(self.outputAtPrior[0],1)
            outputAtPosterior = ot.Sample(self.outputAtPosterior[0],1)
            graph = self._drawObservationsVsInputs1Dimension(inputObservations,outputObservations,outputAtPrior,outputAtPosterior)
        else:
            fig = pl.figure(figsize=(xDim*self.unitlength, yDim*self.unitlength))
            for i in range(xDim):
                for j in range(yDim):
                    k = xDim * j + i
                    inputObservations = self.inputObservations[:,i]
                    outputObservations = self.outputObservations[:,j]
                    outputAtPrior = self.outputAtPrior[:,j]
                    outputAtPosterior = self.outputAtPosterior[:,j]
                    graph = self._drawObservationsVsInputs1Dimension(inputObservations,outputObservations,outputAtPrior,outputAtPosterior)
                    ax = fig.add_subplot(yDim, xDim, k+1)
                    _ = otv.View(graph, figure=fig, axes=[ax])
        return graph

    def _drawObservationsVsInputs1Dimension(self,inputObservations,outputObservations,outputAtPrior,outputAtPosterior):
        """
        Plots the observed output of the model depending 
        on the observed input before and after calibration.
        Can manage only 1D samples.
        """
        xDim = inputObservations.getDimension()
        if (xDim!=1):
            raise TypeError('Input observations are not 1D.')
        yDim = outputObservations.getDimension()
        xdescription = inputObservations.getDescription()
        ydescription = outputObservations.getDescription()
        graph = ot.Graph("",xdescription[0],ydescription[0],True,"topright")
        # Observations
        if (yDim==1):
            cloud = ot.Cloud(inputObservations,outputObservations)
            cloud.setColor(self.observationColor)
            cloud.setLegend("Observations")
            graph.add(cloud)
        else:
            raise TypeError('Output observations are not 1D.')
        # Model outputs before calibration
        yPriorDim = outputAtPrior.getDimension()
        if (yPriorDim==1):
            cloud = ot.Cloud(inputObservations,outputAtPrior)
            cloud.setColor(self.priorColor)
            cloud.setLegend("Prior")
            graph.add(cloud)
        else:
            raise TypeError('Output prior predictions are not 1D.')
        # Model outputs after calibration
        yPosteriorDim = outputAtPosterior.getDimension()
        if (yPosteriorDim==1):
            cloud = ot.Cloud(inputObservations,outputAtPosterior)
            cloud.setColor(self.posteriorColor)
            cloud.setLegend("Posterior")
            graph.add(cloud)
        else:
            raise TypeError('Output posterior predictions are not 1D.')
        return graph


# %%
# Setting the calibration parameters
# ----------------------------------

# %%
# Define the value of the reference values of the :math:`\theta` parameter. In the bayesian framework, this is called the mean of the *prior* gaussian distribution. In the data assimilation framework, this is called the *background*.

# %%
KsInitial = 20.
ZvInitial = 49.
ZmInitial = 51.
thetaPrior = ot.Point([KsInitial,ZvInitial,ZmInitial])

# %%
# The following statement create the calibrated function from the model. The calibrated parameters Ks, Zv, Zm are at indices 1, 2, 3 in the inputs arguments of the model.

# %%
calibratedIndices = [1,2,3]
mycf = ot.ParametricFunction(g, calibratedIndices, thetaPrior)

# %%
# Calibration with linear least squares
# -------------------------------------

# %%
# The `LinearLeastSquaresCalibration` class performs the linear least squares calibration by linearizing the model in the neighbourhood of the reference point.

# %%
algo = ot.LinearLeastSquaresCalibration(mycf, Qobs, Hobs, thetaPrior, "SVD")

# %%
# The `run` method computes the solution of the problem.

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
# The `getParameterMAP` method returns the maximum of the posterior distribution of :math:`\theta`.

# %%
thetaStar = calibrationResult.getParameterMAP()
thetaStar

# %%
# In this case, we see that there seems to be a great distance from the reference value of :math:`\theta` to the optimum: the values seem too large in magnitude. The value of the optimum :math:`K_s` is nonpositive. In fact, there is an identification problem because the Jacobian matrix is rank-degenerate.

# %%
# Diagnostic of the identification issue
# --------------------------------------

# %%
# In this section, we show how to diagnose the identification problem.
#
# The `getParameterPosterior` method returns the posterior gaussian distribution of :math:`\theta`.

# %%
distributionPosterior = calibrationResult.getParameterPosterior()
distributionPosterior

# %%
# We see that there is a large covariance matrix diagonal. 
#
# Let us compute a 95% confidence interval for the solution :math:`\theta^\star`.

# %%
distributionPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# The confidence interval is *very* large.

# %%
mycf.setParameter(thetaPrior)
thetaDim = thetaPrior.getDimension()
jacobianMatrix = ot.Matrix(nbobs,thetaDim)
for i in range(nbobs):
    jacobianMatrix[i,:] = mycf.parameterGradient(Qobs[i]).transpose()
jacobianMatrix[0:5,:]

# %%
jacobianMatrix.computeSingularValues()

# %%
# We can see that there are two singular values which are relatively close to zero. 
#
# This explains why the Jacobian matrix is close to being rank-degenerate.

# %%
# Conclusion of the linear least squares calibration
# --------------------------------------------------

# %%
# There are several methods to solve the problem.
#
# * Given that the problem is not identifiable, we can use some regularization method. Two methods are provided in the library: the gaussian linear least squares `GaussianLinearCalibration` and the gaussian non linear least squares `GaussianNonlinearCalibration`.
# * We can change the problem, replacing it with a problem which is identifiable. In the flooding model, replacing :math:`Z_v-Z_m` with :math:`\Delta Z` allows to solve the issue.

# %%
# Calibration with non linear least squares
# -----------------------------------------

# %%
# The `NonLinearLeastSquaresCalibration` class performs the non linear least squares calibration by minimizing the squared euclidian norm between the predictions and the observations.

# %%
algo = ot.NonLinearLeastSquaresCalibration(mycf, Qobs, Hobs, thetaPrior)

# %%
# The `run` method computes the solution of the problem.

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
# Analysis of the results
# -----------------------

# %%
# The `getParameterMAP` method returns the maximum of the posterior distribution of :math:`\theta`.

# %%
thetaMAP = calibrationResult.getParameterMAP()
thetaMAP

# %%
# We can compute a 95% confidence interval of the parameter :math:`\theta^\star`. 
#
# This confidence interval is based on bootstrap, based on a sample size equal to 100 (as long as the value of the `ResourceMap` key "NonLinearLeastSquaresCalibration-BootstrapSize" is unchanged). This confidence interval reflects the sensitivity of the optimum to the variability in the observations.

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
thetaPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# In this case, the value of the parameter :math:`K_s` is quite accurately computed, but there is a relatively large uncertainty on the values of :math:`Z_v` and :math:`Z_m`.

# %%
mypcr = CalibrationAnalysis(calibrationResult,mycf, Qobs,Hobs)

# %%
graph = mypcr.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)
#graph

# %%
# We see that there is a good fit after calibration, since the predictions after calibration (i.e. the green crosses) are close to the observations (i.e. the blue crosses).

# %%
graph = mypcr.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# We see that there is a much better fit after calibration, since the predictions are close to the diagonal of the graphics.

# %%
observationError = calibrationResult.getObservationsError()
observationError

# %%
# We can see that the observation error is close to have a zero mean and a standard deviation equal to 0.1.

# %%
graph = mypcr.drawResiduals()
graph.setLegendPosition("topleft")
view = viewer.View(graph)
#graph

# %%
# The analysis of the residuals shows that the distribution is centered on zero and symmetric. This indicates that the calibration performed well. 
#
# Moreover, the distribution of the residuals is close to being gaussian.

# %%
graph = mypcr.drawParameterDistributions()
print(graph)
#view = viewer.View(graph)

# %%
# Gaussian linear calibration
# ---------------------------

# %%
# The standard deviation of the observations.

# %%
sigmaH = 0.5 # (m^2)

# %%
# Define the covariance matrix of the output Y of the model.

# %%
errorCovariance = ot.CovarianceMatrix(1)
errorCovariance[0,0] = sigmaH**2

# %%
# Define the covariance matrix of the parameters :math:`\theta` to calibrate.

# %%
sigmaKs = 5.
sigmaZv = 1.
sigmaZm = 1.

# %%
sigma = ot.CovarianceMatrix(3)
sigma[0,0] = sigmaKs**2
sigma[1,1] = sigmaZv**2
sigma[2,2] = sigmaZm**2
sigma

# %%
# The `GaussianLinearCalibration` class performs the gaussian linear calibration by linearizing the model in the neighbourhood of the prior.

# %%
algo = ot.GaussianLinearCalibration(mycf, Qobs, Hobs, thetaPrior, sigma, errorCovariance,"SVD")

# %%
# The `run` method computes the solution of the problem.

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
# Analysis of the results
# -----------------------

# %%
# The `getParameterMAP` method returns the maximum of the posterior distribution of :math:`\theta`.

# %%
thetaStar = calibrationResult.getParameterMAP()
thetaStar

# %%
mypcr = CalibrationAnalysis(calibrationResult,mycf, Qobs,Hobs)

# %%
graph = mypcr.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)
#graph

# %%
# We see that the output of the model after calibration is closer to the observations. However, there is still a distance from the outputs of the model to the observations. This indicates that the calibration cannot be performed with this method.

# %%
graph = mypcr.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# In this case, the fit is better after calibration, but not perfect. Indeed, the cloud of points after calibration is not centered on the diagonal. 

# %%
graph = mypcr.drawResiduals()
graph.setLegendPosition("topleft")
view = viewer.View(graph)
#graph

# %%
# We see that the distribution of the residual is not centered on zero: the mean residual is approximately -0.5, which implies that the predictions are, on average, smaller than the observations. This is a proof that the calibration cannot be performed with this method in this particular case.

# %%
# The `getParameterPosterior` method returns the posterior gaussian distribution of :math:`\theta`.

# %%
distributionPosterior = calibrationResult.getParameterPosterior()
distributionPosterior

# %%
# We can compute a 95% confidence interval of the parameter :math:`\theta^\star`.

# %%
distributionPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# We see that there is a large uncertainty on the value of the parameter :math:`K_s` which can be as small as 14 and as large as 34. 

# %%
# We can compare the prior and posterior distributions of the marginals of :math:`\theta`. 

# %%
graph = mypcr.drawParameterDistributions()
#view = viewer.View(graph)

# %%
# The two distributions are different, which shows that the calibration is sensible to the observations (if the observations were not sensible, the two distributions were superimposed). Moreover, the two distributions are quite close, which implies that the prior distribution has played a roled in the calibration (otherwise the two distributions would be completely different, indicating that only the observations were taken into account). 

# %%
# Gaussian nonlinear calibration
# ------------------------------

# %%
# The `GaussianNonLinearCalibration` class performs the gaussian nonlinear calibration.

# %%
algo = ot.GaussianNonLinearCalibration(mycf, Qobs, Hobs, thetaPrior, sigma, errorCovariance)

# %%
# The `run` method computes the solution of the problem.

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
# Analysis of the results
# -----------------------

# %%
# The `getParameterMAP` method returns the maximum of the posterior distribution of :math:`\theta`.

# %%
thetaStar = calibrationResult.getParameterMAP()
thetaStar

# %%
mypcr = CalibrationAnalysis(calibrationResult,mycf, Qobs,Hobs)

# %%
graph = mypcr.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)
#graph

# %%
# We see that the output of the model after calibration is in the middle of the observations: the calibration seems correct.

# %%
graph = mypcr.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# The fit is excellent after calibration. Indeed, the cloud of points after calibration is on the diagonal. 

# %%
graph = mypcr.drawResiduals()
view = viewer.View(graph)

# %%
# We see that the histogram of the residual is centered on zero. This is a proof that the calibration did perform correctly.

# %%
# The `getParameterPosterior` method returns the posterior gaussian distribution of :math:`\theta`.

# %%
distributionPosterior = calibrationResult.getParameterPosterior()

# %%
# We can compute a 95% confidence interval of the parameter :math:`\theta^\star`.

# %%
distributionPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# We see that there is a small uncertainty on the value of all parameters.

# %%
# We can compare the prior and posterior distributions of the marginals of :math:`\theta`. 

# %%
graph = mypcr.drawParameterDistributions()
#view = viewer.View(graph)

# %%
# The two distributions are very different, with a spiky posterior distribution. This shows that the calibration is very sensible to the observations. 

# %%
# Tuning the posterior distribution estimation
# --------------------------------------------
#
# The "GaussianNonLinearCalibration-BootstrapSize" key controls the posterior distribution estimation.
#
# * If "GaussianNonLinearCalibration-BootstrapSize" > 0 (by default it is equal to 100), then a bootstrap resample algorithm is used to see the dispersion of the MAP estimator. This allows to see the variability of the estimator with respect to the finite observation sample.
# * If "GaussianNonLinearCalibration-BootstrapSize" is zero, then the gaussian linear calibration estimator is used (i.e. the `GaussianLinearCalibration` class) at the optimum. This is called the Laplace approximation. 
#
# We must configure the key before creating the object (otherwise changing the parameter does not change the result). 

# %%
ot.ResourceMap_SetAsUnsignedInteger("GaussianNonLinearCalibration-BootstrapSize",0) 

# %%
algo = ot.GaussianNonLinearCalibration(mycf, Qobs, Hobs, thetaPrior, sigma, errorCovariance)

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
graph = mypcr.drawParameterDistributions()
#view = viewer.View(graph)

if not viewer._noshow:
    plt.show()
# %%
# As we can see, this does not change much the posterior distribution, which remains spiky. 
