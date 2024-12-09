#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp

ratioOfU = otexp.RatioOfUniforms()
print("ratioOfU is initialized?", ratioOfU.isInitialized())
ratioOfU.initialize()
print("ratioOfU is initialized?", ratioOfU.isInitialized())
ratioOfU.setCandidateNumber(10)
print("Candidate number=", ratioOfU.getCandidateNumber())
ratioOfU.setOptimizationAlgorithm(ot.TNC())
print("Algo optim=", ratioOfU.getOptimizationAlgorithm())

# We create a distribution
distribution = ot.Dirichlet([1,2,3,4,5,6,7])
print("distribution = ", repr(distribution))

# Now sample it using RoU
ratioOfU = otexp.RatioOfUniforms(distribution)
print("ratioOfU is initialized?", ratioOfU.isInitialized())
print("r=", ratioOfU.getR())
print("Acceptance ratio=", ratioOfU.getAcceptanceRatio())
ratioOfU.setR(2.0)
print("r=", ratioOfU.getR())
print("Acceptance ratio=", ratioOfU.getAcceptanceRatio())
print("One realization", ratioOfU.getRealization())
print("One sample", ratioOfU.getSample(10))
