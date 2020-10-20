"""
Calibration of the Chaboche mechanical model
============================================
"""
# %%
#
# Deterministic model
# -------------------
#
# The Chaboche mecanical law predicts the stress depending on the strain:
#
# .. math::
#    \sigma = G(\epsilon,R,C,\gamma) = R + \frac{C}{\gamma} (1-\exp(-\gamma\epsilon))
#
#
# where:
#
# - :math:`\epsilon` is the strain,
# - :math:`\sigma` is the stress (Pa),
# - :math:`R`, :math:`C`, :math:`\gamma` are the parameters.
#
# The variables have the following distributions and are supposed to be independent.
#
#  ================  ===========================================================
#  Random var.       Distribution
#  ================  ===========================================================
#  :math:`R`         Lognormal (:math:`\mu = 750` MPa, :math:`\sigma = 11` MPa)
#  :math:`C`         Normal (:math:`\mu = 2750` MPa, :math:`\sigma = 250` MPa)
#  :math:`\gamma`    Normal (:math:`\mu = 10`, :math:`\sigma = 2`)
#  :math:`\epsilon`  Uniform(a=0, b=0.07).
#  ================  ===========================================================
#
# Parameters to calibrate
# -----------------------
#
# The vector of parameters to calibrate is:
#
# .. math::
#    \theta = (R,C,\gamma).
#
#
# We set:
#
# - :math:`R = 750\times 10^6`,
# - :math:`C = 2750\times 10^6`,
# - :math:`\gamma = 10`.
#
# Observations
# ------------
#
# In order to create a calibration problem, we make the hypothesis that the strain has the following distribution:
#
# .. math::
#    \epsilon \sim Uniform(0,0.07).
#
#
# Moreover, we consider a gaussian noise on the observed constraint:
#
# .. math::
#    \epsilon_\sigma \sim \mathcal{N} \left(0,10\times 10^6\right)
#
#
# and we make the hypothesis that the observation errors are independent.
# We set the number of observations to:
#
# .. math::
#    n = 100.
#
#
# We generate a Monte-Carlo sampling with size :math:`n`:
#
# .. math::
#    \sigma_i = G(\epsilon_i,R,C,\gamma) + (\epsilon_\sigma)_i,
#
#
# for :math:`i = 1,..., n`.
# The observations are the pairs :math:`\{(\epsilon_i,\sigma_i)\}_{i=1,...,n}`, i.e. each observation is a couple made of the strain and the corresponding stress.
#
# Thanks to
# ---------
#
# - Antoine Dumas, Phimeca
#

# %%
# References
# ----------
#
# - J. Lemaitre and J. L. Chaboche (2002) "Mechanics of solid materials" Cambridge University Press.
#

# %%
# Generate the observations
# -------------------------

# %%
import numpy as np
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Define the model.

# %%
def modelChaboche(X):
    strain, R, C, gamma = X
    stress = R + C * (1 - np.exp(-gamma * strain)) / gamma
    return [stress]


# %%
# Create the Python function.

# %%
g = ot.PythonFunction(4, 1, modelChaboche)

# %%
# Define the random vector.

# %%
Strain = ot.Uniform(0, 0.07)
unknownR = 750e6
unknownC = 2750e6
unknownGamma = 10
R = ot.Dirac(unknownR)
C = ot.Dirac(unknownC)
Gamma = ot.Dirac(unknownGamma)

Strain.setDescription(["Strain"])
R.setDescription(["R"])
C.setDescription(["C"])
Gamma.setDescription(["Gamma"])

# %%
# Create the joint input distribution function.

# %%
inputDistribution = ot.ComposedDistribution([Strain, R, C, Gamma])

# %%
# Create the Monte-Carlo sample.

# %%
sampleSize = 100
inputSample = inputDistribution.getSample(sampleSize)
outputStress = g(inputSample)
outputStress[0:5]

# %%
# Plot the histogram of the output.

# %%
histoGraph = ot.HistogramFactory().build(outputStress / 1.0e6).drawPDF()
histoGraph.setTitle("Histogram of the sample stress")
histoGraph.setXTitle("Stress (MPa)")
histoGraph.setLegends([""])
view = viewer.View(histoGraph)

# %%
# Generate observation noise.

# %%
stressObservationNoiseSigma = 10.0e6  # (Pa)
noiseSigma = ot.Normal(0.0, stressObservationNoiseSigma)
sampleNoiseH = noiseSigma.getSample(sampleSize)
observedStress = outputStress + sampleNoiseH

# %%
observedStrain = inputSample[:, 0]

# %%
graph = ot.Graph("Observations", "Strain", "Stress (MPa)", True)
cloud = ot.Cloud(observedStrain, observedStress / 1.0e6)
graph.add(cloud)
view = viewer.View(graph)

# %%
# Set the calibration parameters
# ------------------------------

# %%
# Define the value of the reference values of the :math:`\theta` parameter. In the bayesian framework, this is called the mean of the *prior* gaussian distribution. In the data assimilation framework, this is called the *background*.

# %%
R = 700e6  # Exact : 750e6
C = 2500e6  # Exact : 2750e6
Gamma = 8.0  # Exact : 10
thetaPrior = ot.Point([R, C, Gamma])


# %%
# The following statement create the calibrated function from the model. The calibrated parameters `R`, `C`, `Gamma` are at indices 1, 2, 3 in the inputs arguments of the model.

# %%
calibratedIndices = [1, 2, 3]
mycf = ot.ParametricFunction(g, calibratedIndices, thetaPrior)

# %%
# Calibration with linear least squares
# -------------------------------------

# %%
# The `LinearLeastSquaresCalibration` class performs the linear least squares calibration by linearizing the model in the neighbourhood of the reference point.

# %%
algo = ot.LinearLeastSquaresCalibration(
    mycf, observedStrain, observedStress, thetaPrior, "SVD"
)

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

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
thetaPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# We can see that the `Gamma` parameter has a large confidence interval : even the sign of the parameter is unknown.
# The parameter which is calibrated with the smallest confidence
# interval is `R`, which confidence interval is [708.3,780.0] (MPa).
# This is why this parameter seems the most important in this case.

# %%
graph = calibrationResult.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# We see that there is a good fit after calibration, since the predictions after calibration (i.e. the green crosses) are close to the observations (i.e. the blue crosses).

# %%
graph = calibrationResult.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# We see that there is a much better fit after calibration, since the predictions are close to the diagonal of the graphics.

# %%
observationError = calibrationResult.getObservationsError()
observationError

# %%
graph = calibrationResult.drawResiduals()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# The analysis of the residuals shows that the distribution is centered on zero and symmetric. This indicates that the calibration performed well.
#
# Moreover, the distribution of the residuals is close to being gaussian.

# %%
graph = calibrationResult.drawParameterDistributions()
view = viewer.View(graph)

# %%
# Calibration with nonlinear least squares
# ----------------------------------------

# %%
# The `NonLinearLeastSquaresCalibration` class performs the non linear least squares calibration by minimizing the squared euclidian norm between the predictions and the observations.

# %%
algo = ot.NonLinearLeastSquaresCalibration(mycf, observedStrain, observedStress, thetaPrior)


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

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
thetaPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# We can see that all three parameters are estimated with a large confidence interval.

# %%
graph = calibrationResult.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# We see that there is a good fit after calibration, since the predictions after calibration (i.e. the green crosses) are close to the observations (i.e. the blue crosses).

# %%
graph = calibrationResult.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# We see that there is a much better fit after calibration, since the predictions are close to the diagonal of the graphics.

# %%
observationError = calibrationResult.getObservationsError()
observationError

# %%
graph = observationError.drawPDF()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawResiduals()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# The analysis of the residuals shows that the distribution is centered on zero and symmetric. This indicates that the calibration performed well.
#
# Moreover, the distribution of the residuals is close to being gaussian. This indicates that the observation error might be gaussian.

# %%
graph = calibrationResult.drawParameterDistributions()
view = viewer.View(graph)

# %%
# Gaussian calibration parameters
# -------------------------------

# %%
# The standard deviation of the observations errors.

# %%
sigmaStress = 1.0e7  # (Pa)

# %%
# Define the covariance matrix of the output Y of the model.

# %%
errorCovariance = ot.CovarianceMatrix(1)
errorCovariance[0, 0] = sigmaStress ** 2

# %%
# Define the covariance matrix of the parameters :math:`\theta` to calibrate.

# %%
sigmaR = 0.1 * R
sigmaC = 0.1 * C
sigmaGamma = 0.1 * Gamma

# %%
sigma = ot.CovarianceMatrix(3)
sigma[0, 0] = sigmaR ** 2
sigma[1, 1] = sigmaC ** 2
sigma[2, 2] = sigmaGamma ** 2
sigma

# %%
# Gaussian linear calibration
# ---------------------------

# %%
# The `GaussianLinearCalibration` class performs the gaussian linear calibration by linearizing the model in the neighbourhood of the prior.

# %%
algo = ot.GaussianLinearCalibration(mycf, observedStrain, observedStress, thetaPrior, sigma, errorCovariance)

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

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
thetaPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# We can see that all three parameters are estimated with a large confidence interval.

# %%
graph = calibrationResult.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# We see that there is a good fit after calibration, since the predictions after calibration (i.e. the green crosses) are close to the observations (i.e. the blue crosses).

# %%
graph = calibrationResult.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# We see that there is a much better fit after calibration, since the predictions are close to the diagonal of the graphics.

# %%
# The observation error is predicted by linearizing the problem at the prior.

# %%
observationError = calibrationResult.getObservationsError()
observationError

# %%
# This can be compared to the residuals distribution, which is computed at the posterior.

# %%
graph = calibrationResult.drawResiduals()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# The analysis of the gaussian distribution (the blue line) of the observation errors is close to the posterior distribution of the residuals (the green line). Moreover, the posterior distribution is centered. These informations indicate that the calibration performed well.

# %%
graph = calibrationResult.drawParameterDistributions()
view = viewer.View(graph)

# %%
# For the :math:`R` variable, the observations are much more important than the prior: this is shown by the fact that the posterior and prior distribution of the :math:`R` variable are very different.
#
# We see that the prior and posterior distribution are close to each other for the :math:`\gamma` variable: the observations did not convey much information for this variable.

# %%
# Gaussian nonlinear calibration
# ------------------------------

# %%
# The `GaussianNonLinearCalibration` class performs the gaussian nonlinear calibration.

# %%
algo = ot.GaussianNonLinearCalibration(mycf, observedStrain, observedStress, thetaPrior, sigma, errorCovariance)

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

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
thetaPosterior.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)[0]

# %%
# We can see that all three parameters are estimated with a large confidence interval.

# %%
graph = calibrationResult.drawObservationsVsInputs()
graph.setLegendPosition("topleft")
view = viewer.View(graph)

# %%
# We see that there is a good fit after calibration, since the predictions after calibration (i.e. the green crosses) are close to the observations (i.e. the blue crosses).

# %%
graph = calibrationResult.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
# We see that there is a much better fit after calibration, since the predictions are close to the diagonal of the graphics.

# %%
# The observation error is predicted by bootstraping the problem at the posterior.

# %%
observationError = calibrationResult.getObservationsError()
observationError

# %%
# This can be compared to the residuals distribution, which is computed at the posterior.

# %%
graph = calibrationResult.drawResiduals()
view = viewer.View(graph)

# %%
# The analysis is very similar to the linear calibration.

# %%
graph = calibrationResult.drawParameterDistributions()
view = viewer.View(graph)

plt.show()
# %%
# We see that the prior and posterior distribution for the :math:`\gamma` parameter are close to each other, but not superimposed: the observations significantly brought information to the variable :math:`\gamma` during the calibration.
