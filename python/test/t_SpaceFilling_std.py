#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Defining parameters
dimension = 5
size = 100

# Build OT LHS algorithm
lhs = ot.LHSExperiment(
    ot.ComposedDistribution([ot.Uniform()] * dimension), size)

# Generate design without optimization
design = lhs.generate()

# Defining space fillings
spaceFillingC2 = ot.SpaceFillingC2()
spaceFillingMinDist = ot.SpaceFillingMinDist()
spaceFillingPhiP = ot.SpaceFillingPhiP()
spaceFillingPhiP50 = ot.SpaceFillingPhiP(50)

# print the criteria on this design
print("C2=%f MinDist=%f PhiP=%f, PhiP(50)=%f" %
      tuple([sf.evaluate(design) for sf in [spaceFillingC2, spaceFillingMinDist, spaceFillingPhiP, spaceFillingPhiP50]]))
