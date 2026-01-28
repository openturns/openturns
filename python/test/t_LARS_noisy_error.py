#!/usr/bin/env python

# # Cas test Chaos polynomial (openturns 1.21)
# Un bug se produit quand:
# size_doe = 60, sparse = True et seed = 1
# Le chaos fitté correspond à un R^2 = 0.0

import openturns as ot
# ot.Log.Show(ot.Log.NONE)

size_doe = 60  # if 50, 59, 61, 62 pas de bug
sparse = True  # if False pas de bug
seed = 1  # si 2 pas de bug

poutre = ot.SymbolicFunction(['L', 'b', 'h', 'E', 'F'],
                             ['(F * L^3)/(48 * E * b * h^3 / 12)'])

L = ot.Normal(5.0, 0.02 * 5.0)

b = ot.LogNormal()
b.setParameter(ot.LogNormalMuSigmaOverMu()([.2, .05, 0.]))

h = ot.LogNormal()
h.setParameter(ot.LogNormalMuSigmaOverMu()([.4, .05, 0.]))

E = ot.LogNormal()
E.setParameter(ot.LogNormalMuSigmaOverMu()([3e4, .12, 0.]))

F = ot.LogNormal()
F.setParameter(ot.LogNormalMuSigmaOverMu()([.1, .20, 0.]))

S = ot.CorrelationMatrix(5)
S[1, 2] = 0.8
R = ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(S)
distribution = ot.JointDistribution([L, b, h, E, F])

ot.RandomGenerator.SetSeed(seed)
experiment = ot.LHSExperiment(distribution, int(size_doe))
inputSample = experiment.generate()
outputSample = poutre(inputSample)
# print(inputSample, outputSample)

dim = distribution.getDimension()
polyCol = [0.] * dim
for i in range(dim):
    polyCol[i] = ot.StandardDistributionPolynomialFactory(distribution.getMarginal(i))

enumerateFunction = ot.LinearEnumerateFunction(dim)
multivariateBasis = ot.OrthogonalProductPolynomialFactory(polyCol, enumerateFunction)
p = 5
indexMax = enumerateFunction.getStrataCumulatedCardinal(p)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, indexMax)
regressionStrategy = ot.LeastSquaresStrategy()
if sparse:
    approximation_algorithm = ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), ot.CorrectedLeaveOneOut())
    regressionStrategy = ot.LeastSquaresStrategy(approximation_algorithm)
algochaos = ot.FunctionalChaosAlgorithm(inputSample, outputSample,
                                        distribution,
                                        adaptiveStrategy,
                                        regressionStrategy)
algochaos.run()
chaosResult = algochaos.getResult()
chaosMeta = chaosResult.getMetaModel()
print(chaosResult, chaosMeta)
validation = ot.MetaModelValidation(outputSample, chaosMeta(inputSample))
R2 = validation.computeR2Score()[0]
print('R2 chaos = %0.7f' % R2)
assert R2 > 0.9, "low R2"

if 0:
    from openturns.viewer import View
    grid = validation.drawValidation()
    graph_validation = grid.getGraph(0, 0)
    graph_validation.setTitle('Validation graph (Training Samples)')
    graph_validation.setXTitle('Metamodel chaos')
    graph_validation.setXTitle('Model original')
    view = View(graph_validation)
    view.save('chaos_validation.png', dpi=100)
    view2 = View(chaosResult.drawErrorHistory())
    view2.save('chaos_error.png', dpi=100)
    View.ShowAll()
