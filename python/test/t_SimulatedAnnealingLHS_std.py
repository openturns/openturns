#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

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
    lhs, geomProfile, spaceFillingC2)
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
    lhs, linearProfile, spaceFillingPhiP)
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
    initialDesign, distribution, geomProfile, spaceFillingPhiP)
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
