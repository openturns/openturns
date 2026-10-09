#! /usr/bin/env python

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

kernel = ot.ExponentialModel([1.0], [2.0])
scaled = kernel * 3.0
assert scaled.getInputDimension() == 1, "wrong input dim"
assert scaled.getOutputDimension() == 1, "wrong output dim"
explicit_scaled = otexp.ScaledCovarianceModel(kernel, 3.0)
ott.assert_almost_equal([explicit_scaled.getScaleFactor()], [3.0])
ott.assert_almost_equal(scaled.getFullParameter(), explicit_scaled.getFullParameter())
assert scaled.getOutputDimension() == 1, "wrong output dim"
s = [0.5]
t = [1.5]
ott.assert_almost_equal(
    scaled.computeAsScalar(s, t), 3.0 * kernel.computeAsScalar(s, t)
)
ott.assert_almost_equal(scaled(s, t)[0, 0], scaled.computeAsScalar(s, t))
ott.assert_almost_equal(scaled.computeAsScalar(0.5, 1.5), scaled.computeAsScalar(s, t))
ott.assert_almost_equal(
    scaled.computeAsScalar([0.25]), 3.0 * kernel.computeAsScalar([0.25])
)

# Factor accessors are distinct from the input scale vector
ott.assert_almost_equal(scaled.getScale(), kernel.getScale())
ott.assert_almost_equal(scaled.getAmplitude(), kernel.getAmplitude())
scaled.setScale([2.0])
ott.assert_almost_equal(kernel.getScale(), [1.0])
ott.assert_almost_equal(explicit_scaled.getKernel().getScale(), [1.0])
explicit_scaled.setScale([2.0])
ott.assert_almost_equal(explicit_scaled.getKernel().getScale(), [2.0])
explicit_scaled.setScale([1.0])
scaled.setScale([1.0])
with ott.assert_raises(TypeError):
    otexp.ScaledCovarianceModel(kernel, 0.0)
with ott.assert_raises(TypeError):
    otexp.ScaledCovarianceModel(kernel, -1.0)
with ott.assert_raises(TypeError):
    explicit_scaled.setScaleFactor(0.0)

# Factor 1.0 is the identity
assert (kernel * 1.0) == kernel, "factor 1.0 must return the model itself"
ott.assert_almost_equal(
    (3.0 * kernel).computeAsScalar(s, t), scaled.computeAsScalar(s, t)
)

assert scaled != kernel, "scaled must compare unequal to the kernel"
fresh = otexp.ScaledCovarianceModel(kernel, 3.0)
assert fresh == scaled, "equal models must compare equal"
explicit_scaled.setKernel(ot.MaternModel([2.0], [1.0], 1.5))
ott.assert_almost_equal(explicit_scaled.getScale(), [2.0])
explicit_scaled.setNuggetFactor(0.01)
ott.assert_almost_equal([explicit_scaled.getNuggetFactor()], [0.01])

# Flags are delegated to the inner model
assert scaled.isStationary() == kernel.isStationary()
assert scaled.isDiagonal() == kernel.isDiagonal()

# Full parameter appends the factor to the inner full parameter
inner_size = kernel.getFullParameter().getSize()
assert scaled.getFullParameter().getSize() == inner_size + 1
desc = scaled.getFullParameterDescription()
assert desc[desc.getSize() - 1] == "factor"
new_fp = scaled.getFullParameter()
new_fp[inner_size] = 4.0
scaled.setFullParameter(new_fp)
ott.assert_almost_equal([scaled.getFullParameter()[inner_size]], [4.0])
with ott.assert_raises(TypeError):
    scaled.setFullParameter([1.0])

# Active set appends the trailing factor index
assert inner_size in scaled.getActiveParameter(), "factor must be active by default"
scaled.setActiveParameter([inner_size])
frozen = otexp.ScaledCovarianceModel(kernel, 3.0)
frozen.setActiveParameter([inner_size])
assert list(frozen.getKernel().getActiveParameter()) == [], "inner must be frozen"
assert list(scaled.getActiveParameter()) == [inner_size]
with ott.assert_raises(TypeError):
    scaled.setActiveParameter([inner_size + 1])
with ott.assert_raises(TypeError):
    scaled.setActiveParameter([0, 0])

# Gradients scale the inner gradients
ott.assert_almost_equal(
    scaled.partialGradient(s, t), 4.0 * kernel.partialGradient(s, t)
)

# Parameter gradient: scaled inner block plus the kernel value as factor row
gcheck = kernel * 3.0
gpg = gcheck.parameterGradient(s, t)
igpg = kernel.parameterGradient(s, t)
assert gpg.getNbRows() == 3 and gpg.getNbColumns() == 1
for k in range(2):
    ott.assert_almost_equal(
        [gpg[k, 0]], [3.0 * igpg[k, 0]]
    )
ott.assert_almost_equal([gpg[2, 0]], [kernel.computeAsScalar(s, t)])

# Marginals preserve the factor
multi = ot.DiracCovarianceModel(2, [1.0, 2.0])
multi_scaled = multi * 2.0
assert multi_scaled.getOutputDimension() == 2, "wrong multivariate output dim"
p = [0.0, 0.0]
q = [1.0, 1.0]
ott.assert_almost_equal(multi_scaled(p, q), 2.0 * multi(p, q))
marg = multi_scaled.getMarginal(0)
ott.assert_almost_equal(marg(p, q), 2.0 * multi.getMarginal(0)(p, q))
margs = multi_scaled.getMarginal([0, 1])
ott.assert_almost_equal(margs(p, q), multi_scaled(p, q))
mpg = multi_scaled.parameterGradient(p, q)
assert mpg.getNbRows() == 3 and mpg.getNbColumns() == 4
flat = [multi(p, q)[i, j] for j in range(2) for i in range(2)]
ott.assert_almost_equal([[mpg[2, c] for c in range(4)]], [flat])
assert explicit_scaled.isParallel(), "scaled parallel model must be parallel"
assert "ScaledCovarianceModel" in str(explicit_scaled), "wrong __str__"

# Nested wrappers collapse in sums: 2*m + 3*m folds to factor 5
folded = (kernel * 2.0) + (kernel * 3.0)
folded_desc = folded.getFullParameterDescription()
assert folded_desc[folded_desc.getSize() - 1] == "factor", (
    "scaled atoms with equal kernels must fold"
)
ott.assert_almost_equal(
    [folded.getFullParameter()[folded.getFullParameter().getSize() - 1]], [5.0]
)

# Equality and persistence round-trip
assert scaled == scaled, "reflexive equality failed"
assert not scaled == kernel, "scaled must differ from the kernel"
study = ot.Study()
fname = "study_scaled_covariance.xml"
study.setStorageManager(ot.XMLStorageManager(fname))
study.add("scaled", scaled)
study.save()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fname))
study.load()
reloaded = otexp.ScaledCovarianceModel()
study.fillObject("scaled", reloaded)
assert reloaded == scaled, "persistence round-trip failed"
os.remove(fname)
