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
    polyColl[i] = ot.StandardDistributionPolynomialFactory(
        uc.distribution.getMarginal(i)
    )
productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)
r2 = {}
for useTransformation in [False, True]:
    algo = ot.LeastSquaresExpansion(X, Y, distribution, productBasis, indexMax)
    algo.setUseTransformation(useTransformation)
    algo.run()
    pce_result = algo.getResult()
    assert pce_result.getUseTransformation() == useTransformation
    surrogate = pce_result.getMetaModel()
    yPred = surrogate(X)
    validation = ot.MetaModelValidation(Y, yPred)
    r2Score = validation.computeR2Score()[0]
    print(f"{useTransformation=} {r2Score=:.6f}")
    r2[useTransformation] = r2Score
# no transformation (old domination method) gives better R2 for dependent inputs
assert r2[False] > r2[True]

# hot start test
f = ot.SymbolicFunction(["x", "y"], ["sin(x+y)"])
d = ot.ClaytonCopula(1.5)
size = 1000
inL = d.getSample(size)
outL = f(inL)
inT = d.getSample(size)
outT = f(inT)
#
algo = ot.LeastSquaresExpansion(inL, outL, d)
algo.setUseTransformation(True)
algo.run()
validation = ot.MetaModelValidation(algo.getResult().getMetaModel()(inT), outT)
print(f"With transformation, R2={validation.computeR2Score()[0]}")
algo.setUseTransformation(False)
algo.run()
validation = ot.MetaModelValidation(algo.getResult().getMetaModel()(inT), outT)
r2_hot = validation.computeR2Score()[0]
print(f"No transformation after first run, R2={r2_hot}")
algo = ot.LeastSquaresExpansion(inL, outL, d)
algo.setUseTransformation(False)
algo.run()
validation = ot.MetaModelValidation(algo.getResult().getMetaModel()(inT), outT)
r2_cold = validation.computeR2Score()[0]
print(f"No transformation from scratch, R2={r2_cold}")
assert r2_hot == r2_cold
