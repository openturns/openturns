#! /usr/bin/env python

# Polynomial chaos expansion fitted on a real quadrature design: the raw and
# total Sobol indices must converge to the analytical values of the Ishigami
# function, and the weights must be the quadrature weights of the density.
#
# The other weighted tests of the branch use synthetic non-uniform weights.
# Here the design comes from a Gauss product experiment, whose weights sum to
# 1 and are strongly non-uniform, so any estimator normalized by the sample
# size instead of the weight mass would be caught: replacing the quadrature
# weights by 1/n on the same nodes degrades the Sobol indices by a factor of
# about 7 at this degree.

import openturns as ot
import openturns.testing as ott
from math import pi

ot.TESTPREAMBLE()

# Analytical Sobol indices of the Ishigami function
a = 7.0
b = 0.1
covariance = (b**2 * pi**8) / 18.0 + (b * pi**4) / 5.0 + (a**2) / 8.0 + 0.5
s1Reference = [
    ((b * pi**4 / 5.0 + b**2 * pi**8 / 50.0 + 0.5) / covariance),
    ((a**2 / 8.0) / covariance),
    0.0,
]
interaction = (b**2 * pi**8 / 18.0 - b**2 * pi**8 / 50.0) / covariance
stReference = [s1Reference[0] + interaction, s1Reference[1], s1Reference[2] + interaction]

model = ot.SymbolicFunction(
    ["x1", "x2", "x3"],
    ["sin(x1) + 7.0 * (sin(x2))^2 + 0.1 * x3^4 * sin(x1)"],
)
distribution = ot.JointDistribution([ot.Uniform(-pi, pi)] * 3)
enumerateFunction = ot.LinearEnumerateFunction(3)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * 3, enumerateFunction
)
degree = 8
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)

# Independent sample for the quality of the approximation
ot.RandomGenerator.SetSeed(123456)
testInput = distribution.getSample(5000)
testOutput = model(testInput)
testVariance = testOutput.computeVariance()[0]


def fitAndScore(levels):
    """Fit the expansion on a Gauss product design and return its diagnostics."""
    inputSample, weight = ot.GaussProductExperiment(distribution, levels).generateWithWeights()
    outputSample = model(inputSample)
    ot.RandomGenerator.SetSeed(0)
    algorithm = ot.FunctionalChaosAlgorithm(
        inputSample,
        weight,
        outputSample,
        distribution,
        ot.FixedStrategy(productBasis, basisSize),
    )
    algorithm.run()
    result = algorithm.getResult()
    sobolIndices = ot.FunctionalChaosSobolIndices(result)
    s1Error = max(
        abs(sobolIndices.getSobolIndex(i) - s1Reference[i]) for i in range(3)
    )
    stError = max(
        abs(sobolIndices.getSobolTotalIndex(i) - stReference[i]) for i in range(3)
    )
    residual = testOutput - result.getMetaModel()(testInput)
    meanSquareError = residual.computeRawMoment(2)[0]
    print(
        f"levels={levels} size={inputSample.getSize()} basisSize={basisSize} "
        f"S1 error={s1Error:.3e} ST error={stError:.3e} "
        f"L2={meanSquareError:.3e} R2={1.0 - meanSquareError / testVariance:.6f}"
    )
    return inputSample, weight, s1Error, stError, meanSquareError


# A coarse design, then a finer one: both must determine the coefficients and
# the quadrature must be exact for the basis degree
coarseSample, coarseWeight, coarseS1, coarseST, coarseL2 = fitAndScore((9, 9, 9))
sample, weight, s1Error, stError, meanSquareError = fitAndScore((11, 11, 11))

# The design has to determine the coefficients, otherwise the least squares
# problem is rank deficient and the by-products are meaningless
ott.assert_almost_equal(sample.getSize() - basisSize, 1166, 0.0, 0.0)
ott.assert_almost_equal(coarseSample.getSize() - basisSize, 564, 0.0, 0.0)

# The weights are the quadrature weights of a density: they sum to 1 and they
# are strongly non-uniform, which is what makes this a weighted test
ott.assert_almost_equal(sum(weight), 1.0, 1.0e-12, 0.0)
ott.assert_almost_equal(sum(coarseWeight), 1.0, 1.0e-12, 0.0)
print("weight ratio = ", max(weight) / min(weight))
assert max(weight) / min(weight) > 50.0

# Refining the design improves the by-products: this is the convergence
# statement, independent of any threshold
assert s1Error < coarseS1 / 5.0
assert stError < coarseST / 5.0
assert meanSquareError < coarseL2

# Accuracy of the by-products on the refined design. The tolerance is about
# three times the observed error and about four times smaller than the error
# obtained when the quadrature weights are replaced by 1/n weights
ott.assert_almost_equal(s1Error, 0.0, 0.0, 3.0e-4)
ott.assert_almost_equal(stError, 0.0, 0.0, 3.0e-4)
ott.assert_almost_equal(meanSquareError, 0.0, 0.0, 1.0e-2)
ott.assert_almost_equal(1.0 - meanSquareError / testVariance, 1.0, 0.0, 1.0e-3)

print("Weighted PCE on a quadrature design: OK")
