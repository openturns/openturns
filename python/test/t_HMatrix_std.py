#! /usr/bin/env python

import openturns as ot
import math as m
import os


class TestHMatrixRealAssemblyFunction:
    def __init__(self, vertices, scaling=1.0):
        self.vertices = vertices
        self.scaling = scaling

    def __call__(self, i, j):
        pt1 = self.vertices[i]
        pt2 = self.vertices[j]
        difference = pt1 - pt2
        val = m.exp(-difference.norm() / self.scaling)
        return val


ot.ResourceMap.SetAsBool("HMatrix-ForceSequential", True)
ot.ResourceMap.SetAsUnsignedInteger("HMatrix-MaxLeafSize", 10)

ot.PlatformInfo.SetNumericalPrecision(3)

n = 2
indices = [n, n]
intervalMesher = ot.IntervalMesher(indices)
interval = ot.Interval([0.0] * 2, [1.0] * 2)
mesh2D = intervalMesher.build(interval)
vertices = mesh2D.getVertices()

factory = ot.HMatrixFactory()
parameters = ot.HMatrixParameters()
parameters.setAssemblyEpsilon(1.0e-6)
parameters.setRecompressionEpsilon(1.0e-6)
# HMatrix must be symmetric in order to perform Cholesky decomposition
hmat = factory.build(vertices, 1, True, parameters)
simpleAssembly = TestHMatrixRealAssemblyFunction(vertices, 0.1)

hmat.assembleReal(simpleAssembly, "L")

hmatRef = ot.HMatrix(hmat)

hmat.factorize("LLt")

# Compute A - L*L^T
hmatRef.gemm("N", "T", -1.0, hmat, hmat, 1.0)

# Check LU factorization
hmat = factory.build(vertices, 1, False, parameters)
hmat.assembleReal(simpleAssembly, "N")
hmat.factorize("LU")

print("rows=", hmat.getNbRows())
print("columns=", hmat.getNbColumns())
print("norm=", ot.Point(1, hmat.norm()))
if hmatRef.norm() < 1.0e-3:
    print("norm(A-LLt) < 1e-3")
else:
    print("norm(A-LLt) =", hmatRef.norm())
print("diagonal=", hmat.getDiagonal())
print("compressionRatio= (%d, %d)" % hmat.compressionRatio())
print("fullrkRatio= (%d, %d)" % hmat.fullrkRatio())

# vector multiply
y = ot.Point(hmat.getNbColumns())
x = [2.0] * hmat.getNbColumns()
hmat.gemv("N", 1.0, x, 3.0, y)
print("y=", y)


# block assembly
class TestHMatrixTensorRealAssemblyFunction:
    def __init__(self, covarianceModel, vertices):
        self.covarianceModel = covarianceModel
        self.vertices = vertices

    def __call__(self, i, j):
        pt1 = self.vertices[i]
        pt2 = self.vertices[j]
        val = self.covarianceModel(pt1, pt2)
        return val


covarianceModel = ot.ExponentialModel([0.1] * 2, [1.0] * 2)
hmat = factory.build(vertices, covarianceModel.getOutputDimension(), True, parameters)
blockAssembly = TestHMatrixTensorRealAssemblyFunction(covarianceModel, vertices)
hmat.assembleTensor(blockAssembly, covarianceModel.getOutputDimension(), "L")
hmatRef = ot.HMatrix(hmat)
hmat.factorize("LLt")
if hmat.getRegularizationShift() == 0.0:
    print("no regularization applied")
else:
    print("regularization applied")
normL = hmat.norm()
hmatRef.gemm("N", "T", -1.0, hmat, hmat, 1.0)
if hmatRef.norm() < 1e-3:
    print("norm(A-LLt) < 1e-3")
else:
    print("norm(A-LLt) =", hmatRef.norm())

alpha = 0.1
hmat.scale(alpha)
normScaled = hmat.norm()
if abs(normL - normScaled / alpha) < 1e-10:
    print("|norm(L) - 10 * norm(0.1*L)| < 1e-10")
else:
    print("|norm(L) - 10 * norm(0.1*L)| > 1e-10")

# Regression test for the 1D LLt bug (hmat-oss): the low-rank truncation
# epsilon must be set on the matrix before assembly, otherwise every
# admissible block is kept at full rank and the Cholesky factorization of
# the ill-conditioned matrix fails, forcing the wrapper to regularize.
ot.ResourceMap.SetAsUnsignedInteger("HMatrix-MaxLeafSize", 32)
n1d = 2048
vertices1d = ot.Sample([[float(i) / (n1d - 1)] for i in range(n1d)])
matern = ot.MaternModel([0.01], [1.0], 2.5)
hmat1d = matern.discretizeHMatrix(vertices1d, parameters)
hmat1d.factorize("LLt")
if hmat1d.getRegularizationShift() == 0.0:
    print("1D LLt: no regularization applied")
else:
    print("1D LLt: regularization applied")

# Check the factorization accuracy against an unfactored copy.
hmat1dRef = matern.discretizeHMatrix(vertices1d, parameters)
hmat1dRef.gemm("N", "T", -1.0, hmat1d, hmat1d, 1.0)
# the residual is machine-precision and its exact value depends on the BLAS,
# so it cannot be diffed verbatim in the expected output
if hmat1dRef.norm() < 1.0e-6:
    print("1D LLt: norm(A-LLt) < 1e-6")
else:
    print("1D LLt: norm(A-LLt) =", ot.Point(1, hmat1dRef.norm()))

# Check the solve: after an LLt factorization gemv('N') applies L, so
# A.x = L.(L^t.x) is rebuilt with two gemv calls.
x1d = hmat1d.solve(ot.Point(n1d, 1.0))
y1d = ot.Point(n1d)
hmat1d.gemv("T", 1.0, x1d, 0.0, y1d)
z1d = ot.Point(n1d)
hmat1d.gemv("N", 1.0, y1d, 0.0, z1d)
d1d = ot.Point(n1d)
for i in range(n1d):
    d1d[i] = z1d[i] - 1.0
res1d = d1d.norm() / float(n1d) ** 0.5
if res1d < 1.0e-6:
    print("1D LLt: relative residual < 1e-6")
else:
    print("1D LLt: relative residual =", res1d)

# Check applyFactor: after an LLt factorization the stored matrix is L, so
# applyFactor(y, x) = L.x is the uniform factor-application entry point used
# by GaussianProcess sampling (on HMatrix gemv('N') also applies L, whereas on
# HODLRMatrix gemv('N') reconstructs A.x; applyFactor has the same meaning on
# both backends).
xaf = ot.Point(n1d)
for i in range(n1d):
    xaf[i] = 1.0 + (i % 7)
yaf = ot.Point(n1d)
hmat1d.applyFactor(yaf, xaf)
yaf2 = ot.Point(n1d)
hmat1d.gemv("N", 1.0, xaf, 0.0, yaf2)
resAF = max(abs(yaf[i] - yaf2[i]) for i in range(n1d))
if resAF < 1.0e-14:
    print("applyFactor == gemv('N')")
else:
    print("applyFactor != gemv('N'), diff =", resAF)
# L^{-1} (L x) = x
xrec = hmat1d.solveLower(yaf)
resAF2 = max(abs(xrec[i] - xaf[i]) for i in range(n1d))
if resAF2 < 1.0e-6:
    print("solveLower(applyFactor(x)) == x")
else:
    print("solveLower(applyFactor(x)) != x, diff =", resAF2)

# Regression test for the regularization warning: a significant shift must be
# reported through the log. The stderr is captured so the expected output is
# unchanged.
prev_warnh = ot.ResourceMap.GetAsScalar("HMatrix-RegularizationWarnThreshold")
try:
    ot.ResourceMap.SetAsScalar("HMatrix-RegularizationWarnThreshold", 0.0)
    nreg = 256
    vertices_reg = ot.Sample([[float(i) / (nreg - 1.0)] for i in range(nreg)])
    for i in range(nreg // 3, nreg):
        vertices_reg[i, 0] = vertices_reg[i - nreg // 3, 0]
    hmat_reg = matern.discretizeHMatrix(vertices_reg, parameters)
    r, w = os.pipe()
    saved = os.dup(2)
    os.dup2(w, 2)
    hmat_reg.factorize("LLt")
    ot.Log.Flush()
    os.dup2(saved, 2)
    os.close(saved)
    os.close(w)
    msg = os.read(r, 65536).decode(errors="replace")
    assert "regularization shift" in msg, "a significant shift must emit a warning"
finally:
    ot.ResourceMap.SetAsScalar("HMatrix-RegularizationWarnThreshold", prev_warnh)
