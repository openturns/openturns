"""
Calibration of the flooding model
=================================

The goal of this script is to produce the calibration.png image
that is printed in the documentation of the examples.
It is adapted from plot_calibration_flooding.py
"""

from openturns.usecases import flood_model
from matplotlib import pylab as plt
import openturns.viewer as viewer
import numpy as np
import openturns as ot

ot.ResourceMap.SetAsUnsignedInteger("Normal-SmallDimension", 1)
ot.Log.Show(ot.Log.NONE)

fm = flood_model.FloodModel()

Qobs = fm.data[:, 0]
Hobs = fm.data[:, 1]
nbobs = fm.data.getSize()


def functionFlooding(X):
    L = 5.0e3
    B = 300.0
    Q, K_s, Z_v, Z_m = X
    alpha = (Z_m - Z_v) / L
    if alpha < 0.0 or K_s <= 0.0:
        H = np.inf
    else:
        H = (Q / (K_s * B * np.sqrt(alpha))) ** (3.0 / 5.0)
    return [H]


g = ot.PythonFunction(4, 1, functionFlooding)
g = ot.MemoizeFunction(g)
g.setInputDescription(["Q ($m^3/s$)", "Ks ($m^{1/3}/s$)", "Zv (m)", "Zm (m)"])
g.setOutputDescription(["H (m)"])

KsInitial = 20.0
ZvInitial = 49.0
ZmInitial = 51.0
thetaPrior = [KsInitial, ZvInitial, ZmInitial]

calibratedIndices = [1, 2, 3]
mycf = ot.ParametricFunction(g, calibratedIndices, thetaPrior)

sigmaH = 0.5  # (m^2)

errorCovariance = ot.CovarianceMatrix(1)
errorCovariance[0, 0] = sigmaH**2

sigmaKs = 5.0
sigmaZv = 1.0
sigmaZm = 1.0

sigma = ot.CovarianceMatrix(3)
sigma[0, 0] = sigmaKs**2
sigma[1, 1] = sigmaZv**2
sigma[2, 2] = sigmaZm**2

defaultBootstrapSize = ot.ResourceMap.GetAsUnsignedInteger("GaussianNonLinearCalibration-BootstrapSize")

ot.ResourceMap.SetAsUnsignedInteger("GaussianNonLinearCalibration-BootstrapSize", 0)

algo = ot.GaussianNonLinearCalibration(
    mycf, Qobs, Hobs, thetaPrior, sigma, errorCovariance
)
algo.run()
calibrationResult = algo.getResult()

grid = calibrationResult.drawParameterDistributions()
graph = grid.getGraph(0, 0)
graph.setLegends(grid.getGraph(0, 2).getLegends())
graph.setLegendPosition("topleft")
view = viewer.View(
    graph,
    figure_kw={"figsize": (4.0, 3.0)},
)
plt.subplots_adjust(bottom = 0.2, left = 0.2)
view.getFigure().savefig("calibration.png", bbox_inches="tight")
plt.show()

# Set back to default
ot.ResourceMap.SetAsUnsignedInteger("GaussianNonLinearCalibration-BootstrapSize", defaultBootstrapSize)
