#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# A combination absorbs the amplitude of a legacy atom into its coefficient
smooth = ot.MaternModel([1.0], [2.0], 2.5)
smooth.setNuggetFactor(0.0)
other = ot.ExponentialModel([3.0], [0.5])
other.setNuggetFactor(0.0)
model = otexp.LinearCombinationCovarianceModel([smooth, other], [0.5, 2.0])
assert model.getInputDimension() == 1
assert model.getOutputDimension() == 1
assert model.isStationary()
assert model.isDiagonal()
ott.assert_almost_equal(model.getCoefficients(), [0.5 * 4.0, 2.0 * 0.25])
ott.assert_almost_equal(model.getAmplitudes(), [2.0**0.5, 0.5**0.5])
assert model.getAtoms().getSize() == 2

s = [0.5]
t = [1.5]
expected = 0.5 * smooth.computeAsScalar(s, t) + 2.0 * other.computeAsScalar(s, t)
ott.assert_almost_equal(model.computeAsScalar(s, t), expected)
ott.assert_almost_equal(model(s, t)[0, 0], expected)
ott.assert_almost_equal(model.computeAsScalar(0.5, 1.5), expected)
ott.assert_almost_equal(model.computeAsScalar([1.0]), model([1.0])[0, 0])

# A legacy nugget becomes an explicit unit Dirac atom
noisy = ot.MaternModel([1.0], [2.0], 2.5)
noisy.setNuggetFactor(0.1)
combination = otexp.LinearCombinationCovarianceModel([noisy], [1.0])
assert combination.getAtoms().getSize() == 2
ott.assert_almost_equal(combination.getCoefficients(), [4.0, 0.4])
# A legacy nugget is an explicit Dirac atom, not the managed nugget term
ott.assert_almost_equal(combination.getNuggetFactor(), 0.0)
ott.assert_almost_equal(combination.getNuggetVariance(), 0.0)
ott.assert_almost_equal(combination.computeAsScalar([0.0]), 4.4)
ott.assert_almost_equal(
    combination.computeAsScalar([0.0], [1.0]), noisy.computeAsScalar([0.0], [1.0])
)

# The managed nugget is inactive by default and can be activated
managed = otexp.LinearCombinationCovarianceModel([smooth], [1.0])
managed.setNuggetFactor(0.2)
assert managed.getAtoms().getSize() == 2
ott.assert_almost_equal(managed.getNuggetFactor(), 0.2)
ott.assert_almost_equal(managed.getNuggetVariance(), 0.8)
assert 1 not in managed.getActiveParameter()
managed.activateNuggetFactor(True)
assert 1 in managed.getActiveParameter()
managed.activateNuggetFactor(False)
assert 1 not in managed.getActiveParameter()

# Creating the managed nugget preserves the existing active parameters and
# leaves its own coefficient inactive
preserved = otexp.LinearCombinationCovarianceModel([smooth], [1.0])
preserved.setActiveParameter(list(preserved.getActiveParameter())[1:])
preserved_active = [
    preserved.getFullParameterDescription()[i]
    for i in preserved.getActiveParameter()
]
preserved.setNuggetFactor(0.5)
assert [
    preserved.getFullParameterDescription()[i]
    for i in preserved.getActiveParameter()
] == preserved_active
assert "coefficient_1" not in [
    preserved.getFullParameterDescription()[i]
    for i in preserved.getActiveParameter()
]

# Deactivating the managed nugget on a model without one is a no-op
plain = otexp.LinearCombinationCovarianceModel([smooth], [1.0])
plain_active = list(plain.getActiveParameter())
plain.activateNuggetFactor(False)
assert list(plain.getActiveParameter()) == plain_active
assert plain.getAtoms().getSize() == 1
assert otexp.LinearCombinationCovarianceModel().getAtoms().getSize() == 1

# Changing the amplitude scales the smooth part and preserves the factor
managed.setAmplitude([3.0])
ott.assert_almost_equal(managed.getAmplitude(), [3.0])
ott.assert_almost_equal(managed.getNuggetFactor(), 0.2)
ott.assert_almost_equal(managed.getNuggetVariance(), 1.8)
ott.assert_almost_equal(managed.computeAsScalar([0.0]), 10.8)

# Multivariate output with a shared correlation matrix
noise = ot.DiracCovarianceModel(1)
noise.setNuggetFactor(0.0)
correlation = ot.CorrelationMatrix(2)
correlation[0, 1] = 0.5
bivariate = otexp.LinearCombinationCovarianceModel([smooth, noise], [1.0, 0.25], 2)
bivariate.setOutputCorrelation(correlation)
assert bivariate.getOutputDimension() == 2
assert not bivariate.isDiagonal()
scalar = smooth.computeAsScalar(s, t)
expected_matrix = ot.SquareMatrix(2)
for i in range(2):
    for j in range(2):
        expected_matrix[i, j] = scalar * correlation[i, j]
ott.assert_almost_equal(bivariate(s, t), expected_matrix)

marginal = bivariate.getMarginal(0)
ott.assert_almost_equal(marginal.computeAsScalar(s, t), scalar)
ott.assert_almost_equal(bivariate.getMarginal([0, 1]).getFullParameter(), bivariate.getFullParameter())

# Multivariate parameter gradient: coefficient, atom parameter and correlation
# entries are all checked against finite differences
bivariate_parameter = bivariate.getFullParameter()
bivariate_gradient = bivariate.parameterGradient(s, t)
eps = 1e-6
for row, index in enumerate(bivariate.getActiveParameter()):
    plus = ot.Point(bivariate_parameter)
    minus = ot.Point(bivariate_parameter)
    plus[index] += eps
    minus[index] -= eps
    plus_model = otexp.LinearCombinationCovarianceModel([smooth, noise], [1.0, 0.25], 2)
    plus_model.setOutputCorrelation(correlation)
    plus_model.setFullParameter(plus)
    minus_model = otexp.LinearCombinationCovarianceModel([smooth, noise], [1.0, 0.25], 2)
    minus_model.setOutputCorrelation(correlation)
    minus_model.setFullParameter(minus)
    finite_difference = (plus_model(s, t) - minus_model(s, t)) / (2.0 * eps)
    for i in range(2):
        for j in range(2):
            ott.assert_almost_equal(
                bivariate_gradient[row, i + 2 * j], finite_difference[i, j], 1e-6, 1e-6
            )

bivariate_input_gradient = bivariate.partialGradient(s, t)
for j in range(bivariate.getInputDimension()):
    plus_s = ot.Point(s)
    minus_s = ot.Point(s)
    plus_s[j] += eps
    minus_s[j] -= eps
    finite_difference = (bivariate(plus_s, t) - bivariate(minus_s, t)) / (2.0 * eps)
    for i in range(2):
        for k in range(2):
            ott.assert_almost_equal(
                bivariate_input_gradient[j, i + 2 * k], finite_difference[i, k], 1e-6, 1e-6
            )

# Full parameter round trip and finite-difference parameter gradient
parameter = model.getFullParameter()
description = model.getFullParameterDescription()
assert parameter.getSize() == description.getSize()
assert description[0] == "coefficient_0"
assert "atom_0_scale_0" in description
copy = otexp.LinearCombinationCovarianceModel([smooth, other], [0.5, 2.0])
copy.setFullParameter(parameter)
ott.assert_almost_equal(copy.getFullParameter(), parameter)
ott.assert_almost_equal(copy.computeAsScalar(s, t), model.computeAsScalar(s, t))

gradient = model.parameterGradient(s, t)
assert gradient.getNbRows() == model.getActiveParameter().getSize()
eps = 1e-6
for row, index in enumerate(model.getActiveParameter()):
    plus = ot.Point(parameter)
    minus = ot.Point(parameter)
    plus[index] += eps
    minus[index] -= eps
    plus_model = otexp.LinearCombinationCovarianceModel([smooth, other], [0.5, 2.0])
    minus_model = otexp.LinearCombinationCovarianceModel([smooth, other], [0.5, 2.0])
    plus_model.setFullParameter(plus)
    minus_model.setFullParameter(minus)
    finite_difference = (
        plus_model.computeAsScalar(s, t) - minus_model.computeAsScalar(s, t)
    ) / (2.0 * eps)
    ott.assert_almost_equal(gradient[row, 0], finite_difference, 1e-6, 1e-6)

# Input gradient
input_gradient = model.partialGradient(s, t)
eps = 1e-6
for j in range(model.getInputDimension()):
    plus_s = ot.Point(s)
    minus_s = ot.Point(s)
    plus_s[j] += eps
    minus_s[j] -= eps
    finite_difference = (
        model.computeAsScalar(plus_s, t) - model.computeAsScalar(minus_s, t)
    ) / (2.0 * eps)
    ott.assert_almost_equal(input_gradient[j, 0], finite_difference, 1e-6, 1e-6)

# Operators: addition normalizes and folds, scaling multiplies the weights
total = smooth + other
ott.assert_almost_equal(total.computeAsScalar(s, t), smooth.computeAsScalar(s, t) + other.computeAsScalar(s, t))
folded = otexp.LinearCombinationCovarianceModel([smooth, smooth], [1.0, 1.0])
assert folded.getAtoms().getSize() == 1
ott.assert_almost_equal(folded.getCoefficients(), [8.0])
ott.assert_almost_equal(
    (smooth + smooth).computeAsScalar(s, t), 2.0 * smooth.computeAsScalar(s, t)
)
scaled = smooth * 2.0
ott.assert_almost_equal(scaled.computeAsScalar(s, t), 2.0 * smooth.computeAsScalar(s, t))
ott.assert_almost_equal((2.0 * smooth).computeAsScalar(s, t), scaled.computeAsScalar(s, t))
scaled_bivariate = bivariate * 2.0
ott.assert_almost_equal(
    scaled_bivariate(s, t), 2.0 * bivariate(s, t)
)

# Limits of the scalar algebra
try:
    ot.DiracCovarianceModel(2) * 2.0
    raise AssertionError("scaling a multivariate leaf should fail")
except Exception:
    pass
try:
    smooth + ot.DiracCovarianceModel(2)
    raise AssertionError("adding a multivariate leaf should fail")
except Exception:
    pass

# Error handling
for atoms, coefficients in [
    ([], []),
    ([smooth], [-1.0]),
    ([smooth], [1.0, 2.0]),
    ([smooth, ot.MaternModel(2)], [1.0, 1.0]),
]:
    try:
        otexp.LinearCombinationCovarianceModel(atoms, coefficients)
        raise AssertionError("invalid construction should fail")
    except Exception:
        pass
try:
    model.getScale()
    raise AssertionError("a combination has no unique scale")
except Exception:
    pass
try:
    managed.setAmplitude([-1.0])
    raise AssertionError("a negative amplitude should fail")
except Exception:
    pass
try:
    bivariate.getMarginal([])
    raise AssertionError("an empty marginal should fail")
except Exception:
    pass
before = model.getFullParameter()
try:
    model.setAtoms([smooth, ot.DiracCovarianceModel(2)])
    raise AssertionError("incompatible replacement atoms should fail")
except Exception:
    pass
ott.assert_almost_equal(model.getFullParameter(), before)

# Replacing the atoms with the normalized ones must preserve the model
same = otexp.LinearCombinationCovarianceModel([smooth, other], [0.5, 2.0])
same.setAtoms(same.getAtoms())
ott.assert_almost_equal(same.getFullParameter(), model.getFullParameter())
ott.assert_almost_equal(same.computeAsScalar(s, t), model.computeAsScalar(s, t))

assert "LinearCombinationCovarianceModel" in str(model)

# Persistence round trip
filename = "linear_combination_covariance_model.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(filename))
study.add("model", bivariate)
study.save()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(filename))
study.load()
loaded = ot.CovarianceModel()
study.fillObject("model", loaded)
ott.assert_almost_equal(loaded.getFullParameter(), bivariate.getFullParameter())
ott.assert_almost_equal(loaded(s, t), bivariate(s, t))
