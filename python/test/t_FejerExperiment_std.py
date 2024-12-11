#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

all_rules = [
    otexp.FejerExperiment.FEJERTYPE1,
    otexp.FejerExperiment.FEJERTYPE2,
    otexp.FejerExperiment.CLENSHAWCURTIS,
]

for rule in all_rules:
    experiment = otexp.FejerExperiment([3], rule)
    experiment.setBounds(ot.Interval(4.0, 5.0))
    nodes, weights = experiment.generateWithWeights()
    print(rule, nodes, weights)
    assert len(nodes) == len(weights)
    assert not experiment.hasUniformWeights()
    ott.assert_almost_equal(nodes.computeMean(), [4.5])
    ott.assert_almost_equal(sum(weights), 2.0)
