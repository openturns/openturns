#!/usr/bin/env python

from __future__ import print_function
import openturns as ot


size = 8
for method in ["MonteCarlo", "LHS", "QMC"]:
    ot.ResourceMap.SetAsString("SobolIndicesExperiment-SamplingMethod", method)
    for dimension in [2, 3]:
        distribution = ot.Normal(dimension)
        for computeSecondOrder in [False, True]:
            experiment = ot.SobolIndicesExperiment(
                distribution, size, computeSecondOrder)
            print('experiment=', experiment)
            ot.RandomGenerator.SetSeed(0)
            sample = experiment.generate()
            print('sample=', sample)
            print('size=', sample.getSize())
