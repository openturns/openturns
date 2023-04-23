#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import ishigami_function

ot.TESTPREAMBLE()


# Problem parameters
dimension = 3
im = ishigami_function.IshigamiModel()

size = 1000
x = im.distributionX.getSample(size)
y = im.model(x)

# To reduce the time needed by the test
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-LillieforsMinimumSamplingSize", 4)
ot.ResourceMap.SetAsUnsignedInteger("FittingTest-LillieforsMaximumSamplingSize", 4)
ot.ResourceMap.SetAsBool("FunctionalChaosAlgorithm-Sparse", True)
algo = ot.FunctionalChaosAlgorithm(x, y)
algo.run()
result = algo.getResult()
sensitivity = ot.FunctionalChaosSobolIndices(result)

print(sensitivity)
print(sensitivity._repr_html_())
print(sensitivity._repr_markdown_())

# Create consistent PCE of Ishigami
# build basis
degree = 10
enumerateFunction = ot.LinearEnumerateFunction(dimension)
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.LegendreFactory()] * dimension, enumerateFunction
)
adaptiveStrategy = ot.FixedStrategy(productBasis, basisSize)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
x = im.distributionX.getSample(size)
y = im.model(x)
algo = ot.FunctionalChaosAlgorithm(
    x, y, im.distributionX, adaptiveStrategy, projectionStrategy
)
algo.run()
result = algo.getResult()
sensitivity = ot.FunctionalChaosSobolIndices(result)


#
def printSobolResult(S_computed, S_exact):
    absolute_error = abs(S_computed - S_exact)
    print("   S (PCE)    = %.4f" % (S_computed),
          "   S (exact)  = %.4f" % (S_exact),
          "   Abs. Error = %.4e" % (absolute_error))
    return None


#
rtol = 0.0
atol = 0.001
#
print("1. Check first order Sobol' indices")
print("First order, X1")
S_computed = sensitivity.getSobolIndex(0)
printSobolResult(S_computed, im.S1)
assert_almost_equal(S_computed, im.S1, rtol, atol)
#
print("First order, X2")
S_computed = sensitivity.getSobolIndex(1)
printSobolResult(S_computed, im.S2)
assert_almost_equal(S_computed, im.S2, rtol, atol)
#
print("First order, X3")
S_computed = sensitivity.getSobolIndex(2)
printSobolResult(S_computed, im.S3)
assert_almost_equal(S_computed, im.S3, rtol, atol)
#
#
print("2. Check total Sobol' indices")
print("Total, X1")
S_computed = sensitivity.getSobolTotalIndex(0)
printSobolResult(S_computed, im.ST1)
assert_almost_equal(S_computed, im.ST1, rtol, atol)
#
print("Total, X2")
S_computed = sensitivity.getSobolTotalIndex(1)
printSobolResult(S_computed, im.ST2)
assert_almost_equal(S_computed, im.ST2, rtol, atol)
#
print("Total, X3")
S_computed = sensitivity.getSobolTotalIndex(2)
printSobolResult(S_computed, im.ST3)
assert_almost_equal(S_computed, im.ST3, rtol, atol)
#
#
print("3. Check first order (closed) Sobol' indices of groups")
group = [0]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
printSobolResult(S_computed, im.S1)
assert_almost_equal(S_computed, im.S1, rtol, atol)
#
group = [1]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
printSobolResult(S_computed, im.S2)
assert_almost_equal(S_computed, im.S2, rtol, atol)
#
group = [2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
printSobolResult(S_computed, im.S3)
assert_almost_equal(S_computed, im.S3, rtol, atol)
#
group = [0, 1]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
S_exact = im.S1 + im.S2 + im.S12
printSobolResult(S_computed, S_exact)
assert_almost_equal(S_computed, S_exact, rtol, atol)
#
group = [1, 2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
S_exact = im.S2 + im.S3 + im.S23
printSobolResult(S_computed, S_exact)
assert_almost_equal(S_computed, S_exact, rtol, atol)
#
group = [0, 2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
S_exact = im.S1 + im.S3 + im.S13
printSobolResult(S_computed, S_exact)
assert_almost_equal(S_computed, S_exact, rtol, atol)
#
group = [0, 1, 2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedIndex(group)
printSobolResult(S_computed, 1.0)
assert_almost_equal(S_computed, 1.0, rtol, atol)

#
print("4. Check total order index of a group")
group = [0]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
printSobolResult(S_computed, im.ST1)
assert_almost_equal(S_computed, im.ST1, rtol, atol)
#
group = [1]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
printSobolResult(S_computed, im.ST2)
assert_almost_equal(S_computed, im.ST2, rtol, atol)
#
group = [2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
printSobolResult(S_computed, im.ST3)
assert_almost_equal(S_computed, im.ST3, rtol, atol)
#
group = [0, 1]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
S_exact = im.S1 + im.S2 + im.S12 + im.S13 + im.S123
printSobolResult(S_computed, S_exact)
assert_almost_equal(S_computed, S_exact, rtol, atol)
#
group = [1, 2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
S_exact = im.S2 + im.S3 + im.S23 + im.S13 + im.S123
printSobolResult(S_computed, S_exact)
assert_almost_equal(S_computed, S_exact, rtol, atol)
#
group = [0, 2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
S_exact = im.S1 + im.S3 + im.S13 + im.S23 + im.S123
printSobolResult(S_computed, S_exact)
assert_almost_equal(S_computed, S_exact, rtol, atol)
#
group = [0, 1, 2]
print("Group = ", group)
S_computed = sensitivity.getSobolGroupedTotalIndex(group)
printSobolResult(S_computed, 1.0)
assert_almost_equal(S_computed, 1.0, rtol, atol)
