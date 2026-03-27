import openturns as ot
from openturns.usecases import ishigami_function

uc = ishigami_function.IshigamiModel()
# add correlation
distribution = uc.distribution
R = ot.CorrelationMatrix(uc.dim)
R[1, 0] = 0.4
R[2, 0] = 0.25
R[2, 1] = 0.1
distribution.setCopula(ot.NormalCopula(R))
X = distribution.getSample(100)
Y = uc.model(X)
degree = 5
enumerateFunction = ot.LinearEnumerateFunction(uc.dim)
indexMax = enumerateFunction.getBasisSizeFromTotalDegree(degree)
polyColl = [0.0] * uc.dim
for i in range(uc.distribution.getDimension()):
    polyColl[i] = ot.StandardDistributionPolynomialFactory(uc.distribution.getMarginal(i))
productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)
r2 = {}
for useDomination in [False, True]:
    algo = ot.LeastSquaresExpansion(
        X, Y, distribution, productBasis, indexMax
    )
    algo.setUseDomination(useDomination)
    algo.run()
    pce_result = algo.getResult()
    assert pce_result.getUseDomination() == useDomination
    surrogate = pce_result.getMetaModel()
    yPred = surrogate(X)
    validation = ot.MetaModelValidation(Y, yPred)
    r2Score = validation.computeR2Score()[0]
    print(f"{useDomination=} {r2Score=:.6f}")
    r2[useDomination] = r2Score
assert r2[True] > r2[False]
