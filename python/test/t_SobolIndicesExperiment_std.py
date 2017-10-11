#!/usr/bin/env python

from __future__ import print_function
import openturns as ot

distribution = ot.Normal(3)
size = 8

for computeSecondOrder in [False, True]:
    experiment = ot.SobolIndicesExperiment(
        distribution, size, computeSecondOrder)
    print('experiment=', experiment)
    sample = experiment.generate()
    print('sample=', sample)
