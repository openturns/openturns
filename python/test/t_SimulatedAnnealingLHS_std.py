#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

# Defining parameters
dimension = 3
bounds = ot.Interval(dimension)
size = 25

# Build standard LHS algorithm
distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension)
distribution.setDescription(['U'+str(i) for i in range(dimension)])
lhs = ot.LHSExperiment(distribution, size)
lhs.setRandomShift(False)  # centered
lhs.setAlwaysShuffle(True)  # randomized

# print the object
print("lhs=", lhs)
print("Bounds of uniform distributions=", distribution.getRange())

# Generate design without optimization
design = lhs.generate()
print("design=", design)

# Defining space fillings
spaceFillingC2 = ot.SpaceFillingC2()
spaceFillingPhiP = ot.SpaceFillingPhiP(10)
spaceFillingMinDist = ot.SpaceFillingMinDist()

# print the criteria on this design
print("PhiP=%f, C2=%f" %
      (ot.SpaceFillingPhiP().evaluate(design), ot.SpaceFillingC2().evaluate(design)))

# --------------------------------------------------#
# ------------- Simulated annealing  ------------- #
# --------------------------------------------------#
# Geometric profile
T0 = 10.0
iMax = 2000
c = 0.95
geomProfile = ot.GeometricProfile(T0, c, iMax)

# 1) Simulated Annealing LHS with geometric temperature profile, C2
# optimization
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    lhs, spaceFillingC2, geomProfile)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using geometric temperature profile & C2 criterion=", design)
result = optimalLHSAlgorithm.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (result.getC2(), result.getPhiP(), result.getMinDist()))

# Linear profile
linearProfile = ot.LinearProfile(T0, iMax)

# 2) Simulated Annealing LHS with linear temperature profile, PhiP optimization
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    lhs, spaceFillingPhiP, linearProfile)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using linear temperature profile & PhiP criterion =", design)
result = optimalLHSAlgorithm.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (result.getC2(), result.getPhiP(), result.getMinDist()))

# 3) Simulated Annealing LHS with geometric temperature profile, PhiP
# optimization & initial design
initialDesign = ot.Sample(design)
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    initialDesign, distribution, spaceFillingPhiP, geomProfile)
print("lhs=", optimalLHSAlgorithm)
print("initial design=", initialDesign)
print("PhiP=%f, C2=%f" %
      (ot.SpaceFillingPhiP().evaluate(design), ot.SpaceFillingC2().evaluate(design)))
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using linear temperature profile & PhiP criterion =", design)
result = optimalLHSAlgorithm.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (result.getC2(), result.getPhiP(), result.getMinDist()))
# 4) Simulated Annealing LHS with linear temperature profile, PhiP
# optimization and nStart > 1
nStart = 10
design = optimalLHSAlgorithm.generateWithRestart(nStart)
print(
    "Generating design using linear temperature profile & PhiP criterion =", design)
results = optimalLHSAlgorithm.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (results.getC2(), results.getPhiP(), results.getMinDist()))
for i in range(nStart):
    design = results.getOptimalDesign(i)
    print("  Intermediate design for restart iteration number ", i, design)
    print("  Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
          (results.getC2(i), results.getPhiP(i), results.getMinDist(i)))

# 5) Fix https://github.com/openturns/openturns/issues/1826
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    initialDesign, distribution, spaceFillingMinDist, geomProfile)
design = optimalLHSAlgorithm.generate()
result = optimalLHSAlgorithm.getResult()
# optim ok
# Final MinDist is >= initial one
optimal_design = [[0.58, 0.3,  0.18], [0.9,  0.38, 0.3], [0.38, 0.74, 0.66], [0.06, 0.5,  0.26], [0.34, 0.98, 0.5],
                  [0.26, 0.7,  0.14], [0.74, 0.34, 0.62], [
                      0.82, 0.66, 0.58], [0.78, 0.06, 0.1], [0.54, 0.94, 0.06],
                  [0.46, 0.22, 0.54], [0.1, 0.78, 0.46], [0.94, 0.02, 0.82], [
    0.5, 0.26, 0.86], [0.42, 0.58, 0.42],
    [0.22, 0.18, 0.34], [0.18, 0.54, 0.94], [0.14, 0.14, 0.74], [
    0.7,  0.46, 0.98], [0.66, 0.86, 0.78],
    [0.98, 0.1,  0.38], [0.62, 0.62, 0.02], [0.3,  0.42, 0.7], [0.02, 0.9,  0.9], [0.86, 0.82, 0.22]]

assert result.getMinDist() >= ot.SpaceFillingMinDist().evaluate(initialDesign)
assert result.getC2() <= ot.SpaceFillingMinDist().evaluate(initialDesign)
assert result.getPhiP() <= ot.SpaceFillingPhiP().evaluate(initialDesign)
ott.assert_almost_equal(result.getC2(), 0.05473015652160929)
ott.assert_almost_equal(result.getPhiP(), 3.517772966753692)
ott.assert_almost_equal(result.getMinDist(), 0.29120439557122074)
ott.assert_almost_equal(result.getOptimalDesign(), optimal_design)
