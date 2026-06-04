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
    for N in [1, 2, 3, 4, 5]:
        experiment = otexp.FejerExperiment([N], rule)
        experiment.setBounds(ot.Interval(4.0, 5.0))
        nodes, weights = experiment.generateWithWeights()
        print(f"---------- rule={rule} N={N} ----------")
        print(rule, nodes, weights)
        assert len(nodes) == N
        assert len(weights) == N
        assert experiment.hasUniformWeights() == (N < 2)
        ott.assert_almost_equal(nodes.computeMean(), [4.5])
        ott.assert_almost_equal(sum(weights), 2.0)

# check nesting
mapN = {otexp.FejerExperiment.FEJERTYPE2: lambda n: 2**n - 1}
mapN[otexp.FejerExperiment.CLENSHAWCURTIS] = lambda n: 2**n + 1
for rule in [otexp.FejerExperiment.FEJERTYPE2, otexp.FejerExperiment.CLENSHAWCURTIS]:
    prev_nodes = None
    for N in [1, 2, 3, 4, 5]:
        experiment = otexp.FejerExperiment([1], rule)
        experiment.setUseNestedLevels(True)
        experiment.setLevels([N])
        nodes, weights = experiment.generateWithWeights()
        print(f"---------- rule={rule} N={N} nested ----------")
        print(rule, nodes, weights)
        Nref = mapN[rule](N)
        assert len(nodes) == Nref
        assert len(weights) == len(nodes)
        # check previous nodes are nested in current nodes
        if prev_nodes is not None:
            tree = ot.KDTree(nodes)
            for x in prev_nodes:
                k = tree.query(x)
                d = ot.Point(x - nodes[k]).norm()
                assert d < 1e-15, f"x={x} xk={nodes[k]} d={d}"
        prev_nodes = nodes
