#!/usr/bin/env python
"""Pointwise KL-PCE sensitivity with analytic scalar-field references."""

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(42)
mesh = ot.RegularGrid(0.0, 0.025, 41)
distribution = ot.Normal(2)
input_sample = distribution.getSample(200)
old_size = ot.ResourceMap.GetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize")
old_sparse = ot.ResourceMap.GetAsBool("FunctionalChaosAlgorithm-Sparse")
old_q = ot.ResourceMap.GetAsScalar("FunctionalChaosAlgorithm-QNorm")


def fit(formulas):
    """Fit a polynomial field using a complete degree-two chaos basis."""
    model = ot.VertexValuePointToFieldFunction(
        ot.SymbolicFunction(["t", "x1", "x2"], formulas), mesh
    )
    algorithm = ot.PointToFieldFunctionalChaosAlgorithm(
        input_sample, model(input_sample), distribution
    )
    algorithm.setThreshold(1e-12)
    algorithm.run()
    return algorithm.getResult()


try:
    ot.ResourceMap.SetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize", 6)
    ot.ResourceMap.SetAsBool("FunctionalChaosAlgorithm-Sparse", False)
    ot.ResourceMap.SetAsScalar("FunctionalChaosAlgorithm-QNorm", 1.0)
    for formula, interaction in [
        ("(1-t)*x1+t*x2", False),
        ("5+(1-t)*x1+t*x2+2*x1*x2", True),
    ]:
        result = fit([formula])
        sensitivity = ot.FieldFunctionalChaosSobolIndices(result)
        first = sensitivity.getPointwiseFirstOrderIndices()
        total = sensitivity.getPointwiseTotalOrderIndices()
        # Compare Mesh objects to avoid RegularGrid subclass equality dispatch.
        assert first.getMesh() == ot.Mesh(mesh)
        assert total.getMesh() == ot.Mesh(mesh)
        assert first.getDimension() == 2
        for vertex in range(mesh.getVerticesNumber()):
            t = mesh.getVertices()[vertex, 0]
            interaction_variance = 4.0 if interaction else 0.0
            variance = (1.0 - t) ** 2 + t**2 + interaction_variance
            ott.assert_almost_equal(
                first.getValues()[vertex],
                [(1.0 - t) ** 2 / variance, t**2 / variance],
                0.0,
                1e-10,
            )
            ott.assert_almost_equal(
                total.getValues()[vertex],
                [
                    ((1.0 - t) ** 2 + interaction_variance) / variance,
                    (t**2 + interaction_variance) / variance,
                ],
                0.0,
                1e-10,
            )
    # Sensitivity ratios must not depend on the magnitude of chaos coefficients.
    reference = ot.FieldFunctionalChaosSobolIndices(result)
    chaos = result.getFCEResult()
    for scale in [1e-200, 1e200]:
        scaled_chaos = ot.FunctionalChaosResult(
            chaos.getInputSample(),
            chaos.getOutputSample(),
            chaos.getDistribution(),
            chaos.getTransformation(),
            chaos.getInverseTransformation(),
            chaos.getOrthogonalBasis(),
            chaos.getIndices(),
            chaos.getCoefficients() * scale,
            chaos.getReducedBasis(),
        )
        scaled_result = ot.FieldFunctionalChaosResult(
            result.getInputKLResultCollection(),
            scaled_chaos,
            result.getOutputKLResultCollection(),
        )
        scaled_result.setInputSample(input_sample)
        scaled_indices = ot.FieldFunctionalChaosSobolIndices(scaled_result)
        ott.assert_almost_equal(
            scaled_indices.getPointwiseFirstOrderIndices().getValues(),
            reference.getPointwiseFirstOrderIndices().getValues(),
            0.0,
            1e-12,
        )
        ott.assert_almost_equal(
            scaled_indices.getPointwiseTotalOrderIndices().getValues(),
            reference.getPointwiseTotalOrderIndices().getValues(),
            0.0,
            1e-12,
        )
    # The conventional index at exactly zero chaos variance is zero.
    chaos = result.getFCEResult()
    zero_coefficients = ot.Sample(
        chaos.getCoefficients().getSize(), chaos.getCoefficients().getDimension()
    )
    for i, index in enumerate(chaos.getIndices()):
        if index == 0:
            zero_coefficients[i, 0] = 1.0
    zero_chaos = ot.FunctionalChaosResult(
        chaos.getInputSample(),
        chaos.getOutputSample(),
        chaos.getDistribution(),
        chaos.getTransformation(),
        chaos.getInverseTransformation(),
        chaos.getOrthogonalBasis(),
        chaos.getIndices(),
        zero_coefficients,
        chaos.getReducedBasis(),
    )
    zero_result = ot.FieldFunctionalChaosResult(
        result.getInputKLResultCollection(),
        zero_chaos,
        result.getOutputKLResultCollection(),
    )
    zero_result.setInputSample(input_sample)
    zero_sensitivity = ot.FieldFunctionalChaosSobolIndices(zero_result)
    for field in [
        zero_sensitivity.getPointwiseFirstOrderIndices(),
        zero_sensitivity.getPointwiseTotalOrderIndices(),
    ]:
        assert all(value == 0.0 for row in field.getValues() for value in row)

    # Basis indices, rather than coefficient-row order, determine support.
    def with_coefficients(coefficients, indices=None, reduced_basis=None):
        modified_chaos = ot.FunctionalChaosResult(
            chaos.getInputSample(),
            chaos.getOutputSample(),
            chaos.getDistribution(),
            chaos.getTransformation(),
            chaos.getInverseTransformation(),
            chaos.getOrthogonalBasis(),
            chaos.getIndices() if indices is None else indices,
            coefficients,
            chaos.getReducedBasis() if reduced_basis is None else reduced_basis,
        )
        modified_result = ot.FieldFunctionalChaosResult(
            result.getInputKLResultCollection(),
            modified_chaos,
            result.getOutputKLResultCollection(),
        )
        modified_result.setInputSample(input_sample)
        return ot.FieldFunctionalChaosSobolIndices(modified_result)

    coefficients = chaos.getCoefficients()
    order = list(reversed(range(coefficients.getSize())))
    reordered = with_coefficients(
        ot.Sample([coefficients[i] for i in order]),
        ot.Indices([chaos.getIndices()[i] for i in order]),
        [chaos.getReducedBasis()[i] for i in order],
    )
    ott.assert_almost_equal(
        reordered.getPointwiseFirstOrderIndices().getValues(),
        reference.getPointwiseFirstOrderIndices().getValues(),
        0.0,
        1e-14,
    )

    # Opposite KL contributions cancel exactly at one vertex, not everywhere.
    modes = result.getOutputKLResultCollection()[0].getScaledModesAsProcessSample()
    vertex = 20
    cancelling = ot.Sample(coefficients.getSize(), coefficients.getDimension())
    cancelling[1, 0] = modes[1][vertex, 0]
    cancelling[1, 1] = -modes[0][vertex, 0]
    local = with_coefficients(cancelling).getPointwiseFirstOrderIndices().getValues()
    assert all(value == 0.0 for value in local[vertex])
    assert any(value > 0.0 for row in local for value in row)

    invalid = ot.Sample(coefficients)
    invalid[1, 0] = float("nan")
    try:
        with_coefficients(invalid).getPointwiseFirstOrderIndices()
    except (RuntimeError, TypeError) as error:
        assert "Reconstructed chaos coefficients must be finite" in str(error)
    else:
        raise AssertionError("Nonfinite reconstruction was accepted")

    # Vector output is explicitly outside this initial API's scope.
    vector_result = fit(["(1-t)*x1+t*x2", "x1+x2"])
    with ott.assert_raises(RuntimeError):
        ot.FieldFunctionalChaosSobolIndices(
            vector_result
        ).getPointwiseFirstOrderIndices()
finally:
    ot.ResourceMap.SetAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize", old_size)
    ot.ResourceMap.SetAsBool("FunctionalChaosAlgorithm-Sparse", old_sparse)
    ot.ResourceMap.SetAsScalar("FunctionalChaosAlgorithm-QNorm", old_q)
