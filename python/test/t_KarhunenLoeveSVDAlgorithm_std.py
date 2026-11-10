#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

# First eigenvalues of each configuration, captured with the default seed.
# The key is (RandomSVDVariant, configuration index), the last entry of each
# variant is the final 2D test. Randomized variants are seeded, hence
# deterministic, but only checked with a loose tolerance.
referenceFirstEigenvalues = {
    ("", "cfg0"): 1.4991310843259305,
    ("", "cfg1"): 0.9382191799579767,
    ("", "cfg2"): 0.8367021089284625,
    ("", "cfg3"): 0.8367021089284625,
    ("", "cfg4"): 9.26418741154587,
    ("", "final2d"): 4.814227320229137,
    ("Halko2010", "cfg0"): 2.443400210311546,
    ("Halko2010", "cfg1"): 0.4140889665127459,
    ("Halko2010", "cfg2"): 0.5830206729963986,
    ("Halko2010", "cfg3"): 0.5830206729963986,
    ("Halko2010", "cfg4"): 10.45192227950802,
    ("Halko2010", "final2d"): 3.893187186289617,
    ("Halko2011", "cfg0"): 1.66281026335687,
    ("Halko2011", "cfg1"): 1.1697552433996439,
    ("Halko2011", "cfg2"): 1.2434622114002503,
    ("Halko2011", "cfg3"): 1.2434622114002503,
    ("Halko2011", "cfg4"): 3.5566190725328326,
    ("Halko2011", "final2d"): 6.577491541800087,
}


def checkKarhunenLoeveResult(result, sample, referenceEigenvalue):
    lambd = result.getEigenvalues()
    nModes = len(lambd)
    assert nModes > 0
    assert lambd[0] > 0.0
    for i in range(nModes):
        assert lambd[i] >= 0.0
        if i > 0:
            assert lambd[i] <= lambd[i - 1]
    ott.assert_almost_equal(lambd[0], referenceEigenvalue, 1e-6, 0.0)
    KLFunctions = result.getModes()
    assert len(KLFunctions) == nModes
    KLModes = result.getModesAsProcessSample()
    assert KLModes.getSize() == nModes
    assert KLModes.getDimension() == sample.getDimension()
    coefficients = result.project(sample)
    assert coefficients.getSize() == sample.getSize()
    assert coefficients.getDimension() == nModes
    lift = result.lift(coefficients[0])
    assert lift.getOutputDimension() == sample.getDimension()
    liftField = result.liftAsField(coefficients[0])
    assert liftField.getSize() == sample.getMesh().getVerticesNumber()
    assert liftField.getDimension() == sample.getDimension()


for UseRandomSVD, RandomSVDVariant in [
    (False, ""),
    (True, "Halko2010"),
    (True, "Halko2011"),
]:
    ot.ResourceMap.SetAsBool("KarhunenLoeveSVDAlgorithm-UseRandomSVD", UseRandomSVD)
    ot.ResourceMap.SetAsString(
        "KarhunenLoeveSVDAlgorithm-RandomSVDVariant", RandomSVDVariant
    )

    mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
    # 1D mesh, 1D covariance, uniform weight, automatic centering, more samples
    # than vertices
    cov1D = ot.AbsoluteExponential([1.0])
    sample = ot.GaussianProcess(cov1D, mesh).getSample(16)
    algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
    algo.run()
    checkKarhunenLoeveResult(
        algo.getResult(), sample, referenceFirstEigenvalues[(RandomSVDVariant, "cfg0")]
    )

    # 1D mesh, 1D covariance, uniform weight, automatic centering
    sample = ot.GaussianProcess(cov1D, mesh).getSample(6)
    algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
    algo.run()
    checkKarhunenLoeveResult(
        algo.getResult(), sample, referenceFirstEigenvalues[(RandomSVDVariant, "cfg1")]
    )

    # 1D mesh, 1D covariance, uniform weight, declared centered
    algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0, True)
    algo.run()
    checkKarhunenLoeveResult(
        algo.getResult(), sample, referenceFirstEigenvalues[(RandomSVDVariant, "cfg2")]
    )

    # 1D mesh, 1D covariance, nonuniform weight, automatic centering
    weights = mesh.computeWeights()
    algo = ot.KarhunenLoeveSVDAlgorithm(sample, weights, 0.0, True)
    algo.run()
    checkKarhunenLoeveResult(
        algo.getResult(), sample, referenceFirstEigenvalues[(RandomSVDVariant, "cfg3")]
    )

    # 1D mesh, 1D covariance, uniform weight, automatic centering
    R = ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0])
    scale = [1.0]
    amplitude = [1.0, 2.0]
    cov2D = ot.ExponentialModel(scale, amplitude, R)
    sample = ot.GaussianProcess(cov2D, mesh).getSample(6)
    algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
    algo.run()
    checkKarhunenLoeveResult(
        algo.getResult(), sample, referenceFirstEigenvalues[(RandomSVDVariant, "cfg4")]
    )

    # truncation test

    def func(tau, theta):
        return m.sin(2 * m.pi * (tau - theta)) + 1.0

    mesh_size = 10
    mesh = ot.RegularGrid(0.0, 1.0 / (mesh_size - 1.0), mesh_size)
    samples = ot.ProcessSample(mesh, 100, 1)
    for i in range(samples.getSize()):
        samples[i] = [[func(tau[0], i)] for tau in mesh.getVertices()]
    algo = ot.KarhunenLoeveSVDAlgorithm(samples, 0.0, True)
    algo.run()
    result = algo.getResult()
    evs = result.getEigenvalues()
    ott.assert_almost_equal(len(evs), 10)

    # 2d test
    mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
    cov2D = ot.ExponentialModel([1.0], [1.0, 2.0], ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0]))
    sample = ot.GaussianProcess(cov2D, mesh).getSample(16)
    algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
    algo.run()
    result = algo.getResult()
    checkKarhunenLoeveResult(
        result, sample, referenceFirstEigenvalues[(RandomSVDVariant, "final2d")]
    )
