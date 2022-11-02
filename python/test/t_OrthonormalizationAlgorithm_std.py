#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


distribution = ot.Triangular(-1.0, 0.3, 1.0)
algo = ot.OrthonormalizationAlgorithm(distribution)
print("algo=", algo)
print("measure=", repr(algo.getMeasure()))
algo.setMeasure(ot.Triangular(-1.0, -0.2, 1.0))
print("new measure=", repr(algo.getMeasure()))
