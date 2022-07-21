import openturns as ot
from openturns.viewer import View
import math as m

ot.RandomGenerator.SetSeed(0)


def flooding(X):
    L, B = 5.0e3, 300.0
    Q, K_s, Z_v, Z_m = X
    alpha = (Z_m - Z_v)/L
    H = (Q/(K_s*B*m.sqrt(alpha)))**(3.0/5.0)
    return [H]


g = ot.PythonFunction(4, 1, flooding)
Q = ot.TruncatedDistribution(
    ot.Gumbel(558.0, 1013.0), ot.TruncatedDistribution.LOWER)
K_s = ot.Dirac(30.0)
Z_v = ot.Dirac(50.0)
Z_m = ot.Dirac(55.0)
inputRandomVector = ot.ComposedDistribution([Q, K_s, Z_v, Z_m])
nbobs = 100
inputSample = inputRandomVector.getSample(nbobs)
outputH = g(inputSample)
Hobs = outputH + ot.Normal(0.0, 0.1).getSample(nbobs)
Qobs = inputSample[:, 0]
thetaPrior = [20, 49, 51]
model = ot.ParametricFunction(g, [1, 2, 3], thetaPrior)
errorCovariance = ot.CovarianceMatrix([[0.5**2]])
sigma = ot.CovarianceMatrix(3)
sigma[0, 0] = 5.**2
sigma[1, 1] = 1.**2
sigma[2, 2] = 1.**2
algo = ot.GaussianNonLinearCalibration(
    model, Qobs, Hobs, thetaPrior, sigma, errorCovariance)
algo.run()
result = algo.getResult()
graph = result.drawObservationsVsPredictions()

View(graph)
