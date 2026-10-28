#! /usr/bin/env python

import io
import os
import pickle
import tempfile

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(20261028)

# Reference network on a tree: two overlapping bivariate normal factors.
graph = ot.BipartiteGraph([[0, 1], [1, 2]])
cov1 = ot.CovarianceMatrix(2)
cov1[0, 0] = 1.0
cov1[1, 1] = 1.5
cov1[0, 1] = 0.45
cov2 = ot.CovarianceMatrix(2)
cov2[0, 0] = 1.5
cov2[1, 1] = 0.8
cov2[0, 1] = -0.35
reference = ot.CumulativeDistributionNetwork(
    [ot.Normal([0.2, -0.3], cov1), ot.Normal([-0.4, 0.5], cov2)], graph
)

# Construction and accessors.
factory = otexp.CumulativeDistributionNetworkFactory(graph, [ot.Normal(2)] * 2)
assert factory.getGraph().getSize() == 2
assert len(factory.getDistributionCollection()) == 2
assert factory.getParameterDimension() == 10
assert factory.getStartingPoint().getSize() == 0
assert factory.getOptimizationBounds().getDimension() == 0
assert "class=TNC" in repr(factory.getOptimizationAlgorithm())

# Build from parameters and from the templates.
parameter = reference.getParameter()
from_parameter = factory.build(parameter)
assert from_parameter.getDimension() == 3
ott.assert_almost_equal(from_parameter.getParameter(), parameter, 1.0e-12, 0.0)
point = [0.1, 0.2, 0.3]
ott.assert_almost_equal(
    from_parameter.computeLogPDF(point), reference.computeLogPDF(point), 1.0e-12, 0.0
)
default = factory.build()
assert default.getDimension() == 3
assert default.getParameter().getSize() == factory.getParameterDimension()

# Estimate the factors from a sample. The graph has two independent blocks,
# so the density factorises and the factor parameters are identifiable.
independent_graph = ot.BipartiteGraph([[0, 1], [2, 3]])
independent_reference = ot.CumulativeDistributionNetwork(
    [ot.Normal([0.2, -0.3], cov1), ot.Normal([-0.4, 0.5], cov2)], independent_graph
)
sample = independent_reference.getSample(500)
test = independent_reference.getSample(300)
independent_factory = otexp.CumulativeDistributionNetworkFactory(
    independent_graph, [ot.Normal(2)] * 2
)
estimated = independent_factory.build(sample)
assert estimated.getDimension() == 4
assert (
    ot.Distribution(estimated).getImplementation().getClassName()
    == "CumulativeDistributionNetwork"
)
# The estimated joint law must be close to the reference one
log_pdf_reference = independent_reference.computeLogPDF(test)
log_pdf_estimated = estimated.computeLogPDF(test)
kl = (
    sum(
        log_pdf_reference[i, 0] - log_pdf_estimated[i, 0] for i in range(test.getSize())
    )
    / test.getSize()
)
assert abs(kl) < 0.05, "KL=%g" % kl
# The factor parameters are recovered
estimated_parameter = estimated.getParameter()
independent_parameter = independent_reference.getParameter()
for i in range(estimated_parameter.getSize()):
    assert (
        abs(estimated_parameter[i] - independent_parameter[i]) < 0.3
    ), "parameter %d: %g instead of %g" % (
        i,
        estimated_parameter[i],
        independent_parameter[i],
    )

# Starting point and bounds accessors.
starting_point = ot.Point(factory.getParameterDimension(), 0.5)
factory.setStartingPoint(starting_point)
ott.assert_almost_equal(factory.getStartingPoint(), starting_point, 1.0e-12, 0.0)
bounds = ot.Interval([-10.0] * 10, [10.0] * 10)
factory.setOptimizationBounds(bounds)
assert factory.getOptimizationBounds() == bounds
solver = ot.TNC()
solver.setMaximumCallsNumber(100)
factory.setOptimizationAlgorithm(solver)
assert "class=TNC" in repr(factory.getOptimizationAlgorithm())
# The bounds are consistent with the starting point
with ott.assert_raises(TypeError):
    factory.setStartingPoint([0.0] * 3)
with ott.assert_raises(TypeError):
    factory.setOptimizationBounds(ot.Interval([0.0] * 3, [1.0] * 3))
# Inconsistent graph and distributions
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(
        ot.BipartiteGraph([[0, 1, 2]]), [ot.Normal(2)]
    )
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(graph, [ot.Normal(2)])
# Invalid build arguments
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0, 2.0]] * 5))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample(0, 3))
with ott.assert_raises(TypeError):
    factory.build([0.0] * 3)

# Parameter estimation of a single factor network, and bootstrap estimator.
single_graph = ot.BipartiteGraph([[0]])
single_factory = otexp.CumulativeDistributionNetworkFactory(
    single_graph, [ot.Normal(1)]
)
single_reference = ot.CumulativeDistributionNetwork([ot.Normal(0.0, 1.0)], single_graph)
single_sample = single_reference.getSample(300)
single_estimated = single_factory.build(single_sample)
assert single_estimated.getDimension() == 1
single_factory.setBootstrapSize(5)
result = single_factory.buildEstimator(single_sample)
assert result.getDistribution().getDimension() == 1
assert result.getParameterDistribution().getDimension() == 2

# Persistence: all the attributes must be restored.
factory_path = tempfile.mktemp(suffix=".xml")
buffer = io.BytesIO()
pickle.dump(factory, buffer)
buffer.seek(0)
loaded = pickle.load(buffer)
assert loaded.getGraph().getSize() == factory.getGraph().getSize()
assert loaded.getDistributionCollection().getSize() == 2
assert loaded.getParameterDimension() == 10
ott.assert_almost_equal(loaded.getStartingPoint(), starting_point, 1.0e-12, 0.0)
assert loaded.getOptimizationBounds() == bounds
assert "class=TNC" in repr(loaded.getOptimizationAlgorithm())
ott.assert_almost_equal(loaded.build(parameter).getParameter(), parameter, 1.0e-12, 0.0)

if ot.PlatformInfo.HasFeature("libxml2"):
    try:
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(factory_path))
        study.add("factory", factory)
        study.save()
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(factory_path))
        study.load()
        xml_loaded = otexp.CumulativeDistributionNetworkFactory()
        study.fillObject("factory", xml_loaded)
        assert xml_loaded.getGraph().getSize() == 2
        assert xml_loaded.getDistributionCollection().getSize() == 2
        ott.assert_almost_equal(
            xml_loaded.getStartingPoint(), starting_point, 1.0e-12, 0.0
        )
        assert xml_loaded.getOptimizationBounds() == bounds
        ott.assert_almost_equal(
            xml_loaded.build(parameter).getParameter(), parameter, 1.0e-12, 0.0
        )

        # Simulate an archive written before the optimization attributes existed:
        # the missing members must fall back to their defaults.
        with open(factory_path) as f:
            lines = f.readlines()
        removed = ("solver_", "optimizationBounds_", "startingPoint_")
        lines = [
            line
            for line in lines
            if not any('member="%s"' % name in line for name in removed)
        ]
        with open(factory_path, "w") as f:
            f.writelines(lines)
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(factory_path))
        study.load()
        legacy = otexp.CumulativeDistributionNetworkFactory()
        study.fillObject("factory", legacy)
        assert legacy.getGraph().getSize() == 2
        assert legacy.getDistributionCollection().getSize() == 2
        assert legacy.getStartingPoint().getSize() == 0
        assert legacy.getOptimizationBounds().getDimension() == 0
        assert "class=TNC" in repr(legacy.getOptimizationAlgorithm())
        assert "maximumCallsNumber=%d" % ot.ResourceMap.GetAsUnsignedInteger(
            "MaximumLikelihoodFactory-MaximumCallsNumber"
        ) in repr(legacy.getOptimizationAlgorithm())
        ott.assert_almost_equal(
            legacy.build(parameter).getParameter(), parameter, 1.0e-12, 0.0
        )
    finally:
        if os.path.exists(factory_path):
            os.remove(factory_path)
