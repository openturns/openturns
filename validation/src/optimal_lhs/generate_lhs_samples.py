#! /usr/bin/env python

import openturns as ot

# Bounds are [0,1]^dimension
dimension = 2

# Size of sample
size = 20

# Factory: lhs generates
lhsDesign = ot.LHSExperiment(ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setRandomShift(False) # centered
lhsDesign.setAlwaysShuffle(True) # randomized


# Number of designs to generate
N = 10

# For each design, evaluation of some criteria
fp = open("space_filling_criteria.val", "w")
for i in range(N):
    design = lhsDesign.generate()
    filename = "design_dim_%d_size_%d_%d_centered.csv"%(dimension, size,i+1)
    design.exportToCSVFile(filename, ";")
    c2 = ot.SpaceFillingC2().evaluate(design)
    phip = ot.SpaceFillingPhiP().evaluate(design)
    mindist = ot.SpaceFillingMinDist().evaluate(design)
    fp.write("design=%s\n"%filename)
    fp.write("c2=%1.13e, phip=%1.13e, mindist=%1.13e\n"%(c2,phip,mindist))

# New test with biggest dimension
dimension = 50
bounds = ot.Interval(dimension)

# Size of sample
size = 100
lhsDesign = ot.LHSExperiment(ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setRandomShift(False) # centered
lhsDesign.setAlwaysShuffle(True) # randomized

# Number of designs to generate
N = 5

# For each design, evaluation of some criteria
for i in range(N):
    design = lhsDesign.generate()
    filename = "design_dim_%d_size_%d_%d_centered.csv"%(dimension, size,i+1)
    design.exportToCSVFile(filename, ";")
    c2 = ot.SpaceFillingC2().evaluate(design)
    phip = ot.SpaceFillingPhiP().evaluate(design)
    mindist = ot.SpaceFillingMinDist().evaluate(design)
    fp.write("design=%s\n"%filename)
    fp.write("c2=%1.13e, phip=%1.13e, mindist=%1.13e\n"%(c2,phip,mindist))
fp.close()
