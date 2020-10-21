"""
Calibration of the deflection of a tube
=======================================
"""
# %%
# Description
# -----------
#
# We consider the deflection of a tube under a vertical stress.
#
# <img src="_static/simply_supported_beam.png" width="300" />
#
# The parameters of the model are:
#
# * F : the strength,
# * L : the length of the tube,
# * a : position of the force,
# * D : external diameter of the tube,
# * d : internal diameter of the tube,
# * E : Young modulus.
#
# The following figure presents the internal and external diameter of the tube:
#
# <img src="_static/tube-diameters.png" width="200" />
#
# The area moment of inertia of the cross section about the neutral axis of a round tube (i.e. perpendicular to the section) with external and internal diameters :math:`D` and :math:`d` are:
#
# .. math::
#
#    I = \frac{\pi (D^4-d^4)}{32}.
# 
#
# The vertical deflection at point :math:`x=a` is:
#
# .. math::
#    g_1(X) = - F \frac{a^2 (L-a)^2}{3 E L I},
# 
#
# where :math:`X=(F,L,a,D,d,E)`. 
# The angle of the tube at the left end is:
#
# .. math::
#    g_2(X) = 
#    - F \frac{b (L^2-b^2)}{6 E L I},
# 
#
# and the angle of the tube at the right end is:
#
# .. math::
#    g_3(X) = 
#    F \frac{a (L^2-a^2)}{6 E L I}.
# 
#
# The following table presents the distributions of the random variables. These variables are assumed to be independent.
#
# ======== ==========================
# Variable Distribution
# ======== ==========================
# F        Normal(1,0.1)
# L        Normal(1.5,0.01)
# a        Uniform(0.7,1.2)
# D        Triangular(0.75,0.8,0.85)
# d        Triangular(0.09,0.1,0.11)
# E        Normal(200000,2000)
# ======== ==========================
#

# %%
# References
# ----------
#
# * Deflection of beams by Russ Elliott. http://www.clag.org.uk/beam.html
# * https://upload.wikimedia.org/wikipedia/commons/f/ff/Simple_beam_with_offset_load.svg
# * https://en.wikipedia.org/wiki/Deflection_(engineering)
# * https://mechanicalc.com/reference/beam-deflection-tables
# * https://en.wikipedia.org/wiki/Second_moment_of_area
# * Shigley's Mechanical Engineering Design (9th Edition), Richard G. Budynas, J. Keith Nisbettn, McGraw Hill (2011)
# * Mechanics of Materials (7th Edition), James M. Gere, Barry J. Goodno, Cengage Learning (2009)
# * Statics and Mechanics of Materials (5th Edition), Ferdinand Beer, E. Russell Johnston, Jr., John DeWolf, David Mazurek. Mc Graw Hill (2011) Chapter 15: deflection of beams.

# %%
# Create a calibration problem
# ----------------------------

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We use the variable names `De` for the external diameter and `di` for the internal diameter because the symbolic function engine is not case sensitive, hence the variable names `D` and `d` would not be distiguished.

# %%
inputsvars=["F","L","a","De","di","E"]
formula = "var I:=pi_*(De^4-di^4)/32; var b:=L-a; g1:=-F*a^2*(L-a)^2/(3*E*L*I); g2:=-F*b*(L^2-b^2)/(6*E*L*I); g3:=F*a*(L^2-a^2)/(6*E*L*I)"
g = ot.SymbolicFunction(inputsvars,["g1","g2","g3"],formula)
g.setOutputDescription(["Deflection","Left angle","Right angle"])

# %%
XF=ot.Normal(1,0.1)
XE=ot.Normal(200000,2000)
XF.setDescription(["Force"])
XE.setDescription(["Young Modulus"])

# %%
XL = ot.Dirac(1.5)
Xa = ot.Dirac(1.0)
XD = ot.Dirac(0.8)
Xd = ot.Dirac(0.1)
XL.setDescription(["Longueur"])
Xa.setDescription(["Location"])
XD.setDescription(["External diameter"])
Xd.setDescription(["Internal diameter"])

# %%
inputDistribution = ot.ComposedDistribution([XF,XL,Xa,XD,Xd,XE])

# %%
sampleSize = 100
inputSample = inputDistribution.getSample(sampleSize)
inputSample[0:5]

# %%
outputDeflection = g(inputSample)
outputDeflection[0:5]

# %%
observationNoiseSigma = [0.1e-6,0.05e-5,0.05e-5]
observationNoiseCovariance = ot.CovarianceMatrix(3)
for i in range(3):
    observationNoiseCovariance[i,i] = observationNoiseSigma[i]**2

# %%
noiseSigma = ot.Normal([0.,0.,0.],observationNoiseCovariance)
sampleObservationNoise = noiseSigma.getSample(sampleSize)
observedOutput = outputDeflection + sampleObservationNoise
observedOutput[0:5]

# %%
observedInput = ot.Sample(sampleSize,2)
observedInput[:,0] = inputSample[:,0] # F
observedInput[:,1] = inputSample[:,5] # E
observedInput.setDescription(["Force","Young Modulus"])
observedInput[0:5]

# %%
fullSample = ot.Sample(sampleSize,5)
fullSample[:,0:2] = observedInput
fullSample[:,2:5] = observedOutput
fullSample.setDescription(["Force","Young","Deflection","Left Angle","Right Angle"])
fullSample[0:5]

# %%
graph = ot.VisualTest.DrawPairs(fullSample)
view = viewer.View(graph)

# %%
# Setting up the calibration
# --------------------------

# %%
XL = 1.4 # Exact : 1.5
Xa = 1.2 # Exact : 1.0
XD = 0.7 # Exact : 0.8
Xd = 0.2 # Exact : 0.1
thetaPrior = ot.Point([XL,Xa,XD,Xd])


# %%
sigmaXL = 0.1 * XL
sigmaXa = 0.1 * Xa
sigmaXD = 0.1 * XD
sigmaXd = 0.1 * Xd
parameterCovariance = ot.CovarianceMatrix(4)
parameterCovariance[0,0] = sigmaXL**2
parameterCovariance[1,1] = sigmaXa**2
parameterCovariance[2,2] = sigmaXD**2
parameterCovariance[3,3] = sigmaXd**2
parameterCovariance

# %%
calibratedIndices = [1,2,3,4]
calibrationFunction = ot.ParametricFunction(g, calibratedIndices, thetaPrior)

# %%
sigmaObservation = [0.2e-6,0.03e-5,0.03e-5] # Exact : 0.1e-6

# %%
errorCovariance = ot.CovarianceMatrix(3)
errorCovariance[0,0] = sigmaObservation[0]**2
errorCovariance[1,1] = sigmaObservation[1]**2
errorCovariance[2,2] = sigmaObservation[2]**2

# %%
calibrationFunction.setParameter(thetaPrior)
predictedOutput = calibrationFunction(observedInput)
predictedOutput[0:5]

# %%
# Calibration with gaussian non linear least squares
# --------------------------------------------------

# %%
algo = ot.GaussianNonLinearCalibration(calibrationFunction, observedInput, observedOutput, thetaPrior, parameterCovariance, errorCovariance)

# %%
algo.run()

# %%
calibrationResult = algo.getResult()

# %%
# Analysis of the results
# -----------------------

# %%
thetaMAP = calibrationResult.getParameterMAP()
thetaMAP

# %%
# Compute a 95% confidence interval for each marginal.

# %%
thetaPosterior = calibrationResult.getParameterPosterior()
alpha = 0.95
dim = thetaPosterior.getDimension()
for i in range(dim):
    print(thetaPosterior.getMarginal(i).computeBilateralConfidenceInterval(alpha))

# %%
graph = calibrationResult.drawObservationsVsInputs()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawObservationsVsPredictions()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawResiduals()
view = viewer.View(graph)

# %%
graph = calibrationResult.drawParameterDistributions()
view = viewer.View(graph)

plt.show()
