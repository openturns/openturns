#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Basic 1D test
print("=" * 60)
print("Test P1InterpolatedProcess (1D, scalar output)")
print("=" * 60)

ot.RandomGenerator.SetSeed(0)

amplitude = [1.0]
scale = [1.0]
covarianceModel = ot.ExponentialModel(scale, amplitude)

# Coarse mesh (process mesh)
coarseMesh = ot.RegularGrid(0.0, 0.5, 11)
gaussianProcess = ot.GaussianProcess(covarianceModel, coarseMesh)

# Fine mesh (target mesh)
fineMesh = ot.RegularGrid(0.0, 0.1, 51)
process = otexp.P1InterpolatedProcess(gaussianProcess, fineMesh)

print("process = ", process)
print("output dimension = ", process.getOutputDimension())
print("mesh vertices = ", process.getMesh().getVerticesNumber())
print("compound process = ", process.getCompoundProcess())

# Check dimensions
ott.assert_almost_equal(process.getOutputDimension(), 1)
ott.assert_almost_equal(process.getMesh().getVerticesNumber(), 51)

# Check compound process mesh is the coarse mesh
ott.assert_almost_equal(
    process.getCompoundProcess().getMesh().getVerticesNumber(), 11
)

# Generate a realization
realization = process.getRealization()
print("realization output dimension = ", realization.getOutputDimension())
print("realization size = ", realization.getValues().getSize())

# Check realization has correct size and consistent values
ott.assert_almost_equal(realization.getOutputDimension(), 1)
ott.assert_almost_equal(realization.getValues().getSize(), 51)
ott.assert_almost_equal(realization.getValues()[0].getDimension(), 1)

# Generate a sample
sample = process.getSample(10)
print("sample size = ", sample.getSize())
print("sample output dimension = ", sample.getDimension())
ott.assert_almost_equal(sample.getSize(), 10)
ott.assert_almost_equal(sample.getDimension(), 1)

# Check sample mean is approximately zero
sampleMean = sample.computeMean()
print("sample mean = ", sampleMean)
ott.assert_almost_equal(sampleMean[0, 0], 0.0, 0.0, 0.5)

# Verify the realization values are finite and in a reasonable range
values = realization.getValues()
ott.assert_almost_equal(values.getSize(), 51)
ott.assert_almost_equal(values.getDimension(), 1)

# Test with vector-valued output
print("\n" + "=" * 60)
print("Test P1InterpolatedProcess (1D, vector output)")
print("=" * 60)

ot.RandomGenerator.SetSeed(0)

amplitudeVec = [1.0, 0.5]
covVec = ot.ExponentialModel([1.0], amplitudeVec)
gpVec = ot.GaussianProcess(covVec, coarseMesh)
processVec = otexp.P1InterpolatedProcess(gpVec, fineMesh)

print("process vec = ", processVec)
print("output dimension = ", processVec.getOutputDimension())
ott.assert_almost_equal(processVec.getOutputDimension(), 2)

# Generate a realization
realizationVec = processVec.getRealization()
print("vec realization output dimension = ",
      realizationVec.getOutputDimension())
print("vec realization size = ", realizationVec.getValues().getSize())
ott.assert_almost_equal(realizationVec.getOutputDimension(), 2)
ott.assert_almost_equal(realizationVec.getValues().getSize(), 51)

# Check that both components are present
ott.assert_almost_equal(realizationVec.getValues()[0, 0], 0.0, 0.0, 3.0)
ott.assert_almost_equal(realizationVec.getValues()[0, 1], 0.0, 0.0, 3.0)

# Test setMesh with a different mesh
print("\n" + "=" * 60)
print("Test setMesh")
print("=" * 60)

newMesh = ot.RegularGrid(0.0, 0.2, 26)
process.setMesh(newMesh)
print("new mesh vertices = ", process.getMesh().getVerticesNumber())
ott.assert_almost_equal(process.getMesh().getVerticesNumber(), 26)
realization2 = process.getRealization()
ott.assert_almost_equal(realization2.getValues().getSize(), 26)

# Test setMesh with incompatible mesh dimension
print("\n" + "=" * 60)
print("Test setMesh with incompatible dimension")
print("=" * 60)
badMesh = ot.Mesh([[0.0, 0.0], [1.0, 0.0], [0.0, 1.0]], [[0, 1, 2]])
with ott.assert_raises(TypeError):
    process.setMesh(badMesh)

# Test __str__ and __repr__
print("\n" + "=" * 60)
print("Test string converters")
print("=" * 60)
print("str: ", str(process))
print("repr: ", repr(process))
ott.assert_almost_equal(str(process).find("P1InterpolatedProcess"), 0)

# Test with 2D mesh
print("\n" + "=" * 60)
print("Test P1InterpolatedProcess (2D)")
print("=" * 60)

ot.RandomGenerator.SetSeed(0)
cov2D = ot.ExponentialModel([1.0, 1.0], [1.0])
coarseMesh2D = ot.IntervalMesher([5, 5]).build(
    ot.Interval([0.0, 0.0], [1.0, 1.0])
)
gp2D = ot.GaussianProcess(cov2D, coarseMesh2D)
fineMesh2D = ot.IntervalMesher([10, 10]).build(
    ot.Interval([0.0, 0.0], [1.0, 1.0])
)
process2D = otexp.P1InterpolatedProcess(gp2D, fineMesh2D)
print("2D process = ", process2D)
print("2D output dimension = ", process2D.getOutputDimension())
ott.assert_almost_equal(process2D.getOutputDimension(), 1)
print("2D coarse vertices = ", coarseMesh2D.getVerticesNumber())
print("2D fine vertices = ", fineMesh2D.getVerticesNumber())
realization2D = process2D.getRealization()
print("2D realization size = ", realization2D.getValues().getSize())
ott.assert_almost_equal(
    realization2D.getValues().getSize(), fineMesh2D.getVerticesNumber()
)

# Verify the realization values are finite and have correct dimensions
values2D = realization2D.getValues()
ott.assert_almost_equal(values2D.getSize(), fineMesh2D.getVerticesNumber())
ott.assert_almost_equal(values2D.getDimension(), 1)

# Larger sample to check statistics
ot.RandomGenerator.SetSeed(1)
sample2D = process2D.getSample(200)
mean2D = sample2D.computeMean()
print("2D sample mean = ", mean2D)
ott.assert_almost_equal(mean2D[0, 0], 0.0, 0.0, 0.2)

var2D = sample2D.computeVariance()
theoreticalVar = cov2D(ot.Point(2, 0.0))[0, 0]
print("2D empirical variance = ", var2D[0, 0])
print("2D theoretical variance = ", theoreticalVar)
ott.assert_almost_equal(var2D[0, 0], theoreticalVar, 0.0, 0.3)

print("\nAll tests passed!")
