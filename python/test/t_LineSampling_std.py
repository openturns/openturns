#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import cantilever_beam
from openturns.usecases import stressed_beam
import math

# stressed_beam ##############################################

print(f"-- stressed_beam {'-' * 50}")
threshold = -1e6
sm = stressed_beam.AxialStressedBeam()
limitStateFunction = sm.model
R_dist = sm.distribution_R
F_dist = sm.distribution_F
distribution = sm.distribution
inputRandomVector = ot.RandomVector(distribution)
outputRandomVector = ot.CompositeRandomVector(limitStateFunction, inputRandomVector)
event = ot.ThresholdEvent(outputRandomVector, ot.Less(), threshold)
D = 0.02
G = R_dist - F_dist / (D**2 / 4 * math.pi)
pf_ref = G.computeCDF(threshold)
print(f"exact probability={pf_ref}")

# FORM analysis
optimAlgo = ot.Cobyla()
optimAlgo.setStartingPoint(distribution.getMean())
algoFORM = ot.FORM(optimAlgo, event)
algoFORM.run()
resultFORM = algoFORM.getResult()
alpha = resultFORM.getStandardSpaceDesignPoint()
print(f"alpha={alpha}")

# LineSampling
ot.RandomGenerator.SetSeed(0)
solver = ot.Brent(1e-3, 1e-3, 1e-3, 5)
rootStrategy = ot.MediumSafe(solver)
algo = otexp.LineSampling(event, alpha, rootStrategy)
algo.setMaximumCoefficientOfVariation(1e-3)
algo.setMaximumOuterSampling(1000)
algo.setStoreHistory(True)
algo.run()
result = algo.getResult()
print(str(algo)[:100])
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), pf_ref)
assert algo.getStoreHistory()
assert len(algo.getAlphaHistory()) == algo.getMaximumOuterSampling(), "empty alphas"
assert len(algo.getRootValuesHistory()) > 0, "empty roots"
assert len(algo.getRootPointsHistory()) > 0, "empty roots"
assert algo.getAdaptiveImportantDirection(), "adaptive?"
assert algo.getSearchOppositeDirection(), "opposite?"
assert algo.getInitialAlpha() == alpha, "alpha?"
rootStrategy2 = algo.getRootStrategy()
algo.setRootStrategy(rootStrategy2)

# cantilever_beam ##############################################

print(f"-- cantilever_beam {'-' * 50}")

# model
cb = cantilever_beam.CantileverBeam()
distribution = cb.distribution
model = cb.model
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Greater(), 0.30)

# FORM for alpha
optimAlgo = ot.AbdoRackwitz()
optimAlgo.setStartingPoint(distribution.getMean())
optimAlgo.setMaximumCallsNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-10)
optimAlgo.setMaximumRelativeError(1.0e-10)
optimAlgo.setMaximumResidualError(1.0e-10)
optimAlgo.setMaximumConstraintError(1.0e-10)
algo = ot.FORM(optimAlgo, event)
algo.run()
result = algo.getResult()
alpha = result.getStandardSpaceDesignPoint()
print(f"alpha={alpha}")

# LineSampling
ot.RandomGenerator.SetSeed(0)
solver = ot.Brent(1e-3, 1e-3, 1e-3, 5)
rootStrategy = ot.MediumSafe(solver)
algo = otexp.LineSampling(event, alpha, rootStrategy)
algo.setMaximumOuterSampling(1000)
algo.setMaximumCoefficientOfVariation(5e-2)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 4.57807e-07)

# two branch model ##############################################

print(f"-- 2 branch {'-' * 50}")

X = ot.RandomVector(ot.Normal(2))
g_twoBranch = ot.SymbolicFunction(["x1", "x2"], ["min(5 + 0.1 * (x1 - x2)^2 - (x1 + x2) / sqrt(2), 5 + 0.1 * (x1 - x2)^2 + (x1 + x2) / sqrt(2))"])
Y_twoBranch = ot.CompositeRandomVector(g_twoBranch, X)
threshold = 1.5
event_twoBranch = ot.ThresholdEvent(Y_twoBranch, ot.Less(), threshold)

g1 = ot.SymbolicFunction(["x1", "x2"], ["5 + 0.1 * (x1 - x2)^2 - (x1 + x2) / sqrt(2)"])
Y1 = ot.CompositeRandomVector(g1, X)
event1 = ot.ThresholdEvent(Y1, ot.Less(), threshold)

g2 = ot.SymbolicFunction(["x1", "x2"], ["5 + 0.1 * (x1 - x2)^2 + (x1 + x2) / sqrt(2)"])
Y2 = ot.CompositeRandomVector(g2, X)
event2 = ot.ThresholdEvent(Y2, ot.Less(), threshold)
unionEvent = ot.UnionEvent([event1, event2])

optimAlgo = ot.Cobyla()
optimAlgo.setStartingPoint(X.getMean())
algo = ot.FORM(optimAlgo, event_twoBranch)
algo.run()
result = algo.getResult()
alpha_twoBranch = result.getStandardSpaceDesignPoint()

# LineSampling / only one branch
ot.RandomGenerator.SetSeed(0)
solver = ot.Brent(1e-3, 1e-3, 1e-3, 5)
rootStrategy = ot.MediumSafe(solver)
algo = otexp.LineSampling(event_twoBranch, alpha_twoBranch, rootStrategy)
algo.setSearchOppositeDirection(False)
algo.setMaximumOuterSampling(1000)
algo.setMaximumCoefficientOfVariation(5e-2)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 1.5e-4, 0.0, 1e-5)

# LineSampling
ot.RandomGenerator.SetSeed(0)
algo.setSearchOppositeDirection(True)
algo.setMaximumOuterSampling(1000)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 3e-4, 0.0, 1e-5)

# LineSampling / system event / no adaptive alpha
algo = otexp.LineSampling(unionEvent, alpha_twoBranch, rootStrategy)
ot.RandomGenerator.SetSeed(0)
algo.setMaximumOuterSampling(1000)
algo.setAdaptiveImportantDirection(False)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 3e-4, 0.0, 1e-5)
ot.ResourceMap.SetAsBool("LineSampling-DefaultAdaptiveImportantDirection", True)


# rosenbrock ##############################################

print(f"-- rosenbrock {'-' * 50}")
g = ot.SymbolicFunction(["x1", "x2"], ["100 * (x2 - x1^2)^2 + (x1 - 1)^2"])
X = ot.RandomVector(ot.Normal(2))
Y = ot.CompositeRandomVector(g, X)
threshold = 0.1
event = ot.ThresholdEvent(Y, ot.Less(), threshold)
###################################################

# FORM for  alpha
optimAlgo = ot.Cobyla()
optimAlgo.setStartingPoint(X.getMean())
algo = ot.FORM(optimAlgo, event)
algo.run()
result = algo.getResult()
alpha = result.getStandardSpaceDesignPoint()

# LineSampling
rootStrategy = ot.SafeAndSlow(ot.Brent(1e-3, 1e-3, 1e-3, 5), 3, 0.1)
ot.RandomGenerator.SetSeed(0)
algo = otexp.LineSampling(event, alpha, rootStrategy)
algo.setMaximumOuterSampling(3000)
algo.setMaximumCoefficientOfVariation(5e-2)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.00110258, 0.0, 1e-4)

# complementary
event = ot.ThresholdEvent(Y, ot.Greater(), threshold)
ot.RandomGenerator.SetSeed(0)
algo = otexp.LineSampling(event, alpha, rootStrategy)
algo.setMaximumOuterSampling(3000)
algo.setMaximumCoefficientOfVariation(5e-2)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.997749, 0.0, 1e-4)

# complementary + opposite
ot.RandomGenerator.SetSeed(0)
algo = otexp.LineSampling(event, alpha, rootStrategy)
algo.setMaximumOuterSampling(3000)
algo.setMaximumCoefficientOfVariation(5e-2)
algo.setSearchOppositeDirection(True)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.997749, 0.0, 1e-4)

# paraboloid ##############################################

print(f"-- paraboloid {'-' * 50}")

# model
paraboloid = ot.SymbolicFunction(['u1', 'u2', 'u3', 'u4', 'u5'], ['- u5 + u1^2 + u2^2 + u3^2 + u4^2'])
b = 3.5
db = 0.02
t = 0.
dim = 5
U = ot.Normal(dim)
X = ot.RandomVector(U)
Y = ot.CompositeRandomVector(paraboloid, X)
E1 = ot.ThresholdEvent(Y, ot.Less(), -b)
E2 = ot.ThresholdEvent(Y, ot.Less(), -(b + db))
E3 = ot.ThresholdEvent(Y, ot.Greater(), -(b + db))
E4 = ot.IntersectionEvent([E1, E3])

# LineSampling, with step < db to find the roots
alpha = [0.0, 0.0, 0.0, 0.0, 1.0]
rootStrategy = ot.SafeAndSlow(ot.Brent(1e-3, 1e-3, 1e-3, 5), 8, 0.01)
ot.RandomGenerator.SetSeed(0)
algo = otexp.LineSampling(E4, alpha, rootStrategy)
algo.setMaximumOuterSampling(2000)
algo.setMaximumCoefficientOfVariation(5e-2)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getProbabilityEstimate(), 2.103779e-07)
