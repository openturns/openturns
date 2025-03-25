#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ratioOfU = otexp.RatioOfUniforms()
print("ratioOfU is initialized?", ratioOfU.isInitialized())
coll = ratioOfU.initialize()
print("ratioOfU is initialized?", ratioOfU.isInitialized())
print("Multistarts=", coll)
ratioOfU.setCandidateNumber(10)
print("Candidate number=", ratioOfU.getCandidateNumber())
ratioOfU.setOptimizationAlgorithm(ot.TNC())
print("Algo optim=", ratioOfU.getOptimizationAlgorithm())

# We create a distribution
# distribution = ot.Dirichlet([1, 2, 3, 4, 5, 6, 7])
R = ot.CorrelationMatrix(4, [1.0, 0.5, 0.2, 0.1,
                             0.5, 1.0, 0.5, 0.2,
                             0.2, 0.5, 1.0, 0.5,
                             0.1, 0.2, 0.5, 1.0])
distribution = ot.Normal([0.0] * 4, [1.0] * 4, R)
print("distribution = ", repr(distribution))


# Now sample it using RoU
ratioOfU = otexp.RatioOfUniforms(distribution)
print("ratioOfU=", repr(ratioOfU))
print("ratioOfU=", str(ratioOfU))
print("ratioOfU is initialized?", ratioOfU.isInitialized())
print("r=", ratioOfU.getR())
print("Acceptance ratio=%.4g" % ratioOfU.getAcceptanceRatio())
print("c=", ratioOfU.getC())
ratioOfU.setR(2.0)
print("r=", ratioOfU.getR())
print("Acceptance ratio=%.4g" % ratioOfU.getAcceptanceRatio())
print("c=", ratioOfU.getC())
print("One realization", ratioOfU.getRealization())
print("One sample", ratioOfU.getSample(10))
sample, tryNumber = ratioOfU.getSampleWithTryNumber(10)
print("One sample with try number", sample, "try number=", tryNumber)

# RoU based on a non-normalized logPDF
tri = ot.Triangular(0.0, 1.0, 1.0)
ot.RandomGenerator.SetSeed(1234)
logPDF = ot.SymbolicFunction("x", "log(x)")
ratioOfU = otexp.RatioOfUniforms(logPDF, ot.Interval(0, 1))
print("Acceptance ratio=%.4g" % ratioOfU.getAcceptanceRatio())
print("c=", ratioOfU.getC())
sample1 = ratioOfU.getSample(10)
ot.RandomGenerator.SetSeed(1234)
ratioOfU = otexp.RatioOfUniforms(tri)
print("ratioOfU=", ratioOfU)
sample2 = ratioOfU.getSample(10)
ott.assert_almost_equal(sample1, sample2)
