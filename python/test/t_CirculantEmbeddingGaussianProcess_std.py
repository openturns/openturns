#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Basic 1D test with ExponentialModel (scalar output)
print("=" * 60)
print("Test 1D CirculantEmbeddingGaussianProcess (scalar output)")
print("=" * 60)
amplitude = [1.0]
scale = [1.0]
covarianceModel = ot.ExponentialModel(scale, amplitude)

tmin = 0.0
tmax = 10.0
interval = ot.Interval(tmin, tmax)
n = 100
discretization = [n - 1]

# Create process
process = otexp.CirculantEmbeddingGaussianProcess(covarianceModel, interval, discretization)
print("process = ", process)
print("is stationary? ", process.isStationary())
print("is normal? ", process.isNormal())
print("interval = ", process.getInterval())
print("discretization = ", process.getDiscretization())

# Generate a realization
realization = process.getRealization()
print("realization dimension = ", realization.getOutputDimension())
print("realization size = ", realization.getValues().getSize())

# Verify the covariance structure empirically
ot.RandomGenerator.SetSeed(0)
size = 500
sample = process.getSample(size)
mean = sample.computeMean()
print("empirical mean = ", mean)
ott.assert_almost_equal(mean[0, 0], 0.0, 0.0, 0.2)

# Compute empirical covariance at lag 0 vs theoretical variance
empiricalVar = sample.computeVariance()
theoreticalVar = covarianceModel(ot.Point(1, 0.0))[0, 0]
print("empirical variance = ", empiricalVar[0, 0], "theoretical = ", theoreticalVar)
ott.assert_almost_equal(empiricalVar[0, 0], theoreticalVar, 0.0, 0.15)

# Test with SquaredExponential
print("\nTest with SquaredExponential")
cov2 = ot.SquaredExponential([1.0], [1.0])
process2 = otexp.CirculantEmbeddingGaussianProcess(cov2, interval, discretization)
realization2 = process2.getRealization()

# Test get/set circular size
print("\ncircular size = ", process.getCircularSize())
process.setCircularSize([256])
print("circular size after set = ", process.getCircularSize())
realization3 = process.getRealization()

# Test FFT algorithm accessor
fftAlgo = process.getFFTAlgorithm()
print("FFT algorithm = ", fftAlgo)

# Test marginal
print("\nTest getMarginal")
marginal = process.getMarginal([0])
print("marginal = ", marginal)

# Test with different discretization
print("\nTest with different discretization")
interval2 = ot.Interval(0.0, 5.0)
discretization2 = [50]
process3 = otexp.CirculantEmbeddingGaussianProcess(covarianceModel, interval2, discretization2)
realization4 = process3.getRealization()

# Test that getRealization returns consistent mesh
field = process.getRealization()
assert field.getMesh().getVerticesNumber() == n, "mesh mismatch"

# Test setTimeGrid
print("\nTest setTimeGrid")
timeGrid = ot.RegularGrid(tmin, 0.1, n)
process.setTimeGrid(timeGrid)
print("circular size after setTimeGrid = ", process.getCircularSize())

# Verify method returns the covariance model
cov = process.getCovarianceModel()
print("covariance model = ", cov)

# Test with MaternModel
print("\nTest with MaternModel")
cov3 = ot.MaternModel([0.5], [1.0], 1.5)
process4 = otexp.CirculantEmbeddingGaussianProcess(cov3, interval, discretization)
realization5 = process4.getRealization()

# Larger sample to verify variance
ot.RandomGenerator.SetSeed(1)
sample2 = process4.getSample(200)
empiricalVar2 = sample2.computeVariance()
theoreticalVar2 = cov3(ot.Point(1, 0.0))[0, 0]
print("empirical variance = ", empiricalVar2[0, 0], "theoretical = ", theoreticalVar2)
ott.assert_almost_equal(empiricalVar2[0, 0], theoreticalVar2, 0.0, 0.2)

# Test 1D with vector-valued output
print("\n" + "=" * 60)
print("Test 1D CirculantEmbeddingGaussianProcess (vector output)")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
amplitudeVec = [1.0, 0.5]
covVec = ot.ExponentialModel([1.0], amplitudeVec)
processVec = otexp.CirculantEmbeddingGaussianProcess(covVec, interval, [50])
print("process vec = ", processVec)
print("output dimension = ", processVec.getOutputDimension())

# Generate realization
realizationVec = processVec.getRealization()
print("realization output dimension = ", realizationVec.getOutputDimension())
print("realization values shape = ", realizationVec.getValues().getSize(), "x", realizationVec.getValues().getDimension())

# Check statistics
sampleVec = processVec.getSample(200)
meanVec = sampleVec.computeMean()
print("vector empirical mean = ", meanVec)
ott.assert_almost_equal(meanVec[0, 0], 0.0, 0.0, 0.2)
ott.assert_almost_equal(meanVec[0, 1], 0.0, 0.0, 0.2)

empiricalVarVec = sampleVec.computeVariance()
theoreticalVarVec = covVec(ot.Point(1, 0.0))
print("vector empirical variance = ", empiricalVarVec)
print("vector theoretical variance = ", theoreticalVarVec)
ott.assert_almost_equal(empiricalVarVec[0, 0], theoreticalVarVec[0, 0], 0.0, 0.2)
ott.assert_almost_equal(empiricalVarVec[1, 1], theoreticalVarVec[1, 1], 0.0, 0.2)

# Test 2D
print("\n" + "=" * 60)
print("Test 2D CirculantEmbeddingGaussianProcess")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov2D = ot.ExponentialModel([1.0, 1.0], [1.0])
interval2D = ot.Interval([0.0, 0.0], [5.0, 5.0])
discretization2D = [10, 8]
process2D = otexp.CirculantEmbeddingGaussianProcess(cov2D, interval2D, discretization2D)
print("process 2D = ", process2D)
print("is stationary? ", process2D.isStationary())
print("interval = ", process2D.getInterval())
print("discretization = ", process2D.getDiscretization())
print("circular size = ", process2D.getCircularSize())

# Generate a 2D realization
realization2D = process2D.getRealization()
print("2D realization output dimension = ", realization2D.getOutputDimension())
print("2D realization vertices = ", realization2D.getMesh().getVerticesNumber())
ott.assert_almost_equal(realization2D.getMesh().getVerticesNumber(), 11 * 9, 0.0, 0.0)

# Compute statistics
sample2D = process2D.getSample(100)
mean2D = sample2D.computeMean()
print("2D empirical mean = ", mean2D)
ott.assert_almost_equal(mean2D[0, 0], 0.0, 0.0, 0.3)

empiricalVar2D = sample2D.computeVariance()
theoreticalVar2D = cov2D(ot.Point(2, 0.0))[0, 0]
print("2D empirical variance = ", empiricalVar2D[0, 0], "theoretical = ", theoreticalVar2D)
ott.assert_almost_equal(empiricalVar2D[0, 0], theoreticalVar2D, 0.0, 0.3)

# Test 3D
print("\n" + "=" * 60)
print("Test 3D CirculantEmbeddingGaussianProcess")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov3D = ot.ExponentialModel([1.0, 1.0, 1.0], [1.0])
interval3D = ot.Interval([0.0, 0.0, 0.0], [3.0, 3.0, 3.0])
discretization3D = [6, 6, 6]
process3D = otexp.CirculantEmbeddingGaussianProcess(cov3D, interval3D, discretization3D)
print("process 3D = ", process3D)
print("is stationary? ", process3D.isStationary())
print("interval = ", process3D.getInterval())
print("discretization = ", process3D.getDiscretization())
print("circular size = ", process3D.getCircularSize())

# Generate a 3D realization
realization3D = process3D.getRealization()
print("3D realization output dimension = ", realization3D.getOutputDimension())
print("3D realization vertices = ", realization3D.getMesh().getVerticesNumber())
ott.assert_almost_equal(realization3D.getMesh().getVerticesNumber(), 7 * 7 * 7, 0.0, 0.0)

# Compute statistics
sample3D = process3D.getSample(50)
mean3D = sample3D.computeMean()
print("3D empirical mean = ", mean3D)
ott.assert_almost_equal(mean3D[0, 0], 0.0, 0.0, 0.4)

empiricalVar3D = sample3D.computeVariance()
theoreticalVar3D = cov3D(ot.Point(3, 0.0))[0, 0]
print("3D empirical variance = ", empiricalVar3D[0, 0], "theoretical = ", theoreticalVar3D)
ott.assert_almost_equal(empiricalVar3D[0, 0], theoreticalVar3D, 0.0, 0.4)

# Test 4D
print("\n" + "=" * 60)
print("Test 4D CirculantEmbeddingGaussianProcess")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov4D = ot.ExponentialModel([1.0] * 4, [1.0])
interval4D = ot.Interval([0.0] * 4, [2.0] * 4)
discretization4D = [4] * 4
process4D = otexp.CirculantEmbeddingGaussianProcess(cov4D, interval4D, discretization4D)
print("process 4D = ", process4D)
print("is stationary? ", process4D.isStationary())
print("interval = ", process4D.getInterval())
print("discretization = ", process4D.getDiscretization())
print("circular size = ", process4D.getCircularSize())

# Generate a 4D realization
realization4D = process4D.getRealization()
nVertices4D = 5 ** 4
print("4D realization output dimension = ", realization4D.getOutputDimension())
print("4D realization vertices = ", realization4D.getMesh().getVerticesNumber())
ott.assert_almost_equal(realization4D.getMesh().getVerticesNumber(), nVertices4D, 0.0, 0.0)

# Compute statistics
sample4D = process4D.getSample(30)
mean4D = sample4D.computeMean()
print("4D empirical mean = ", mean4D)
ott.assert_almost_equal(mean4D[0, 0], 0.0, 0.0, 0.5)

empiricalVar4D = sample4D.computeVariance()
theoreticalVar4D = cov4D(ot.Point(4, 0.0))[0, 0]
print("4D empirical variance = ", empiricalVar4D[0, 0], "theoretical = ", theoreticalVar4D)
ott.assert_almost_equal(empiricalVar4D[0, 0], theoreticalVar4D, 0.0, 0.5)

# Test 2D with vector-valued output
print("\n" + "=" * 60)
print("Test 2D CirculantEmbeddingGaussianProcess (vector output)")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov2DVec = ot.ExponentialModel([1.0, 1.0], [1.0, 0.5])
interval2D = ot.Interval([0.0, 0.0], [3.0, 3.0])
process2DVec = otexp.CirculantEmbeddingGaussianProcess(cov2DVec, interval2D, [6, 6])
print("process 2D vec = ", process2DVec)
print("output dimension = ", process2DVec.getOutputDimension())

# Generate realization
realization2DVec = process2DVec.getRealization()
print("2D vec output dimension = ", realization2DVec.getOutputDimension())
print("2D vec vertices = ", realization2DVec.getMesh().getVerticesNumber())
ott.assert_almost_equal(realization2DVec.getMesh().getVerticesNumber(), 7 * 7, 0.0, 0.0)

sample2DVec = process2DVec.getSample(50)
mean2DVec = sample2DVec.computeMean()
print("2D vec empirical mean = ", mean2DVec)
ott.assert_almost_equal(mean2DVec[0, 0], 0.0, 0.0, 0.4)
ott.assert_almost_equal(mean2DVec[0, 1], 0.0, 0.0, 0.4)

empiricalVar2DVec = sample2DVec.computeVariance()
theoreticalVar2DVec = cov2DVec(ot.Point(2, 0.0))
print("2D vec empirical variance = ", empiricalVar2DVec)
print("2D vec theoretical variance = ", theoreticalVar2DVec)
ott.assert_almost_equal(empiricalVar2DVec[0, 0], theoreticalVar2DVec[0, 0], 0.0, 0.4)
ott.assert_almost_equal(empiricalVar2DVec[1, 1], theoreticalVar2DVec[1, 1], 0.0, 0.4)

# Test 3D with vector-valued output (exercises TBB-parallelized fftND)
print("\n" + "=" * 60)
print("Test 3D CirculantEmbeddingGaussianProcess (vector output)")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov3DVec = ot.ExponentialModel([1.0] * 3, [1.0, 0.5])
interval3DVec = ot.Interval([0.0] * 3, [2.0] * 3)
process3DVec = otexp.CirculantEmbeddingGaussianProcess(cov3DVec, interval3DVec, [5, 5, 5])
print("process 3D vec = ", process3DVec)
print("output dimension = ", process3DVec.getOutputDimension())

realization3DVec = process3DVec.getRealization()
print("3D vec output dimension = ", realization3DVec.getOutputDimension())
print("3D vec vertices = ", realization3DVec.getMesh().getVerticesNumber())
ott.assert_almost_equal(realization3DVec.getMesh().getVerticesNumber(), 6 * 6 * 6, 0.0, 0.0)

sample3DVec = process3DVec.getSample(30)
mean3DVec = sample3DVec.computeMean()
print("3D vec empirical mean = ", mean3DVec)
ott.assert_almost_equal(mean3DVec[0, 0], 0.0, 0.0, 0.5)
ott.assert_almost_equal(mean3DVec[0, 1], 0.0, 0.0, 0.5)

empiricalVar3DVec = sample3DVec.computeVariance()
theoreticalVar3DVec = cov3DVec(ot.Point(3, 0.0))
print("3D vec empirical variance = ", empiricalVar3DVec)
print("3D vec theoretical variance = ", theoreticalVar3DVec)
ott.assert_almost_equal(empiricalVar3DVec[0, 0], theoreticalVar3DVec[0, 0], 0.0, 0.5)
ott.assert_almost_equal(empiricalVar3DVec[1, 1], theoreticalVar3DVec[1, 1], 0.0, 0.5)

print("\nAll tests passed!")
