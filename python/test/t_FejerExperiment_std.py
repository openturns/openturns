#! /usr/bin/env python
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

all_methods = [
    ot.FejerAlgorithm.FEJERTYPE1,
    ot.FejerAlgorithm.FEJERTYPE2,
    ot.FejerAlgorithm.CLENSHAWCURTIS,
]

bounds = ot.Interval(4.0, 5.0)
for method in all_methods:
    algo = ot.FejerAlgorithm([3], method)
    experiment = otexp.FejerExperiment(algo, bounds)
    nodes, weights = experiment.generateWithWeights()
    print(method, nodes, weights)
    assert len(nodes) == len(weights)
    ott.assert_almost_equal(nodes.computeMean(), [4.5])
    ott.assert_almost_equal(sum(weights), 2.0)
