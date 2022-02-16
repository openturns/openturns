"""
Validation of a Karhunen-Loeve decomposition
============================================
"""
# %%
#
# In this example we are going to assess a Karhunen-Loeve decomposition
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create a Gaussian process.
numberOfVertices = 20
interval = ot.Interval(-1.0, 1.0)
mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
covariance = ot.SquaredExponential()
process = ot.GaussianProcess(covariance, mesh)

# %%
# Decompose it using KL-SVD.
sampleSize = 100
processSample = process.getSample(sampleSize)
threshold = 1.0e-7
algo = ot.KarhunenLoeveSVDAlgorithm(processSample, threshold)
algo.run()
klresult = algo.getResult()

# %%
# Instanciate the validation service.
validation = ot.KarhunenLoeveValidation(processSample, klresult)

# %%
# Plot the residual field.
residualProcessSample = validation.computeResidual()
view = viewer.View(residualProcessSample.drawMarginal(0))

# %%
# Plot the residual mean field.
residualMean = validation.computeResidualMean()
view = viewer.View(residualMean.drawMarginal(0))

# %%
# Plot the residual standard deviation field.
residualSigmaField = validation.computeResidualStandardDeviation()
view = viewer.View(residualSigmaField.drawMarginal(0))

# %%
# Build the validation graph.
view = viewer.View(validation.drawValidation())

# %%
# Build the weight graph.
view = viewer.View(validation.drawObservationWeight(0))

# %%
# Build the quality graph.
view = viewer.View(validation.drawObservationQuality())
plt.show()
