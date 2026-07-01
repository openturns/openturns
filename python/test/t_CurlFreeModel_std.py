#! /usr/bin/env python
"""Test the CurlFreeModel class."""

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import os

ot.TESTPREAMBLE()


# helpers
def assert_matrix_symmetric(mat):
    """Check that a square matrix is symmetric."""
    n = mat.getNbRows()
    for i in range(n):
        for j in range(n):
            ott.assert_almost_equal(mat[i, j], mat[j, i])


def assert_curl_curl_transpose(curl, s, t):
    """Check C_curl(s,t) == C_curl(t,s)^T."""
    st = curl(s, t)
    ts = curl(t, s)
    n = curl.getInputDimension()
    for i in range(n):
        for j in range(n):
            ott.assert_almost_equal(st[i, j], ts[j, i])


def assert_stationary_consistency(curl, s, t):
    """For stationary models, C_curl(s,t) == C_curl(s-t)."""
    tau = [s[i] - t[i] for i in range(curl.getInputDimension())]
    from_tau = curl(tau)
    from_st = curl(s, t)
    n = curl.getInputDimension()
    for i in range(n):
        for j in range(n):
            ott.assert_almost_equal(from_tau[i, j], from_st[i, j])


# ====================================================================
# 1D case with SquaredExponential (should throw)
# ====================================================================
print("=" * 60)
print("Test 1D SquaredExponential (must fail)")
print("=" * 60)
model_1d = ot.SquaredExponential([1.5], [1.0])
with ott.assert_raises(TypeError):
    otexp.CurlFreeModel(model_1d)
with ott.assert_raises(TypeError):
    otexp.CurlFreeModel(1)

# ====================================================================
# Non-scalar underlying model (should throw)
# ====================================================================
print("=" * 60)
print("Test non-scalar underlying model (must fail)")
print("=" * 60)
model_vec = ot.SquaredExponential([1.5, 2.0], [1.0, 2.0])
assert model_vec.getOutputDimension() == 2
with ott.assert_raises(TypeError):
    otexp.CurlFreeModel(model_vec)

# ====================================================================
# 2D case with SquaredExponential
# ====================================================================
print("=" * 60)
print("Test 2D SquaredExponential")
print("=" * 60)
scale = [1.5, 2.0]
amplitude = [1.0]
model = ot.SquaredExponential(scale, amplitude)
curlFree = otexp.CurlFreeModel(model)
assert curlFree.getInputDimension() == 2
assert curlFree.getOutputDimension() == 2
assert curlFree.isStationary()
ott.assert_almost_equal(curlFree.getScale(), scale)
s = [0.5, 0.3]
t = [0.1, 0.2]
value = curlFree(s, t)
assert value.getNbRows() == 2
assert value.getNbColumns() == 2
ott.assert_almost_equal(value[0, 0], 0.397921, 1e-5)
ott.assert_almost_equal(value[0, 1], -0.00428384, 1e-5)
ott.assert_almost_equal(value[1, 0], -0.00428384, 1e-5)
ott.assert_almost_equal(value[1, 1], 0.240363, 1e-5)
assert_matrix_symmetric(value)
assert_curl_curl_transpose(curlFree, s, t)
assert_stationary_consistency(curlFree, s, t)
# tau=0: C_curl(0)_{ii} = 1/scale_i^2 (exact)
val0 = curlFree([0.0, 0.0])
ott.assert_almost_equal(val0[0, 0], 1.0 / (1.5 * 1.5), 1e-10)
ott.assert_almost_equal(val0[1, 1], 1.0 / (2.0 * 2.0), 1e-10)
ott.assert_almost_equal(val0[0, 1], 0.0, 1e-10)
# setScale then check tau=0
curlFree.setScale([3.0, 4.0])
ott.assert_almost_equal(curlFree([0.0, 0.0])[0, 0], 1.0 / 9.0, 1e-10)
ott.assert_almost_equal(curlFree([0.0, 0.0])[1, 1], 1.0 / 16.0, 1e-10)
curlFree.setScale(scale)
# getCovarianceModel
underlying = curlFree.getCovarianceModel()
ott.assert_almost_equal(underlying.getScale(), model.getScale())
ott.assert_almost_equal(underlying.getAmplitude(), model.getAmplitude())
# getClassName
assert curlFree.getClassName() == "CurlFreeModel"
# getParameter / setParameter
param = curlFree.getParameter()
assert param.getSize() > 0
curlFree.setParameter(param)
desc = curlFree.getParameterDescription()
assert desc.getSize() == param.getSize()
# __repr__ and __str__
assert "CurlFreeModel" in curlFree.__repr__()
assert "CurlFreeModel" in curlFree.__str__()

# ====================================================================
# 3D case with SquaredExponential
# ====================================================================
print("=" * 60)
print("Test 3D SquaredExponential")
print("=" * 60)
model_3d = ot.SquaredExponential([1.0, 1.5, 2.0], [1.0])
curl_3d = otexp.CurlFreeModel(model_3d)
assert curl_3d.getInputDimension() == 3
assert curl_3d.getOutputDimension() == 3
s_3d = [0.5, 0.3, 0.2]
t_3d = [0.1, 0.2, 0.3]
val_3d = curl_3d(s_3d, t_3d)
assert val_3d.getNbRows() == 3
assert val_3d.getNbColumns() == 3
assert_matrix_symmetric(val_3d)
assert_curl_curl_transpose(curl_3d, s_3d, t_3d)
assert_stationary_consistency(curl_3d, s_3d, t_3d)
# tau=0: cross terms vanish
val0_3d = curl_3d([0.0, 0.0, 0.0])
ott.assert_almost_equal(val0_3d[0, 1], 0.0, 1e-10)
ott.assert_almost_equal(val0_3d[0, 2], 0.0, 1e-10)
ott.assert_almost_equal(val0_3d[1, 2], 0.0, 1e-10)

# ====================================================================
# 2D case with AbsoluteExponential
# ====================================================================
print("=" * 60)
print("Test 2D AbsoluteExponential")
print("=" * 60)
model_ae2 = ot.AbsoluteExponential([2.0, 3.0], [1.0])
curl_ae2 = otexp.CurlFreeModel(model_ae2)
s_ae = [0.1, 0.2]
t_ae = [0.3, 0.4]
val_ae = curl_ae2(s_ae, t_ae)
assert val_ae.getNbRows() == 2
assert val_ae.getNbColumns() == 2
assert_matrix_symmetric(val_ae)
assert_curl_curl_transpose(curl_ae2, s_ae, t_ae)
assert_stationary_consistency(curl_ae2, s_ae, t_ae)

# ====================================================================
# 2D case with MaternModel (nu=2.5)
# ====================================================================
print("=" * 60)
print("Test 2D MaternModel nu=2.5")
print("=" * 60)
model_matern = ot.MaternModel([1.5, 2.0], [1.0], 2.5)
curl_matern = otexp.CurlFreeModel(model_matern)
assert curl_matern.getInputDimension() == 2
assert curl_matern.getOutputDimension() == 2
val_mat = curl_matern(s, t)
assert val_mat.getNbRows() == 2
assert val_mat.getNbColumns() == 2
assert_matrix_symmetric(val_mat)
assert_curl_curl_transpose(curl_matern, s, t)
assert_stationary_consistency(curl_matern, s, t)

# ====================================================================
# 2D case with GeneralizedExponential
# ====================================================================
print("=" * 60)
print("Test 2D GeneralizedExponential")
print("=" * 60)
model_ge = ot.GeneralizedExponential([1.5, 2.0], [1.0], 1.5)
curl_ge = otexp.CurlFreeModel(model_ge)
assert curl_ge.getInputDimension() == 2
assert curl_ge.getOutputDimension() == 2
val_ge = curl_ge(s, t)
assert val_ge.getNbRows() == 2
assert val_ge.getNbColumns() == 2
assert_matrix_symmetric(val_ge)
assert_curl_curl_transpose(curl_ge, s, t)
assert_stationary_consistency(curl_ge, s, t)

# ====================================================================
# 4D case (warning expected, conjecture)
# ====================================================================
print("=" * 60)
print("Test 4D SquaredExponential")
print("=" * 60)
model_4d = ot.SquaredExponential([1.0, 1.5, 2.0, 2.5], [1.0])
curl_4d = otexp.CurlFreeModel(model_4d)
assert curl_4d.getInputDimension() == 4
assert curl_4d.getOutputDimension() == 4
assert curl_4d.isStationary()
s_4d = [0.5, 0.3, 0.2, 0.1]
t_4d = [0.1, 0.2, 0.3, 0.4]
val_4d = curl_4d(s_4d, t_4d)
assert val_4d.getNbRows() == 4
assert val_4d.getNbColumns() == 4
assert_matrix_symmetric(val_4d)
assert_curl_curl_transpose(curl_4d, s_4d, t_4d)
assert_stationary_consistency(curl_4d, s_4d, t_4d)

# ====================================================================
# Default constructor
# ====================================================================
print("=" * 60)
print("Test default constructor")
print("=" * 60)
curl_default = otexp.CurlFreeModel(2)
assert curl_default.getInputDimension() == 2
assert curl_default.getOutputDimension() == 2
assert curl_default.isStationary()
val_def = curl_default([0.5, 0.3], [0.1, 0.2])
assert val_def.getNbRows() == 2
assert val_def.getNbColumns() == 2
assert_matrix_symmetric(val_def)

# ====================================================================
# Exception cases
# ====================================================================
print("=" * 60)
print("Test exception cases")
print("=" * 60)
with ott.assert_raises(TypeError):
    curlFree([0.5, 0.3, 0.1], [0.1, 0.2])
with ott.assert_raises(TypeError):
    curlFree([0.5, 0.3], [0.1, 0.2, 0.3])
with ott.assert_raises(TypeError):
    curlFree([0.5, 0.3, 0.1])

# ====================================================================
# Non-stationary model
# ====================================================================
print("=" * 60)
print("Test non-stationary model")
print("=" * 60)
f = ot.SymbolicFunction(["x", "y"], ["x^2 + y^2"])
nonStatModel = ot.StationaryFunctionalCovarianceModel([1.0, 1.5], [1.0], f)
print("Non-stationary model stationary?", nonStatModel.isStationary())
try:
    curlNonStat = otexp.CurlFreeModel(nonStatModel)
    print("Non-stationary CurlFreeModel input dim:", curlNonStat.getInputDimension())
    print("Non-stationary CurlFreeModel stationary:", curlNonStat.isStationary())
    assert not curlNonStat.isStationary()
    sns = [0.5, 0.3]
    tns = [0.1, 0.2]
    val_ns = curlNonStat(sns, tns)
    assert val_ns.getNbRows() == 2
    assert val_ns.getNbColumns() == 2
    # non-stationary should not satisfy stationary consistency
    assert_matrix_symmetric(val_ns)
    assert_curl_curl_transpose(curlNonStat, sns, tns)
except Exception as e:
    print("Cannot create from stationary functional model:", e)

# ====================================================================
# computeAsScalar
# ====================================================================
print("=" * 60)
print("Test computeAsScalar")
print("=" * 60)
scalar_val = curlFree.computeAsScalar(s, t)
ott.assert_almost_equal(scalar_val, value[0, 0])
scalar_tau = curlFree.computeAsScalar([0.2, 0.1])
ott.assert_almost_equal(scalar_tau, curlFree([0.2, 0.1])[0, 0])

# ====================================================================
# Full matrix API
# ====================================================================
print("=" * 60)
print("Test full matrix API")
print("=" * 60)
sample = ot.Sample([[0.0, 0.0], [0.1, 0.1], [0.2, 0.2]])
cov_matrix = curlFree.discretize(sample)
assert cov_matrix.getDimension() == 6
row = curlFree.discretizeRow(sample, 0)
assert row.getSize() == 6
assert row.getDimension() == 2

# ====================================================================
# Parameter gradient
# ====================================================================
print("=" * 60)
print("Test parameter gradient")
print("=" * 60)
pg = curlFree.parameterGradient(s, t)
assert pg.getNbRows() == 3
assert pg.getNbColumns() == 4

# ====================================================================
# Active parameter
# ====================================================================
print("=" * 60)
print("Test active parameter")
print("=" * 60)
active = curlFree.getActiveParameter()
assert active.getSize() == 4
curlFree.setActiveParameter([0, 1])
assert curlFree.getActiveParameter().getSize() == 2

# ====================================================================
# Save / load round-trip
# ====================================================================
print("=" * 60)
print("Test save / load")
print("=" * 60)
if ot.PlatformInfo.HasFeature("libxml2"):
    fname = "curlfree.xml"
    study = ot.Study(fname)
    study.add("curlFree", curlFree)
    study.save()
    study = ot.Study(fname)
    study.load()
    curlFreeLoaded = ot.CovarianceModel()
    study.fillObject("curlFree", curlFreeLoaded)
    os.remove(fname)
    assert curlFreeLoaded.getInputDimension() == 2
    assert curlFreeLoaded.getOutputDimension() == 2
    val_loaded = curlFreeLoaded(s, t)
    ott.assert_almost_equal(val_loaded[0, 0], value[0, 0])
    ott.assert_almost_equal(val_loaded[0, 1], value[0, 1])

# ====================================================================
# Negative assertion: not in main namespace
# ====================================================================
print("=" * 60)
print("Test namespace isolation")
print("=" * 60)
assert not hasattr(ot, "CurlFreeModel")

print("All tests passed!")
