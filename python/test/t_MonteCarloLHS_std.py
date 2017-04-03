#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Defining parameters
dimension = 3
size = 25
# Specifiy if initial LHS is centered or randomized
centered = True
# Build standard LHS algorithm
distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension)
lhs = ot.LHSExperiment(distribution, size)
lhs.setRandomShift(not centered)  # centered
lhs.setAlwaysShuffle(True)  # randomized

# print the object
print("lhs=", lhs)
print("Bounds of uniform distributions=", distribution.getRange())

# Generate design without optimization
design = lhs.generate()
print("design=", design)

# Defining space fillings
spaceFillingPhiP = ot.SpaceFillingPhiP()
spaceFillingC2 = ot.SpaceFillingC2()
spaceFillingMinDist = ot.SpaceFillingMinDist()

# print the criteria on this design
print("PhiP=%f, C2=%f, MinDist=%f" %
      (spaceFillingPhiP.evaluate(design), spaceFillingC2.evaluate(design), spaceFillingMinDist.evaluate(design)))

# --------------------------------------------------#
# ------------ MonteCarlo algorithm  ------------- #
# --------------------------------------------------#

# RandomBruteForce MonteCarlo with N designs
N = 1000

# 1) LHS with C2 optimization
optimalLHSAlgorithmC2 = ot.MonteCarloLHS(lhs, N, spaceFillingC2)
# print lhs
print("optimal lhs=", optimalLHSAlgorithmC2)
design = optimalLHSAlgorithmC2.generate()
print("Best design with MonteCarlo and C2 space filling=", design)
resultC2 = optimalLHSAlgorithmC2.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (resultC2.getC2(), resultC2.getPhiP(), resultC2.getMinDist()))

# 2) LHS with PhiP optimization
optimalLHSAlgorithmPhiP = ot.MonteCarloLHS(lhs, N, spaceFillingPhiP)
print("optimal lhs=", optimalLHSAlgorithmPhiP)
design = optimalLHSAlgorithmPhiP.generate()
print("Best design with MonteCarlo and PhiP space filling=", design)
resultPhiP = optimalLHSAlgorithmPhiP.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (resultPhiP.getC2(), resultPhiP.getPhiP(), resultPhiP.getMinDist()))

# 3) LHS with MinDist optimization
optimalLHSAlgorithmMinDist = ot.MonteCarloLHS(lhs, N, spaceFillingMinDist)
print("optimal lhs=", optimalLHSAlgorithmMinDist)
design = optimalLHSAlgorithmMinDist.generate()
print("Best design with MonteCarlo and MinDist space filling=", design)
resultMinDist = optimalLHSAlgorithmMinDist.getResult()
print("Final criteria: C2=%f, PhiP=%f, MinDist=%f" %
      (resultMinDist.getC2(), resultMinDist.getPhiP(), resultMinDist.getMinDist()))
