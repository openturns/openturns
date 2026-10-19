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
        ott.assert_almost_equal(sum(weights), 1.0)

# reference nodes/weights on [-1, 1] from #3149
# nodes are stored in increasing order, computed nodes are decreasing
ref_nodes_weights = {
    otexp.FejerExperiment.CLENSHAWCURTIS: {
        2: ([-1.0, 1.0], [0.5, 0.5]),
        3: ([-1.0, 0.0, 1.0], [1.0 / 6.0, 2.0 / 3.0, 1.0 / 6.0]),
        4: (
            [-1.0, -0.5, 0.5, 1.0],
            [1.0 / 18.0, 4.0 / 9.0, 4.0 / 9.0, 1.0 / 18.0],
        ),
        5: (
            [-1.0, -0.7071067811865475, 0.0, 0.7071067811865475, 1.0],
            [1.0 / 30.0, 4.0 / 15.0, 2.0 / 5.0, 4.0 / 15.0, 1.0 / 30.0],
        ),
    },
    otexp.FejerExperiment.FEJERTYPE1: {
        2: (
            [-0.7071067811865475, 0.7071067811865475],
            [0.5, 0.5],
        ),
        3: (
            [-0.8660254037844387, 0.0, 0.8660254037844388],
            [2.0 / 9.0, 5.0 / 9.0, 2.0 / 9.0],
        ),
        4: (
            [-0.9238795325112867, -0.3826834323650897, 0.3826834323650898, 0.9238795325112867],
            [0.1321488698022421, 0.3678511301977579, 0.3678511301977579, 0.1321488698022421],
        ),
        5: (
            [-0.9510565162951535, -0.587785252292473, 0.0, 0.5877852522924731, 0.9510565162951536],
            [0.0838906142333418, 0.2627760524333249, 0.3066666666666666, 0.2627760524333249, 0.0838906142333418],
        ),
    },
    otexp.FejerExperiment.FEJERTYPE2: {
        2: ([-0.5, 0.5], [0.5, 0.5]),
        3: (
            [-0.7071067811865475, 0.0, 0.7071067811865475],
            [1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0],
        ),
        4: (
            [-0.8090169943749473, -0.3090169943749473, 0.3090169943749475, 0.8090169943749475],
            [0.2127322003750036, 0.2872677996249965, 0.2872677996249965, 0.2127322003750036],
        ),
        5: (
            [-0.8660254037844387, -0.5, 0.0, 0.5, 0.8660254037844388],
            [0.1555555555555555, 0.2, 0.2888888888888889, 0.2, 0.1555555555555555],
        ),
    },
}

for rule, ref_by_n in ref_nodes_weights.items():
    for n, (ref_nodes, ref_weights) in ref_by_n.items():
        experiment = otexp.FejerExperiment([n], rule)
        experiment.setBounds(ot.Interval([-1.0], [1.0]))
        nodes, weights = experiment.generateWithWeights()
        # sort computed pairs by node to match increasing reference order
        pairs = sorted(zip(nodes.asPoint(), weights))
        sorted_nodes = [p[0] for p in pairs]
        sorted_weights = [p[1] for p in pairs]
        ott.assert_almost_equal(sorted_nodes, ref_nodes)
        ott.assert_almost_equal(sorted_weights, ref_weights)

# check integration against known integral (#3149)
g = ot.SymbolicFunction(["x"], ["abs(x - 0.3)"])
interval = ot.Interval([-1.0], [1.0])
exact = 0.545
for rule in all_rules:
    experiment = otexp.FejerExperiment([100], rule)
    experiment.setBounds(interval)
    integration = ot.ExperimentIntegration(experiment)
    value = integration.integrate(g)
    ott.assert_almost_equal(value[0], exact, 1e-3, 1e-3)

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
