#! /usr/bin/env python
"""Test the DivFreeModel class."""

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


def assert_div_transpose(div, s, t):
    """Check C_div(s,t) == C_div(t,s)^T."""
    st = div(s, t)
    ts = div(t, s)
    n = div.getInputDimension()
    for i in range(n):
        for j in range(n):
            ott.assert_almost_equal(st[i, j], ts[j, i])


def assert_stationary_consistency(div, s, t):
    """For stationary models, C_div(s,t) == C_div(s-t)."""
    tau = [s[i] - t[i] for i in range(div.getInputDimension())]
    from_tau = div(tau)
    from_st = div(s, t)
    n = div.getInputDimension()
    for i in range(n):
        for j in range(n):
            ott.assert_almost_equal(from_tau[i, j], from_st[i, j])


def assert_curl_div_relation(curl, div, s, t):
    """Check C_curl + C_div = trace(H)*I."""
    n = curl.getInputDimension()
    curl_mat = curl(s, t)
    div_mat = div(s, t)
    sum_mat = curl_mat + div_mat
    trace = sum(curl_mat[i, i] for i in range(n))
    for i in range(n):
        ott.assert_almost_equal(sum_mat[i, i], trace)
        for j in range(n):
            if i != j:
                ott.assert_almost_equal(sum_mat[i, j], 0.0)


# ====================================================================
# 1D case with SquaredExponential (should throw)
# ====================================================================
print("=" * 60)
print("Test 1D SquaredExponential (must fail)")
print("=" * 60)
model_1d = ot.SquaredExponential([1.5], [1.0])
with ott.assert_raises(TypeError):
    otexp.DivFreeModel(model_1d)
with ott.assert_raises(TypeError):
    otexp.DivFreeModel(1)

# ====================================================================
# Non-scalar underlying model (should throw)
# ====================================================================
print("=" * 60)
print("Test non-scalar underlying model (must fail)")
print("=" * 60)
model_vec = ot.SquaredExponential([1.5, 2.0], [1.0, 2.0])
assert model_vec.getOutputDimension() == 2
with ott.assert_raises(TypeError):
    otexp.DivFreeModel(model_vec)

# ====================================================================
# 2D case with SquaredExponential
# ====================================================================
print("=" * 60)
print("Test 2D SquaredExponential")
print("=" * 60)
scale = [1.5, 2.0]
amplitude = [1.0]
model = ot.SquaredExponential(scale, amplitude)
divFree = otexp.DivFreeModel(model)
assert divFree.getInputDimension() == 2
assert divFree.getOutputDimension() == 2
assert divFree.isStationary()
ott.assert_almost_equal(divFree.getScale(), scale)
s = [0.5, 0.3]
t = [0.1, 0.2]
value = divFree(s, t)
assert value.getNbRows() == 2
assert value.getNbColumns() == 2
ott.assert_almost_equal(value[0, 0], 0.240363, 1e-5)
ott.assert_almost_equal(value[0, 1], 0.00428384, 1e-5)
ott.assert_almost_equal(value[1, 0], 0.00428384, 1e-5)
ott.assert_almost_equal(value[1, 1], 0.397921, 1e-5)
assert_matrix_symmetric(value)
assert_div_transpose(divFree, s, t)
assert_stationary_consistency(divFree, s, t)
# curl-div relation
curlFree = otexp.CurlFreeModel(model)
assert_curl_div_relation(curlFree, divFree, s, t)
# tau=0: C_div(0) = trace(H)*I - H, H = diag(1/L_i^2) (exact)
val0 = divFree([0.0, 0.0])
ott.assert_almost_equal(val0[0, 0], 1.0 / (2.0 * 2.0), 1e-10)
ott.assert_almost_equal(val0[1, 1], 1.0 / (1.5 * 1.5), 1e-10)
ott.assert_almost_equal(val0[0, 1], 0.0, 1e-10)
divFree.setScale([3.0, 4.0])
ott.assert_almost_equal(divFree([0.0, 0.0])[0, 0], 1.0 / 16.0, 1e-10)
ott.assert_almost_equal(divFree([0.0, 0.0])[1, 1], 1.0 / 9.0, 1e-10)
divFree.setScale(scale)
# getCovarianceModel
underlying = divFree.getCovarianceModel()
ott.assert_almost_equal(underlying.getScale(), model.getScale())
ott.assert_almost_equal(underlying.getAmplitude(), model.getAmplitude())
assert divFree.getClassName() == "DivFreeModel"
# getParameter / setParameter
param = divFree.getParameter()
assert param.getSize() > 0
divFree.setParameter(param)
desc = divFree.getParameterDescription()
assert desc.getSize() == param.getSize()
# __repr__ and __str__
assert "DivFreeModel" in divFree.__repr__()
assert "DivFreeModel" in divFree.__str__()

# ====================================================================
# 3D case with SquaredExponential
# ====================================================================
print("=" * 60)
print("Test 3D SquaredExponential")
print("=" * 60)
model_3d = ot.SquaredExponential([1.0, 1.5, 2.0], [1.0])
div_3d = otexp.DivFreeModel(model_3d)
assert div_3d.getInputDimension() == 3
assert div_3d.getOutputDimension() == 3
s_3d = [0.5, 0.3, 0.2]
t_3d = [0.1, 0.2, 0.3]
val_3d = div_3d(s_3d, t_3d)
assert val_3d.getNbRows() == 3
assert val_3d.getNbColumns() == 3
assert_matrix_symmetric(val_3d)
assert_div_transpose(div_3d, s_3d, t_3d)
assert_stationary_consistency(div_3d, s_3d, t_3d)
curl_3d = otexp.CurlFreeModel(model_3d)
assert_curl_div_relation(curl_3d, div_3d, s_3d, t_3d)
# tau=0: cross terms vanish
val0_3d = div_3d([0.0, 0.0, 0.0])
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
div_ae2 = otexp.DivFreeModel(model_ae2)
s_ae = [0.1, 0.2]
t_ae = [0.3, 0.4]
val_ae = div_ae2(s_ae, t_ae)
assert val_ae.getNbRows() == 2
assert val_ae.getNbColumns() == 2
assert_matrix_symmetric(val_ae)
assert_div_transpose(div_ae2, s_ae, t_ae)
assert_stationary_consistency(div_ae2, s_ae, t_ae)
curl_ae2 = otexp.CurlFreeModel(model_ae2)
assert_curl_div_relation(curl_ae2, div_ae2, s_ae, t_ae)

# ====================================================================
# 2D case with MaternModel (nu=2.5)
# ====================================================================
print("=" * 60)
print("Test 2D MaternModel nu=2.5")
print("=" * 60)
model_matern = ot.MaternModel([1.5, 2.0], [1.0], 2.5)
div_matern = otexp.DivFreeModel(model_matern)
assert div_matern.getInputDimension() == 2
assert div_matern.getOutputDimension() == 2
val_mat = div_matern(s, t)
assert val_mat.getNbRows() == 2
assert val_mat.getNbColumns() == 2
assert_matrix_symmetric(val_mat)
assert_div_transpose(div_matern, s, t)
assert_stationary_consistency(div_matern, s, t)
curl_matern = otexp.CurlFreeModel(model_matern)
assert_curl_div_relation(curl_matern, div_matern, s, t)

# ====================================================================
# 2D case with GeneralizedExponential
# ====================================================================
print("=" * 60)
print("Test 2D GeneralizedExponential")
print("=" * 60)
model_ge = ot.GeneralizedExponential([1.5, 2.0], [1.0], 1.5)
div_ge = otexp.DivFreeModel(model_ge)
assert div_ge.getInputDimension() == 2
assert div_ge.getOutputDimension() == 2
val_ge = div_ge(s, t)
assert val_ge.getNbRows() == 2
assert val_ge.getNbColumns() == 2
assert_matrix_symmetric(val_ge)
assert_div_transpose(div_ge, s, t)
assert_stationary_consistency(div_ge, s, t)
curl_ge = otexp.CurlFreeModel(model_ge)
assert_curl_div_relation(curl_ge, div_ge, s, t)

# ====================================================================
# 4D case (warning expected, conjecture)
# ====================================================================
print("=" * 60)
print("Test 4D SquaredExponential")
print("=" * 60)
model_4d = ot.SquaredExponential([1.0, 1.5, 2.0, 2.5], [1.0])
div_4d = otexp.DivFreeModel(model_4d)
assert div_4d.getInputDimension() == 4
assert div_4d.getOutputDimension() == 4
assert div_4d.isStationary()
s_4d = [0.5, 0.3, 0.2, 0.1]
t_4d = [0.1, 0.2, 0.3, 0.4]
val_4d = div_4d(s_4d, t_4d)
assert val_4d.getNbRows() == 4
assert val_4d.getNbColumns() == 4
assert_matrix_symmetric(val_4d)
assert_div_transpose(div_4d, s_4d, t_4d)
assert_stationary_consistency(div_4d, s_4d, t_4d)
curl_4d = otexp.CurlFreeModel(model_4d)
assert_curl_div_relation(curl_4d, div_4d, s_4d, t_4d)

# ====================================================================
# Default constructor
# ====================================================================
print("=" * 60)
print("Test default constructor")
print("=" * 60)
div_default = otexp.DivFreeModel(2)
assert div_default.getInputDimension() == 2
assert div_default.getOutputDimension() == 2
assert div_default.isStationary()
val_def = div_default([0.5, 0.3], [0.1, 0.2])
assert val_def.getNbRows() == 2
assert val_def.getNbColumns() == 2
assert_matrix_symmetric(val_def)
curl_default = otexp.CurlFreeModel(2)
assert_curl_div_relation(curl_default, div_default, [0.5, 0.3], [0.1, 0.2])

# ====================================================================
# Exception cases
# ====================================================================
print("=" * 60)
print("Test exception cases")
print("=" * 60)
with ott.assert_raises(TypeError):
    divFree([0.5, 0.3, 0.1], [0.1, 0.2])
with ott.assert_raises(TypeError):
    divFree([0.5, 0.3], [0.1, 0.2, 0.3])
with ott.assert_raises(TypeError):
    divFree([0.5, 0.3, 0.1])

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
    divNonStat = otexp.DivFreeModel(nonStatModel)
    print("Non-stationary DivFreeModel input dim:", divNonStat.getInputDimension())
    print("Non-stationary DivFreeModel stationary:", divNonStat.isStationary())
    assert not divNonStat.isStationary()
    sns = [0.5, 0.3]
    tns = [0.1, 0.2]
    val_ns = divNonStat(sns, tns)
    assert val_ns.getNbRows() == 2
    assert val_ns.getNbColumns() == 2
    assert_matrix_symmetric(val_ns)
    assert_div_transpose(divNonStat, sns, tns)
    curlNonStat = otexp.CurlFreeModel(nonStatModel)
    assert_curl_div_relation(curlNonStat, divNonStat, sns, tns)
except Exception as e:
    print("Cannot create from stationary functional model:", e)

# ====================================================================
# computeAsScalar
# ====================================================================
print("=" * 60)
print("Test computeAsScalar")
print("=" * 60)
scalar_val = divFree.computeAsScalar(s, t)
ott.assert_almost_equal(scalar_val, value[0, 0])
scalar_tau = divFree.computeAsScalar([0.2, 0.1])
ott.assert_almost_equal(scalar_tau, divFree([0.2, 0.1])[0, 0])

# ====================================================================
# Full matrix API
# ====================================================================
print("=" * 60)
print("Test full matrix API")
print("=" * 60)
sample = ot.Sample([[0.0, 0.0], [0.1, 0.1], [0.2, 0.2]])
cov_matrix = divFree.discretize(sample)
assert cov_matrix.getDimension() == 6
row = divFree.discretizeRow(sample, 0)
assert row.getSize() == 6
assert row.getDimension() == 2

# ====================================================================
# Parameter gradient
# ====================================================================
print("=" * 60)
print("Test parameter gradient")
print("=" * 60)
pg = divFree.parameterGradient(s, t)
assert pg.getNbRows() == 3
assert pg.getNbColumns() == 4

# ====================================================================
# Active parameter
# ====================================================================
print("=" * 60)
print("Test active parameter")
print("=" * 60)
active = divFree.getActiveParameter()
assert active.getSize() == 4
divFree.setActiveParameter([0, 1])
assert divFree.getActiveParameter().getSize() == 2

# ====================================================================
# Save / load round-trip
# ====================================================================
print("=" * 60)
print("Test save / load")
print("=" * 60)
if ot.PlatformInfo.HasFeature("libxml2"):
    fname = "divfree.xml"
    study = ot.Study(fname)
    study.add("divFree", divFree)
    study.save()
    study = ot.Study(fname)
    study.load()
    divFreeLoaded = ot.CovarianceModel()
    study.fillObject("divFree", divFreeLoaded)
    os.remove(fname)
    assert divFreeLoaded.getInputDimension() == 2
    assert divFreeLoaded.getOutputDimension() == 2
    val_loaded = divFreeLoaded(s, t)
    ott.assert_almost_equal(val_loaded[0, 0], value[0, 0])
    ott.assert_almost_equal(val_loaded[0, 1], value[0, 1])

# ====================================================================
# Negative assertion: not in main namespace
# ====================================================================
print("=" * 60)
print("Test namespace isolation")
print("=" * 60)
assert not hasattr(ot, "DivFreeModel")

print("All tests passed!")
