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

# The exact log-likelihood only supports continuous or integral networks:
# reject a mixed network before the optimization starts.
mixed_factory = otexp.CumulativeDistributionNetworkFactory(
    ot.BipartiteGraph([[0, 1]]),
    [ot.JointDistribution([ot.Normal(), ot.Bernoulli()])],
)
with ott.assert_raises(RuntimeError):
    mixed_factory.build(ot.Sample([[0.0, 1.0], [1.0, 0.0]]))

# Fail fast when the derivative-sum-product enumeration exceeds its limit.
cyclic_factory = otexp.CumulativeDistributionNetworkFactory(
    ot.BipartiteGraph([[0, 1], [1, 2], [0, 2]]),
    [ot.Normal(2), ot.Normal(2), ot.Normal(2)],
)
maximum_terms = ot.ResourceMap.GetAsUnsignedInteger(
    "CumulativeDistributionNetwork-MaximumTerms"
)
ot.ResourceMap.SetAsUnsignedInteger("CumulativeDistributionNetwork-MaximumTerms", 1)
try:
    with ott.assert_raises(RuntimeError):
        cyclic_factory.build(ot.Sample([[0.0, 0.5, 1.0], [1.0, 0.5, 0.0]]))
finally:
    ot.ResourceMap.SetAsUnsignedInteger(
        "CumulativeDistributionNetwork-MaximumTerms", maximum_terms
    )

# Structure learning: recover a block-independent partition. The first two
# variables are correlated, the third one is independent.
correlation = ot.CorrelationMatrix(2)
correlation[1, 0] = 0.8
structure_reference = ot.CumulativeDistributionNetwork(
    [ot.Normal([0.0, 0.0], [1.0, 1.0], correlation), ot.Normal(2.0, 0.5)],
    ot.BipartiteGraph([[0, 1], [2]]),
)
structure_sample = structure_reference.getSample(1000)
structure_factory = otexp.CumulativeDistributionNetworkFactory(1, 3, 2)
assert structure_factory.isStructureLearning()
assert structure_factory.getMinimumFactorNumber() == 1
assert structure_factory.getMaximumFactorNumber() == 3
assert structure_factory.getMaximumFactorDimension() == 2
assert structure_factory.getParameterDimension() == 0
assert len(structure_factory.getDistributionCollection()) == 0
assert "structureLearning=true" in repr(structure_factory)
with ott.assert_raises(RuntimeError):
    structure_factory.getGraph()

structure_estimated = structure_factory.build(structure_sample)
assert structure_estimated.getDimension() == 3
structure_graph = structure_estimated.getImplementation().getGraph()
assert structure_graph.getSize() == 2
assert list(structure_graph[0]) == [0, 1]
assert list(structure_graph[1]) == [2]

# The BIC must improve upon the independent (singleton) partition.
independent_structure_factory = otexp.CumulativeDistributionNetworkFactory(1, 3, 1)
independent_structure_estimated = independent_structure_factory.build(structure_sample)
assert independent_structure_estimated.getImplementation().getGraph().getSize() == 3
bic_structure = ot.FittingTest.BIC(
    structure_sample,
    structure_estimated,
    structure_estimated.getParameterDimension(),
)
bic_independent = ot.FittingTest.BIC(
    structure_sample,
    independent_structure_estimated,
    independent_structure_estimated.getParameterDimension(),
)
assert bic_structure < bic_independent - 0.5, (bic_structure, bic_independent)

# The maximum number of factors is a hard constraint: a single factor of
# dimension 3 is built even though the BIC is not improved.
forced_factory = otexp.CumulativeDistributionNetworkFactory(1, 1, 3)
forced_estimated = forced_factory.build(structure_sample[:300])
forced_graph = forced_estimated.getImplementation().getGraph()
assert forced_graph.getSize() == 1
assert list(forced_graph[0]) == [0, 1, 2]

# The minimum number of factors is also a hard constraint: the two correlated
# variables are kept in separate factors even though merging them improves the
# BIC.
minimum_factory = otexp.CumulativeDistributionNetworkFactory(2, 2, 2)
minimum_estimated = minimum_factory.build(structure_sample.getMarginal([0, 1]))
assert minimum_estimated.getImplementation().getGraph().getSize() == 2

# A one-dimensional sample is a single factor.
univariate_estimated = otexp.CumulativeDistributionNetworkFactory(1, 1, 1).build(
    structure_sample.getMarginal(0)
)
assert univariate_estimated.getDimension() == 1
assert univariate_estimated.getImplementation().getGraph().getSize() == 1

# Invalid bounds and unavailable usages.
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(0, 3, 2)
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(3, 2, 2)
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(1, 3, 0)
with ott.assert_raises(TypeError):
    structure_factory.build()
with ott.assert_raises(TypeError):
    structure_factory.build([0.0] * 7)
with ott.assert_raises(RuntimeError):
    structure_factory.buildEstimator(structure_sample)
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(4, 4, 2).build(structure_sample)
with ott.assert_raises(TypeError):
    otexp.CumulativeDistributionNetworkFactory(1, 1, 2).build(structure_sample)
with ott.assert_raises(TypeError):
    structure_factory.build(ot.Sample(0, 3))
with ott.assert_raises(TypeError):
    structure_factory.setGraph(ot.BipartiteGraph([[0, 1, 2]]))
with ott.assert_raises(TypeError):
    structure_factory.setDistributionCollection([ot.Normal(3)])

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
    finally:
        if os.path.exists(factory_path):
            os.remove(factory_path)

# Persistence of the structure-learning factory.
structure_buffer = io.BytesIO()
pickle.dump(structure_factory, structure_buffer)
structure_buffer.seek(0)
loaded_structure_factory = pickle.load(structure_buffer)
assert loaded_structure_factory.isStructureLearning()
assert loaded_structure_factory.getMinimumFactorNumber() == 1
assert loaded_structure_factory.getMaximumFactorNumber() == 3
assert loaded_structure_factory.getMaximumFactorDimension() == 2
assert loaded_structure_factory.getParameterDimension() == 0
with ott.assert_raises(RuntimeError):
    loaded_structure_factory.getGraph()
loaded_structure_estimated = loaded_structure_factory.build(structure_sample[:200])
assert loaded_structure_estimated.getDimension() == 3

if ot.PlatformInfo.HasFeature("libxml2"):
    structure_path = tempfile.mktemp(suffix=".xml")
    try:
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(structure_path))
        study.add("factory", structure_factory)
        study.save()
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(structure_path))
        study.load()
        xml_loaded = otexp.CumulativeDistributionNetworkFactory()
        study.fillObject("factory", xml_loaded)
        assert xml_loaded.isStructureLearning()
        assert xml_loaded.getMinimumFactorNumber() == 1
        assert xml_loaded.getMaximumFactorNumber() == 3
        assert xml_loaded.getMaximumFactorDimension() == 2
    finally:
        if os.path.exists(structure_path):
            os.remove(structure_path)
