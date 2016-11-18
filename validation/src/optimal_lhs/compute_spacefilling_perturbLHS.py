#! /usr/bin/env python

import openturns as ot

# Bounds are [0,1]^dimension
dimension = 2

# Size of sample
size = 20

# Factory: lhs generates
lhsDesign = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setRandomShift(False) # centered
lhsDesign.setAlwaysShuffle(True) # randomized

# For each design, evaluation of some criteria
fp = open("space_filling_criteria_perturbation.val", "w")
design = lhsDesign.generate()
filename = "design_dim_2_size_20_centered_perturbLHS.csv"
design.exportToCSVFile(filename, ";")
c2 = ot.SpaceFillingC2().evaluate(design)
phip = ot.SpaceFillingPhiP().evaluate(design)
mindist = ot.SpaceFillingMinDist().evaluate(design)
fp.write("initial design=%s\n"%filename)
fp.write("c2=%1.10e, phip=%1.10e, mindist=%1.10e\n"%(c2,phip,mindist))

# Perturbations
# Echange of type:
#tmp = design[row1, column]
#design[row1, column] = design[row2, column]
#design[row2, column] = tmp
for row1 in range(size):
    for row2 in range(size):
        for column in range(dimension):
            # The 3-upplet (row1, row2, column)
            # Criteria exchange design[row1, column] and design[row2, column]
            updated_c2 = ot.SpaceFillingC2().perturbLHS(design, c2, row1, row2, column)
            #updated_phip = ot.SpaceFillingPhiP().perturbLHS(design, phip, row1, row2, column)
            fp.write("row1=%d, row2=%d, column=%d, updated c2=%1.10e\n"%(row1+1,row2+1, column+1, updated_c2))

## New test with biggest dimension
dimension = 50

# Size of sample
size = 100
lhsDesign = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension), size)
lhsDesign.setRandomShift(False) # centered
lhsDesign.setAlwaysShuffle(True) # randomized

design = lhsDesign.generate()
# export the design
filename = "design_dim_50_size_100_centered_perturbLHS.csv"
design.exportToCSVFile(filename, ";")
c2 = ot.SpaceFillingC2().evaluate(design)
phip = ot.SpaceFillingPhiP().evaluate(design)
mindist = ot.SpaceFillingMinDist().evaluate(design)
fp.write("initial design=%s\n"%filename)
fp.write("c2=%1.10e, phip=%1.10e, mindist=%1.10e\n"%(c2,phip,mindist))
for row1 in range(0, size, 10):
    for row2 in range(0, size, 10):
        for column in range(0, dimension, 10):
            # The 3-upplet (row1, row2, column)
            # Criteria exchange design[row1, column] and design[row2, column]
            updated_c2 = ot.SpaceFillingC2().perturbLHS(design, c2, row1, row2, column)
            updated_phip = ot.SpaceFillingPhiP().perturbLHS(design, phip, row1, row2, column)
            fp.write("row1=%d, row2=%d, column=%d, updated c2=%1.10e\n"%(row1+1,row2+1, column+1, updated_c2))

fp.close()
