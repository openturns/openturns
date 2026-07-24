#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import os
import tempfile

ot.TESTPREAMBLE()

# ---------------------------------------------------------------------------
# Error cases: constructor validation
# ---------------------------------------------------------------------------
print("=" * 60)
print("Test constructor error cases")
print("=" * 60)

# Dimension mismatch: interval is 1D, covariance is 2D
with ott.assert_raises(Exception):
    otexp.CirculantEmbeddingGaussianProcess(
        ot.ExponentialModel([1.0, 1.0], [1.0]),
        ot.Interval(0.0, 10.0),
        [50],
    )

# Discretization size mismatch: 1D interval, 2D discretization
with ott.assert_raises(Exception):
    otexp.CirculantEmbeddingGaussianProcess(
        ot.ExponentialModel([1.0], [1.0]),
        ot.Interval(0.0, 10.0),
        [50, 50],
    )

# Discretization component < 1
with ott.assert_raises(Exception):
    otexp.CirculantEmbeddingGaussianProcess(
        ot.ExponentialModel([1.0], [1.0]),
        ot.Interval(0.0, 10.0),
        [0],
    )

# Circular size not a power of two
with ott.assert_raises(Exception):
    p = otexp.CirculantEmbeddingGaussianProcess(
        ot.ExponentialModel([1.0], [1.0]),
        ot.Interval(0.0, 10.0),
        [50],
    )
    p.setCircularSize([100])
    p.getRealization()

# Circular size too small
with ott.assert_raises(Exception):
    p = otexp.CirculantEmbeddingGaussianProcess(
        ot.ExponentialModel([1.0], [1.0]),
        ot.Interval(0.0, 10.0),
        [50],
    )
    p.setCircularSize([32])
    p.getRealization()

print("Constructor error cases passed!")

# ---------------------------------------------------------------------------
# Basic 1D test with ExponentialModel (scalar output)
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
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

ott.assert_almost_equal(process.getInterval().getDimension(), 1, 0.0, 0.0)
ott.assert_almost_equal(process.getDiscretization()[0], n - 1, 0.0, 0.0)

# Generate a realization
realization = process.getRealization()
print("realization dimension = ", realization.getOutputDimension())
print("realization size = ", realization.getValues().getSize())
ott.assert_almost_equal(realization.getOutputDimension(), 1, 0.0, 0.0)
ott.assert_almost_equal(realization.getValues().getSize(), n, 0.0, 0.0)

# Verify the covariance structure empirically
ot.RandomGenerator.SetSeed(0)
size = 500
sample = process.getSample(size)
ott.assert_almost_equal(sample.getSize(), size, 0.0, 0.0)
ott.assert_almost_equal(sample.getDimension(), 1, 0.0, 0.0)
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
ott.assert_almost_equal(realization2.getOutputDimension(), 1, 0.0, 0.0)
ott.assert_almost_equal(realization2.getMesh().getVerticesNumber(), n, 0.0, 0.0)

# Test get/set circular size
print("\ncircular size = ", process.getCircularSize())
process.setCircularSize([256])
print("circular size after set = ", process.getCircularSize())
ott.assert_almost_equal(process.getCircularSize()[0], 256, 0.0, 0.0)
realization3 = process.getRealization()

# Test FFT algorithm accessor
fftAlgo = process.getFFTAlgorithm()
print("FFT algorithm = ", fftAlgo)
ott.assert_almost_equal(
    fftAlgo.__class__.__name__ == "KissFFT" or "FFT" in fftAlgo.__class__.__name__,
    True,
    0.0,
    0.0,
)

# Test setFFTAlgorithm
print("\nTest setFFTAlgorithm")
newFFT = ot.KissFFT()
process.setFFTAlgorithm(newFFT)
sameFFT = process.getFFTAlgorithm()
print("FFT after set = ", sameFFT)

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
ott.assert_almost_equal(field.getMesh().getVerticesNumber(), n)
ott.assert_almost_equal(field.getOutputDimension(), 1, 0.0, 0.0)

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
ott.assert_almost_equal(realization5.getOutputDimension(), 1, 0.0, 0.0)

# Larger sample to verify variance
ot.RandomGenerator.SetSeed(1)
sample2 = process4.getSample(200)
empiricalVar2 = sample2.computeVariance()
theoreticalVar2 = cov3(ot.Point(1, 0.0))[0, 0]
print("empirical variance = ", empiricalVar2[0, 0], "theoretical = ", theoreticalVar2)
ott.assert_almost_equal(empiricalVar2[0, 0], theoreticalVar2, 0.0, 0.2)

# Test isStationary / isNormal
ott.assert_almost_equal(process.isStationary(), True, 0.0, 0.0)
ott.assert_almost_equal(process.isNormal(), True, 0.0, 0.0)

# ---------------------------------------------------------------------------
# Serialization (save/load)
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test save/load serialization")
print("=" * 60)
ot.RandomGenerator.SetSeed(42)

with tempfile.TemporaryDirectory() as tmpdir:
    filename = os.path.join(tmpdir, "process.xml")

    study = ot.Study(filename)
    study.setStorageManager(ot.XMLStorageManager(filename))
    study.add("process", process)
    study.save()

    study2 = ot.Study(filename)
    study2.setStorageManager(ot.XMLStorageManager(filename))
    study2.load()
    processLoaded = study2.get("process")

ott.assert_almost_equal(
    processLoaded.getInterval().getDimension(),
    process.getInterval().getDimension(),
    0.0,
    0.0,
)
ott.assert_almost_equal(
    processLoaded.getDiscretization()[0],
    process.getDiscretization()[0],
    0.0,
    0.0,
)
ott.assert_almost_equal(processLoaded.isStationary(), process.isStationary(), 0.0, 0.0)
ott.assert_almost_equal(processLoaded.isNormal(), process.isNormal(), 0.0, 0.0)
print("Serialization passed!")

# ---------------------------------------------------------------------------
# Test with non-power-of-2 discretization (auto-doubling path)
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test non-power-of-2 discretization")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
covNP = ot.ExponentialModel([1.0], [1.0])
processNP = otexp.CirculantEmbeddingGaussianProcess(covNP, ot.Interval(0.0, 10.0), [33])
ott.assert_almost_equal(processNP.getDiscretization()[0], 33, 0.0, 0.0)
realizationNP = processNP.getRealization()
ott.assert_almost_equal(realizationNP.getMesh().getVerticesNumber(), 34, 0.0, 0.0)
ott.assert_almost_equal(processNP.getCircularSize()[0] >= 2 * 33, True, 0.0, 0.0)
print("Non-power-of-2 discretization passed!")

# ---------------------------------------------------------------------------
# 1D with vector-valued output
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test 1D CirculantEmbeddingGaussianProcess (vector output)")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
amplitudeVec = [1.0, 0.5]
covVec = ot.ExponentialModel([1.0], amplitudeVec)
processVec = otexp.CirculantEmbeddingGaussianProcess(covVec, interval, [50])
print("process vec = ", processVec)
print("output dimension = ", processVec.getOutputDimension())
ott.assert_almost_equal(processVec.getOutputDimension(), 2, 0.0, 0.0)

# Generate realization
realizationVec = processVec.getRealization()
print("realization output dimension = ", realizationVec.getOutputDimension())
ott.assert_almost_equal(realizationVec.getOutputDimension(), 2, 0.0, 0.0)
ott.assert_almost_equal(realizationVec.getValues().getDimension(), 2, 0.0, 0.0)

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

# ---------------------------------------------------------------------------
# 2D
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test 2D CirculantEmbeddingGaussianProcess")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov2D = ot.ExponentialModel([1.0, 1.0], [1.0])
interval2D = ot.Interval([0.0, 0.0], [5.0, 5.0])
discretization2D = [10, 8]
process2D = otexp.CirculantEmbeddingGaussianProcess(cov2D, interval2D, discretization2D)
print("process 2D = ", process2D)
ott.assert_almost_equal(process2D.isStationary(), True, 0.0, 0.0)
ott.assert_almost_equal(process2D.isNormal(), True, 0.0, 0.0)
ott.assert_almost_equal(process2D.getInterval().getDimension(), 2, 0.0, 0.0)
ott.assert_almost_equal(process2D.getDiscretization()[0], 10, 0.0, 0.0)
ott.assert_almost_equal(process2D.getDiscretization()[1], 8, 0.0, 0.0)

# Generate a 2D realization
realization2D = process2D.getRealization()
print("2D realization output dimension = ", realization2D.getOutputDimension())
ott.assert_almost_equal(realization2D.getOutputDimension(), 1, 0.0, 0.0)
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

# ---------------------------------------------------------------------------
# 3D
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test 3D CirculantEmbeddingGaussianProcess")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov3D = ot.ExponentialModel([1.0, 1.0, 1.0], [1.0])
interval3D = ot.Interval([0.0, 0.0, 0.0], [3.0, 3.0, 3.0])
discretization3D = [6, 6, 6]
process3D = otexp.CirculantEmbeddingGaussianProcess(cov3D, interval3D, discretization3D)
print("process 3D = ", process3D)
ott.assert_almost_equal(process3D.isStationary(), True, 0.0, 0.0)

realization3D = process3D.getRealization()
print("3D realization output dimension = ", realization3D.getOutputDimension())
ott.assert_almost_equal(realization3D.getOutputDimension(), 1, 0.0, 0.0)
ott.assert_almost_equal(realization3D.getMesh().getVerticesNumber(), 7 * 7 * 7, 0.0, 0.0)

# Compute statistics
sample3D = process3D.getSample(50)
mean3D = sample3D.computeMean()
ott.assert_almost_equal(mean3D[0, 0], 0.0, 0.0, 0.4)

empiricalVar3D = sample3D.computeVariance()
theoreticalVar3D = cov3D(ot.Point(3, 0.0))[0, 0]
ott.assert_almost_equal(empiricalVar3D[0, 0], theoreticalVar3D, 0.0, 0.4)

# ---------------------------------------------------------------------------
# 4D
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test 4D CirculantEmbeddingGaussianProcess")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov4D = ot.ExponentialModel([1.0] * 4, [1.0])
interval4D = ot.Interval([0.0] * 4, [2.0] * 4)
discretization4D = [4] * 4
process4D = otexp.CirculantEmbeddingGaussianProcess(cov4D, interval4D, discretization4D)
print("process 4D = ", process4D)
ott.assert_almost_equal(process4D.isStationary(), True, 0.0, 0.0)
ott.assert_almost_equal(process4D.getInterval().getDimension(), 4, 0.0, 0.0)

realization4D = process4D.getRealization()
nVertices4D = 5**4
ott.assert_almost_equal(realization4D.getOutputDimension(), 1, 0.0, 0.0)
ott.assert_almost_equal(realization4D.getMesh().getVerticesNumber(), nVertices4D, 0.0, 0.0)

# Compute statistics
sample4D = process4D.getSample(30)
mean4D = sample4D.computeMean()
ott.assert_almost_equal(mean4D[0, 0], 0.0, 0.0, 0.5)

empiricalVar4D = sample4D.computeVariance()
theoreticalVar4D = cov4D(ot.Point(4, 0.0))[0, 0]
ott.assert_almost_equal(empiricalVar4D[0, 0], theoreticalVar4D, 0.0, 0.5)

# ---------------------------------------------------------------------------
# 2D with vector-valued output
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test 2D CirculantEmbeddingGaussianProcess (vector output)")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov2DVec = ot.ExponentialModel([1.0, 1.0], [1.0, 0.5])
interval2DVec = ot.Interval([0.0, 0.0], [3.0, 3.0])
process2DVec = otexp.CirculantEmbeddingGaussianProcess(cov2DVec, interval2DVec, [6, 6])
print("process 2D vec = ", process2DVec)
ott.assert_almost_equal(process2DVec.getOutputDimension(), 2, 0.0, 0.0)

realization2DVec = process2DVec.getRealization()
ott.assert_almost_equal(realization2DVec.getOutputDimension(), 2, 0.0, 0.0)
ott.assert_almost_equal(realization2DVec.getMesh().getVerticesNumber(), 7 * 7, 0.0, 0.0)

sample2DVec = process2DVec.getSample(50)
mean2DVec = sample2DVec.computeMean()
ott.assert_almost_equal(mean2DVec[0, 0], 0.0, 0.0, 0.4)
ott.assert_almost_equal(mean2DVec[0, 1], 0.0, 0.0, 0.4)

empiricalVar2DVec = sample2DVec.computeVariance()
theoreticalVar2DVec = cov2DVec(ot.Point(2, 0.0))
ott.assert_almost_equal(empiricalVar2DVec[0, 0], theoreticalVar2DVec[0, 0], 0.0, 0.4)
ott.assert_almost_equal(empiricalVar2DVec[1, 1], theoreticalVar2DVec[1, 1], 0.0, 0.4)

# ---------------------------------------------------------------------------
# 3D with vector-valued output
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test 3D CirculantEmbeddingGaussianProcess (vector output)")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
cov3DVec = ot.ExponentialModel([1.0] * 3, [1.0, 0.5])
interval3DVec = ot.Interval([0.0] * 3, [2.0] * 3)
process3DVec = otexp.CirculantEmbeddingGaussianProcess(cov3DVec, interval3DVec, [5, 5, 5])
print("process 3D vec = ", process3DVec)
ott.assert_almost_equal(process3DVec.getOutputDimension(), 2, 0.0, 0.0)

realization3DVec = process3DVec.getRealization()
ott.assert_almost_equal(realization3DVec.getOutputDimension(), 2, 0.0, 0.0)
ott.assert_almost_equal(realization3DVec.getMesh().getVerticesNumber(), 6 * 6 * 6, 0.0, 0.0)

sample3DVec = process3DVec.getSample(30)
mean3DVec = sample3DVec.computeMean()
ott.assert_almost_equal(mean3DVec[0, 0], 0.0, 0.0, 0.5)
ott.assert_almost_equal(mean3DVec[0, 1], 0.0, 0.0, 0.5)

empiricalVar3DVec = sample3DVec.computeVariance()
theoreticalVar3DVec = cov3DVec(ot.Point(3, 0.0))
ott.assert_almost_equal(empiricalVar3DVec[0, 0], theoreticalVar3DVec[0, 0], 0.0, 0.5)
ott.assert_almost_equal(empiricalVar3DVec[1, 1], theoreticalVar3DVec[1, 1], 0.0, 0.5)

# ---------------------------------------------------------------------------
# Test description and __str__
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test description and string conversion")
print("=" * 60)
desc = process.getDescription()
ott.assert_almost_equal(desc.getSize(), 1, 0.0, 0.0)
ott.assert_almost_equal(desc[0], "y0", 0.0, 0.0)
str_repr = repr(process)
ott.assert_almost_equal("CirculantEmbeddingGaussianProcess" in str_repr, True, 0.0, 0.0)
str_val = str(process)
ott.assert_almost_equal("CirculantEmbeddingGaussianProcess" in str_val, True, 0.0, 0.0)
print("Description/string conversion passed!")

# ---------------------------------------------------------------------------
# Test auto-doubling convergence path (non-power-of-2, small discretization)
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test auto-doubling convergence")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
processAuto = otexp.CirculantEmbeddingGaussianProcess(
    ot.ExponentialModel([0.5], [1.0]),
    ot.Interval(0.0, 5.0),
    [7],
)
realizationAuto = processAuto.getRealization()
ott.assert_almost_equal(realizationAuto.getMesh().getVerticesNumber(), 8, 0.0, 0.0)
ott.assert_almost_equal(processAuto.getCircularSize()[0] >= 16, True, 0.0, 0.0)
print("Auto-doubling convergence passed!")

# ---------------------------------------------------------------------------
# Test with MaternModel vector output
# ---------------------------------------------------------------------------
print("\n" + "=" * 60)
print("Test MaternModel vector output")
print("=" * 60)
ot.RandomGenerator.SetSeed(0)
covMatVec = ot.MaternModel([1.0], [1.0, 0.5], 1.5)
processMatVec = otexp.CirculantEmbeddingGaussianProcess(covMatVec, ot.Interval(0.0, 5.0), [20])
ott.assert_almost_equal(processMatVec.getOutputDimension(), 2, 0.0, 0.0)
realizationMatVec = processMatVec.getRealization()
ott.assert_almost_equal(realizationMatVec.getOutputDimension(), 2, 0.0, 0.0)
ott.assert_almost_equal(realizationMatVec.getMesh().getVerticesNumber(), 21, 0.0, 0.0)
print("MaternModel vector output passed!")

print("\nAll CirculantEmbeddingGaussianProcess tests passed!")
