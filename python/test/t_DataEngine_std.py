#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math

ot.TESTPREAMBLE()

# ============================================================
# DataContainer tests
# ============================================================
print("=== DataContainer Tests ===")

# Default constructor
dc1 = ot.DataContainer()
assert dc1.isEmpty()
assert dc1.getSize() == 0
assert dc1.getDimension() == 0
print("Default constructor: OK")

# Constructor with size
dc2 = ot.DataContainer(5, 3.0)
assert dc2.getSize() == 5
assert dc2.getDimension() == 1
assert dc2.getNbRows() == 5
assert dc2.getNbColumns() == 1
assert not dc2.isEmpty()
assert dc2[0] == 3.0
assert dc2[4] == 3.0
print("Constructor with size: OK")

# Constructor with dimensions
dc3 = ot.DataContainer(3, 4, 2.0)
assert dc3.getSize() == 3
assert dc3.getDimension() == 4
assert dc3.getLayout() == ot.DataContainer.COLUMN_MAJOR
print("Constructor with dimensions: OK")

# Row-major layout
dc4 = ot.DataContainer(3, 2, 1.0, ot.DataContainer.ROW_MAJOR)
assert dc4.getLayout() == ot.DataContainer.ROW_MAJOR
assert dc4.getLayout() == 0
print("Row-major layout: OK")

# Element access
dc5 = ot.DataContainer(4, 0.0)
dc5[0] = 1.0
dc5[1] = 2.0
dc5[2] = 3.0
dc5[3] = 4.0
assert dc5[0] == 1.0
assert dc5[3] == 4.0
print("Element access: OK")

# stride
dc6 = ot.DataContainer(3, 4, 0.0)
assert dc6.stride(0) == 1  # column-major: stride between rows
assert dc6.stride(1) == 3  # column-major: stride between columns
print("Stride: OK")

# __len__
assert len(dc6) == 12
print("len(): OK")

# setDimension
dc7 = ot.DataContainer(6, 0.0)
dc7.setDimension(3)
assert dc7.getSize() == 6
assert dc7.getDimension() == 3
assert dc7.getNbRows() == 6
assert dc7.getNbColumns() == 3
print("setDimension: OK")

# resize
dc8 = ot.DataContainer(4, 1.0)
dc8.resize(8)
assert dc8.getSize() == 8
print("resize: OK")

# bounds-checked element access
dc9 = ot.DataContainer(3, 0.0)
dc9[0] = 10.0
dc9[2] = 30.0
assert dc9[0] == 10.0
assert dc9[2] == 30.0
with ott.assert_raises(IndexError):
    dc9[3]
with ott.assert_raises(IndexError):
    dc9[999]
with ott.assert_raises(IndexError):
    dc9[3] = 1.0
print("Bounds-checked access: OK")

# erase
dc10 = ot.DataContainer(6, 0.0)
for i in range(6):
    dc10[i] = float(i)
# empty range is a no-op
dc10.erase(2, 2)
assert len(dc10) == 6
# erase [1, 4) -> [0, 4, 5]
dc10.erase(1, 4)
assert len(dc10) == 3
ott.assert_almost_equal(dc10[0], 0.0)
ott.assert_almost_equal(dc10[1], 4.0)
ott.assert_almost_equal(dc10[2], 5.0)
# out-of-range throws and leaves the container unchanged
with ott.assert_raises(TypeError):
    dc10.erase(1, 4)
assert len(dc10) == 3
print("erase: OK")

# clear
dc13 = ot.DataContainer(3, 4, 1.0, ot.DataContainer.ROW_MAJOR)
dc13.clear()
assert dc13.isEmpty()
assert len(dc13) == 0
print("clear: OK")

# makeUnique (copy-on-write)
dc14 = ot.DataContainer(4, 2, 0.0, ot.DataContainer.ROW_MAJOR)
for i in range(8):
    dc14[i] = float(i)
dc14.makeUnique()
assert not dc14.isView()
assert len(dc14) == 8
assert dc14[3] == 3.0
# mutations after makeUnique still work and are isolated
dc14[0] = -1.0
assert dc14[0] == -1.0
print("makeUnique: OK")

# views (subView over contiguous rows, zero-copy)
parent = ot.DataContainer(4, 2, 0.0, ot.DataContainer.ROW_MAJOR)
for i in range(8):
    parent[i] = float(i)
view = parent.subView(1, 2)
assert view.isView()
assert not parent.isView()
assert view.getSize() == 2
assert view.getDimension() == 2
# view shares memory: mutation through the view is visible in the parent
view[0] = 100.0
ott.assert_almost_equal(parent[2], 100.0)
ott.assert_almost_equal(view[0], 100.0)
ott.assert_almost_equal(parent[3], view[1])
# out-of-range subView throws
with ott.assert_raises(TypeError):
    parent.subView(3, 2)
# erase on a view is rejected: views do not own their memory
with ott.assert_raises(RuntimeError):
    view.erase(0, 1)
assert len(parent) == 8
ott.assert_almost_equal(parent[2], 100.0)

print("All DataContainer tests passed!")

# ============================================================
# AlgebraEngine tests
# ============================================================
print("\n=== AlgebraEngine Tests ===")

# Dot product
a = ot.DataContainer(3, 1.0)
b = ot.DataContainer(3, 2.0)
assert abs(ot.AlgebraEngine.Dot(a, b) - 6.0) < 1e-12
print("Dot product: OK")

# Norm
v = ot.DataContainer(4, 1.0)
assert abs(ot.AlgebraEngine.Norm(v) - 2.0) < 1e-12
print("Norm: OK")

# Norm1
assert abs(ot.AlgebraEngine.Norm1(v) - 4.0) < 1e-12
print("Norm1: OK")

# NormInf
assert abs(ot.AlgebraEngine.NormInf(v) - 1.0) < 1e-12
print("NormInf: OK")

# Scale
s = ot.AlgebraEngine.Scale(a, 3.0)
assert s[0] == 3.0
assert s[2] == 3.0
print("Scale: OK")

# Axpy
x = ot.DataContainer(3, 1.0)
y = ot.DataContainer(3, 2.0)
r = ot.AlgebraEngine.Axpy(2.0, x, y)
assert r[0] == 4.0
assert r[2] == 4.0
print("Axpy: OK")

# MatrixPointProduct
A = ot.DataContainer(2, 3, 0.0)
A[0] = 1.0
A[1] = 2.0
A[2] = 3.0
A[3] = 4.0
A[4] = 5.0
A[5] = 6.0
x = ot.DataContainer(3, 1.0)
r = ot.AlgebraEngine.MatrixPointProduct(A, x)
assert abs(r[0] - 9.0) < 1e-12
assert abs(r[1] - 12.0) < 1e-12
print("MatrixPointProduct: OK")

# SymMatrixPointProduct (A symmetric, column-major)
Asym = ot.DataContainer(2, 2, 0.0)
Asym[0] = 4.0
Asym[1] = 1.0
Asym[2] = 1.0
Asym[3] = 3.0
xs = ot.DataContainer(2, 1.0)
xs[1] = 2.0
rs = ot.AlgebraEngine.SymMatrixPointProduct(Asym, xs)
assert abs(rs[0] - 6.0) < 1e-12  # 4*1 + 1*2
assert abs(rs[1] - 7.0) < 1e-12  # 1*1 + 3*2
print("SymMatrixPointProduct: OK")

# MatrixProduct
A2 = ot.DataContainer(2, 2, 0.0)
A2[0] = 1.0
A2[1] = 2.0
A2[2] = 3.0
A2[3] = 4.0
B2 = ot.DataContainer(2, 2, 0.0)
B2[0] = 5.0
B2[1] = 6.0
B2[2] = 7.0
B2[3] = 8.0
C2 = ot.AlgebraEngine.MatrixProduct(A2, B2)
assert abs(C2[0] - 23.0) < 1e-12
assert abs(C2[1] - 34.0) < 1e-12
assert abs(C2[2] - 31.0) < 1e-12
assert abs(C2[3] - 46.0) < 1e-12
print("MatrixProduct: OK")

# SymProd: 'L' -> A B A^T, 'R' -> A^T B A
Adiag = ot.DataContainer(2, 2, 0.0)
Adiag[0] = 1.0
Adiag[3] = 2.0
Bdiag = ot.DataContainer(2, 2, 0.0)
Bdiag[0] = 2.0
Bdiag[3] = 3.0
symL = ot.AlgebraEngine.SymProd(Adiag, Bdiag, "L")
assert abs(symL[0] - 2.0) < 1e-12
assert abs(symL[3] - 12.0) < 1e-12
symR = ot.AlgebraEngine.SymProd(Adiag, Bdiag, "R")
assert abs(symR[0] - 2.0) < 1e-12
assert abs(symR[3] - 12.0) < 1e-12
print("SymProd: OK")

# TriangularProd: lower/upper, left/right
Ltri = ot.DataContainer(2, 2, 0.0)
Ltri[0] = 2.0
Ltri[1] = 1.0
Ltri[3] = 3.0  # lower triangular [[2, 0], [1, 3]]
Id2 = ot.DataContainer(2, 2, 0.0)
Id2[0] = 1.0
Id2[3] = 1.0
tL = ot.AlgebraEngine.TriangularProd(Ltri, Id2, "L", "L")
assert abs(tL[0] - 2.0) < 1e-12
assert abs(tL[3] - 3.0) < 1e-12
tR = ot.AlgebraEngine.TriangularProd(Id2, Ltri, "R", "L")
assert abs(tR[0] - 2.0) < 1e-12
assert abs(tR[1] - 1.0) < 1e-12
assert abs(tR[3] - 3.0) < 1e-12
print("TriangularProd: OK")

# ComputeLU (non-block): L unit lower-triangular, U upper-triangular, L*U equals A up to pivoting
ALU = ot.DataContainer(2, 2, 0.0)
ALU[0] = 1.0
ALU[1] = 3.0
ALU[2] = 2.0
ALU[3] = 1.0  # column-major [[1, 2], [3, 1]]
L4 = ot.DataContainer()
U4 = ot.DataContainer()
ot.AlgebraEngine.ComputeLU(ALU, L4, U4)
# L unit lower triangular, U upper triangular
assert L4[0] == 1.0
assert L4[3] == 1.0
assert L4[2] == 0.0
assert U4[1] == 0.0
LULU = ot.AlgebraEngine.MatrixProduct(L4, U4)
rows_lu = sorted([(LULU[0], LULU[2]), (LULU[1], LULU[3])])
rows_a = sorted([(ALU[0], ALU[2]), (ALU[1], ALU[3])])
for (rl0, rl1), (ra0, ra1) in zip(rows_lu, rows_a):
    assert abs(rl0 - ra0) < 1e-10
    assert abs(rl1 - ra1) < 1e-10
# solve A x = [3, 4] -> x = [1, 1]
bLU = ot.DataContainer(2, 3.0)
bLU[1] = 4.0
xLU = ot.AlgebraEngine.SolveLinearSystem(ALU, bLU)
assert abs(xLU[0] - 1.0) < 1e-10
assert abs(xLU[1] - 1.0) < 1e-10
print("ComputeLU: OK")

# Transpose of [[1,3],[2,4]] = [[1,2],[3,4]], column-major stored as [1,3,2,4]
AT = ot.AlgebraEngine.Transpose(A2)
assert AT.getSize() == 2
assert AT.getDimension() == 2
assert abs(AT[0] - 1.0) < 1e-12
assert abs(AT[1] - 3.0) < 1e-12
assert abs(AT[2] - 2.0) < 1e-12
assert abs(AT[3] - 4.0) < 1e-12
print("Transpose: OK")

# ComputeCholesky (2x2 SPD)
spd = ot.DataContainer(2, 2, 0.0)
spd[0] = 4.0
spd[1] = 2.0
spd[2] = 2.0
spd[3] = 5.0
L = ot.AlgebraEngine.ComputeCholesky(spd)
assert abs(L[0] - 2.0) < 1e-12
assert abs(L[1] - 1.0) < 1e-12
assert abs(L[3] - 2.0) < 1e-12
print("ComputeCholesky: OK")

# SolveLinearSystemSPD: inv([[4,2],[2,5]]) * [1,1] = [5/16-2/16, -2/16+4/16] = [3/16, 2/16]
b = ot.DataContainer(2, 1.0)
x = ot.AlgebraEngine.SolveLinearSystemSPD(spd, b)
assert abs(x[0] - 3.0 / 16.0) < 1e-12
assert abs(x[1] - 2.0 / 16.0) < 1e-12
print("SolveLinearSystemSPD: OK")

# Inverse
inv = ot.AlgebraEngine.Inverse(spd)
assert abs(inv[0] - 5.0 / 16.0) < 1e-12
assert abs(inv[1] - (-2.0 / 16.0)) < 1e-12
assert abs(inv[2] - (-2.0 / 16.0)) < 1e-12
assert abs(inv[3] - 4.0 / 16.0) < 1e-12
print("Inverse: OK")

# InverseSPD
invSPD = ot.AlgebraEngine.InverseSPD(spd)
assert abs(invSPD[0] - inv[0]) < 1e-12
print("InverseSPD: OK")

# ComputeDeterminant
det = ot.AlgebraEngine.ComputeDeterminant(spd)
assert abs(det - 16.0) < 1e-12
print("ComputeDeterminant: OK")

# ComputeDeterminant on non-symmetric matrix
detNS = ot.AlgebraEngine.ComputeDeterminant(ALU)
assert abs(detNS + 5.0) < 1e-12  # det([[1, 2], [3, 1]]) = -5
print("ComputeDeterminantNS: OK")

# ComputeLogAbsoluteDeterminant returns (logAbsDet, sign)
logdet, sign = ot.AlgebraEngine.ComputeLogAbsoluteDeterminant(spd)
assert abs(logdet - math.log(16.0)) < 1e-12
assert abs(sign - 1.0) < 1e-12
logdetNS, signNS = ot.AlgebraEngine.ComputeLogAbsoluteDeterminant(ALU)
assert abs(logdetNS - math.log(5.0)) < 1e-12
assert abs(signNS + 1.0) < 1e-12
logdetB, signB = ot.AlgebraEngine.ComputeLogAbsoluteDeterminantBlockwise(spd, 1)
assert abs(logdetB - math.log(16.0)) < 1e-12
assert abs(signB - 1.0) < 1e-12
logdetB2, signB2 = ot.AlgebraEngine.ComputeLogAbsoluteDeterminantBlockwise(ALU)
assert abs(logdetB2 - math.log(5.0)) < 1e-12
assert abs(signB2 + 1.0) < 1e-12
print("ComputeLogAbsoluteDeterminant(+Blockwise): OK")

# ComputeTrace
tr = ot.AlgebraEngine.ComputeTrace(spd)
assert abs(tr - 9.0) < 1e-12
print("ComputeTrace: OK")

# Eigenvalues of a symmetric matrix are real
eigSym = ot.AlgebraEngine.ComputeEigenValuesSymmetric(spd)
assert len(eigSym) == 2
# eigenvalues of [[4, 2], [2, 5]]: (9 +/- sqrt(17))/2, ascending
assert abs(eigSym[0] - (9.0 - math.sqrt(17.0)) / 2.0) < 1e-12
assert abs(eigSym[1] - (9.0 + math.sqrt(17.0)) / 2.0) < 1e-12
print("ComputeEigenValuesSymmetric: OK")

# Eigenvalues of a square matrix are returned as real/imag pairs
# rotation [[0, -1], [1, 0]] has eigenvalues +/- i
rot = ot.DataContainer(2, 2, 0.0)
rot[0] = 0.0
rot[1] = 1.0
rot[2] = -1.0
rot[3] = 0.0
eigRot = ot.AlgebraEngine.ComputeEigenValuesSquare(rot)
assert len(eigRot) == 4
pairs = [(eigRot[0], eigRot[1]), (eigRot[2], eigRot[3])]
for re, im in pairs:
    assert abs(re) < 1e-12
    assert abs(abs(im) - 1.0) < 1e-12
print("ComputeEigenValuesSquare (complex pairs): OK")

# Singular values
Ssg = ot.DataContainer(2, 2, 0.0)
Ssg[0] = 4.0
Ssg[3] = 2.0
sv = ot.AlgebraEngine.ComputeSingularValues(Ssg)
assert len(sv) == 2
assert abs(sv[0] - 4.0) < 1e-12
assert abs(sv[1] - 2.0) < 1e-12
print("ComputeSingularValues: OK")

# IsSymmetric
assert ot.AlgebraEngine.IsSymmetric(spd)
notSym = ot.DataContainer(2, 2, 0.0)
notSym[0] = 1.0
notSym[1] = 2.0
notSym[2] = 3.0
notSym[3] = 4.0
assert not ot.AlgebraEngine.IsSymmetric(notSym)
print("IsSymmetric: OK")

# IsPositiveDefinite
assert ot.AlgebraEngine.IsPositiveDefinite(spd)
notPD = ot.DataContainer(2, 2, 0.0)
notPD[0] = 0.0
notPD[1] = 1.0
notPD[2] = 1.0
notPD[3] = 0.0
assert not ot.AlgebraEngine.IsPositiveDefinite(notPD)
print("IsPositiveDefinite: OK")

# FrobeniusNorm
fn = ot.AlgebraEngine.FrobeniusNorm(A2)
assert abs(fn - math.sqrt(30.0)) < 1e-12
print("FrobeniusNorm: OK")

# SumElements
se = ot.AlgebraEngine.SumElements(A2)
assert abs(se - 10.0) < 1e-12
print("SumElements: OK")

# Clean
Adirty = ot.DataContainer(2, 2, 0.0)
Adirty[0] = 1e-15
Adirty[1] = 1.0
Adirty[2] = -1e-13
Adirty[3] = 2.0
Aclean = ot.AlgebraEngine.Clean(Adirty, 1e-10)
assert Aclean.getSize() == 2
assert Aclean.getDimension() == 2
for idx in [0, 2]:
    assert Aclean[idx] == 0.0
assert abs(Aclean[1] - 1.0) < 1e-15
assert abs(Aclean[3] - 2.0) < 1e-15
print("Clean: OK")

# ComputeGram (2x2 matrix → 2x2 Gram)
G = ot.AlgebraEngine.ComputeGram(A2, True)
assert G.getSize() == 2
assert G.getDimension() == 2
print("ComputeGram: OK")

# ComputeQR
A3 = ot.DataContainer(3, 2, 0.0)
A3[0] = 1.0
A3[1] = 2.0
A3[2] = 3.0
A3[3] = 4.0
A3[4] = 5.0
A3[5] = 6.0
Q = ot.DataContainer()
R = ot.DataContainer()
ot.AlgebraEngine.ComputeQR(A3, Q, R)
assert Q.getSize() == 3
assert Q.getDimension() == 2
assert R.getSize() == 2
assert R.getDimension() == 2
# A = Q R and Q^T Q = I
QRcheck = ot.AlgebraEngine.MatrixProduct(Q, R)
for i in range(len(A3)):
    assert abs(QRcheck[i] - A3[i]) < 1e-10
QtQ = ot.AlgebraEngine.MatrixProduct(ot.AlgebraEngine.Transpose(Q), Q)
Id_check = ot.DataContainer(2, 2, 0.0)
Id_check[0] = 1.0
Id_check[3] = 1.0
for i in range(4):
    assert abs(QtQ[i] - Id_check[i]) < 1e-10
# fullQR: Q becomes 3 x 3
Qf = ot.DataContainer()
Rf = ot.DataContainer()
ot.AlgebraEngine.ComputeQR(A3, Qf, Rf, True)
assert Qf.getSize() == 3
assert Qf.getDimension() == 3
print("ComputeQR (economy + full): OK")

# ComputeQR on a wide matrix (thin Q: 2 x 2 for a 2 x 3 A)
A4 = ot.DataContainer(2, 3, 0.0)
A4[0] = 1.0
A4[1] = 4.0
A4[2] = 2.0
A4[3] = 5.0
A4[4] = 3.0
A4[5] = 6.0
Qw = ot.DataContainer()
Rw = ot.DataContainer()
ot.AlgebraEngine.ComputeQR(A4, Qw, Rw)
assert Qw.getSize() == 2
assert Qw.getDimension() == 2
assert Rw.getSize() == 2
assert Rw.getDimension() == 3
QRw = ot.AlgebraEngine.MatrixProduct(Qw, Rw)
for i in range(len(A4)):
    assert abs(QRw[i] - A4[i]) < 1e-10
Qfw = ot.DataContainer()
Rfw = ot.DataContainer()
ot.AlgebraEngine.ComputeQR(A4, Qfw, Rfw, True)
assert Qfw.getSize() == 2
assert Qfw.getDimension() == 2
print("ComputeQR (wide matrix): OK")

# ComputeSVD: A = U S VT, U^T U = I, VT^T VT = I
U = ot.DataContainer()
S = ot.DataContainer()
VT = ot.DataContainer()
ot.AlgebraEngine.ComputeSVD(A3, U, S, VT)
assert S.getSize() == 2
assert S[0] >= S[1]
Sdiag = ot.DataContainer(2, 2, 0.0)
Sdiag[0] = S[0]
Sdiag[3] = S[1]
USVT = ot.AlgebraEngine.MatrixProduct(U, ot.AlgebraEngine.MatrixProduct(Sdiag, VT))
for i in range(len(A3)):
    assert abs(USVT[i] - A3[i]) < 1e-9
UtU = ot.AlgebraEngine.MatrixProduct(ot.AlgebraEngine.Transpose(U), U)
for i in range(4):
    assert abs(UtU[i] - Id_check[i]) < 1e-10
VtV = ot.AlgebraEngine.MatrixProduct(VT, ot.AlgebraEngine.Transpose(VT))
for i in range(4):
    assert abs(VtV[i] - Id_check[i]) < 1e-10
# fullSVD: U is 3 x 3
Uf = ot.DataContainer()
Sf = ot.DataContainer()
VTf = ot.DataContainer()
ot.AlgebraEngine.ComputeSVD(A3, Uf, Sf, VTf, True)
assert Uf.getSize() == 3
assert Uf.getDimension() == 3
print("ComputeSVD (economy + full): OK")

# SolveLinearSystem (rectangular, least-squares): min ||A x - b||
Arect = ot.DataContainer(3, 3, 0.0)
Arect[0] = 1.0
Arect[1] = 4.0
Arect[2] = 7.0
Arect[3] = 2.0
Arect[4] = 5.0
Arect[5] = 8.0
Arect[6] = 3.0
Arect[7] = 6.0
Arect[8] = 10.0
brect = ot.DataContainer(3, 1.0)
xrect = ot.AlgebraEngine.SolveLinearSystemRectangular(Arect, brect)
assert abs(xrect[1] - 1.0) < 1e-10
xrectB = ot.AlgebraEngine.SolveLinearSystemRectangularBlockwise(Arect, brect, 1)
assert abs(xrectB[1] - 1.0) < 1e-10
print("SolveLinearSystemRectangular(+Blockwise): OK")

# SolveLinearSystemTriangular
tri = ot.DataContainer(2, 2, 0.0)
tri[0] = 2.0
tri[1] = 0.0
tri[2] = 1.0
tri[3] = 3.0
btri = ot.DataContainer(2, 1.0)
xtri = ot.AlgebraEngine.SolveLinearSystemTriangular(tri, btri, True, False)
assert abs(xtri[0] - 0.5) < 1e-12
print("SolveLinearSystemTriangular: OK")

# Block methods (blockSize=1 matches C++ test coverage)
spd2 = ot.DataContainer(2, 2, 0.0)
spd2[0] = 4.0
spd2[1] = 2.0
spd2[2] = 2.0
spd2[3] = 5.0

Gblock = ot.AlgebraEngine.ComputeGramBlockwise(spd2, True, 1)
Gdirect = ot.AlgebraEngine.ComputeGram(spd2, True)
for i in range(len(Gblock)):
    assert abs(Gblock[i] - Gdirect[i]) < 1e-10
print("ComputeGramBlockwise: OK")

Lblock = ot.AlgebraEngine.ComputeCholeskyBlockwise(spd2, 1)
# Correct Cholesky: L = [[2,0],[1,2]], column-major = [2, 1, 0, 2]
assert abs(Lblock[0] - 2.0) < 1e-12
assert abs(Lblock[1] - 1.0) < 1e-12
assert abs(Lblock[2] - 0.0) < 1e-12
assert abs(Lblock[3] - 2.0) < 1e-12
print("ComputeCholeskyBlockwise: OK")

# Block methods: test at multiple block sizes
for bs in [1, 2]:
    # CholeskyBlockwise: verify L * L^T = A
    Lb = ot.AlgebraEngine.ComputeCholeskyBlockwise(spd2, bs)
    LLT = ot.AlgebraEngine.MatrixProduct(Lb, ot.AlgebraEngine.Transpose(Lb))
    for i in range(len(spd2)):
        assert abs(LLT[i] - spd2[i]) < 1e-10
    print(f"  CholeskyBlockwise(bs={bs}): L*L^T=A OK")

    # DeterminantBlockwise
    detb = ot.AlgebraEngine.ComputeDeterminantBlockwise(spd2, bs)
    assert abs(detb - 16.0) < 1e-10
    print(f"  DeterminantBlockwise(bs={bs}): OK")

    # InverseBlockwise: verify A * inv(A) = I
    invb = ot.AlgebraEngine.InverseBlockwise(spd2, bs)
    Ainv = ot.AlgebraEngine.MatrixProduct(spd2, invb)
    assert abs(Ainv[0] - 1.0) < 1e-10
    assert abs(Ainv[1] - 0.0) < 1e-10
    assert abs(Ainv[2] - 0.0) < 1e-10
    assert abs(Ainv[3] - 1.0) < 1e-10
    print(f"  InverseBlockwise(bs={bs}): OK")

    # InverseSPDBlockwise
    invSPDb = ot.AlgebraEngine.InverseSPDBlockwise(spd2, bs)
    AinvSPD = ot.AlgebraEngine.MatrixProduct(spd2, invSPDb)
    assert abs(AinvSPD[0] - 1.0) < 1e-10
    assert abs(AinvSPD[1] - 0.0) < 1e-10
    assert abs(AinvSPD[2] - 0.0) < 1e-10
    assert abs(AinvSPD[3] - 1.0) < 1e-10
    print(f"  InverseSPDBlockwise(bs={bs}): OK")

    # SolveLinearSystemBlockwise: A * x = b
    xb = ot.AlgebraEngine.SolveLinearSystemBlockwise(spd2, b, bs)
    Axb = ot.AlgebraEngine.MatrixPointProduct(spd2, xb)
    assert abs(Axb[0] - 1.0) < 1e-10
    assert abs(Axb[1] - 1.0) < 1e-10
    print(f"  SolveLinearSystemBlockwise(bs={bs}): OK")

    # ComputeGramBlockwise (transpose=True)
    Gb = ot.AlgebraEngine.ComputeGramBlockwise(spd2, True, bs)
    Gref = ot.AlgebraEngine.ComputeGram(spd2, True)
    for i in range(len(Gb)):
        assert abs(Gb[i] - Gref[i]) < 1e-10
    print(f"  ComputeGramBlockwise(bs={bs}): OK")

    # ComputeGramBlockwise (transpose=False)
    Gntb = ot.AlgebraEngine.ComputeGramBlockwise(spd2, False, bs)
    Gntref = ot.AlgebraEngine.ComputeGram(spd2, False)
    for i in range(len(Gntb)):
        assert abs(Gntb[i] - Gntref[i]) < 1e-10
    print(f"  ComputeGramBlockwise(NT,bs={bs}): OK")

    # MatrixProductBlockwise
    ABb = ot.AlgebraEngine.MatrixProductBlockwise(spd2, spd2, bs)
    ABref = ot.AlgebraEngine.MatrixProduct(spd2, spd2)
    for i in range(len(ABb)):
        assert abs(ABb[i] - ABref[i]) < 1e-10
    print(f"  MatrixProductBlockwise(bs={bs}): OK")

    # ComputeLUBlockwise: P * A = L * U
    Pb = ot.DataContainer()
    LUb = ot.DataContainer()
    UUb = ot.DataContainer()
    ot.AlgebraEngine.ComputeLUBlockwise(spd2, Pb, LUb, UUb, bs)
    PA = ot.AlgebraEngine.MatrixProduct(Pb, spd2)
    LU = ot.AlgebraEngine.MatrixProduct(LUb, UUb)
    for i in range(len(PA)):
        assert abs(PA[i] - LU[i]) < 1e-10
    print(f"  ComputeLUBlockwise(bs={bs}): OK")

    # SolveLinearSystemTriangularBlockwise
    xt = ot.AlgebraEngine.SolveLinearSystemTriangularBlockwise(Lb, b, True, False, bs)
    Lxt = ot.AlgebraEngine.MatrixProduct(Lb, xt)
    for i in range(len(b)):
        assert abs(Lxt[i] - b[i]) < 1e-10
    print(f"  SolveLinearSystemTriangularBlockwise(bs={bs}): OK")

print("All block method tests passed!")

print("All AlgebraEngine tests passed!")

# ============================================================
# StatisticsEngine tests
# ============================================================
print("\n=== StatisticsEngine Tests ===")

sample = ot.DataContainer(4, 2, 0.0, ot.DataContainer.ROW_MAJOR)
sample[0] = 1.0
sample[1] = 2.0
sample[2] = 3.0
sample[3] = 4.0
sample[4] = 5.0
sample[5] = 6.0
sample[6] = 7.0
sample[7] = 8.0

# Mean
mean = ot.StatisticsEngine.ComputeMean(sample)
assert abs(mean[0] - 4.0) < 1e-12
assert abs(mean[1] - 5.0) < 1e-12
print("ComputeMean: OK")

# Variance
var = ot.StatisticsEngine.ComputeVariance(sample)
assert abs(var[0] - 20.0 / 3.0) < 1e-12
assert abs(var[1] - 20.0 / 3.0) < 1e-12
print("ComputeVariance: OK")

# StandardDeviation
std = ot.StatisticsEngine.ComputeStandardDeviation(sample)
assert abs(std[0] - math.sqrt(20.0 / 3.0)) < 1e-12
print("ComputeStandardDeviation: OK")

# Covariance
cov = ot.StatisticsEngine.ComputeCovariance(sample)
assert cov.getSize() == 2
assert cov.getDimension() == 2
print("ComputeCovariance: OK")

# PearsonCorrelation
corr = ot.StatisticsEngine.ComputePearsonCorrelation(sample)
assert abs(corr[0] - 1.0) < 1e-12
assert abs(corr[3] - 1.0) < 1e-12
print("ComputePearsonCorrelation: OK")

# SpearmanCorrelation (monotone and non-monotone samples)
sp_r = ot.DataContainer(4, 2, 0.0, ot.DataContainer.ROW_MAJOR)
sp_r[0] = 1.0
sp_r[1] = 1.0
sp_r[2] = 2.0
sp_r[3] = 4.0
sp_r[4] = 3.0
sp_r[5] = 9.0
sp_r[6] = 4.0
sp_r[7] = 16.0
sp_corr = ot.StatisticsEngine.ComputeSpearmanCorrelation(sp_r)
assert abs(sp_corr[0] - 1.0) < 1e-12   # strictly increasing in both columns
assert abs(sp_corr[3] - 1.0) < 1e-12
print("ComputeSpearmanCorrelation: OK")

# Min / Max
mn = ot.StatisticsEngine.ComputeMin(sample)
mx = ot.StatisticsEngine.ComputeMax(sample)
assert mn[0] == 1.0
assert mn[1] == 2.0
assert mx[0] == 7.0
assert mx[1] == 8.0
print("ComputeMin/Max: OK")

# Sum
sm = ot.StatisticsEngine.ComputeSum(sample)
assert sm[0] == 16.0
assert sm[1] == 20.0
print("ComputeSum: OK")

# Quantile
q = ot.StatisticsEngine.ComputeQuantile(sample, 0.5)
assert q[0] == 4.0
assert q[1] == 5.0
print("ComputeQuantile: OK")

# Sort
sorted_sample = ot.StatisticsEngine.Sort(sample, 0)
assert sorted_sample[0] == 1.0
assert sorted_sample[6] == 7.0
print("Sort: OK")

# RawMoment
rm = ot.StatisticsEngine.ComputeRawMoment(sample, 2)
print("RawMoment(2):", rm[0], rm[1])
print("ComputeRawMoment: OK")

# CentralMoment
cm = ot.StatisticsEngine.ComputeCentralMoment(sample, 2)
assert abs(cm[0] - 20.0 / 4.0) < 1e-12
print("ComputeCentralMoment: OK")

# Skewness
sk = ot.StatisticsEngine.ComputeSkewness(sample)
print("ComputeSkewness: OK")

# Kurtosis
ku = ot.StatisticsEngine.ComputeKurtosis(sample)
print("ComputeKurtosis: OK")

# Blockwise methods
meanBlock = ot.StatisticsEngine.ComputeMeanBlockwise(sample)
assert abs(meanBlock[0] - mean[0]) < 1e-12
assert abs(meanBlock[1] - mean[1]) < 1e-12
print("ComputeMeanBlockwise: OK")

varBlock = ot.StatisticsEngine.ComputeVarianceBlockwise(sample)
assert abs(varBlock[0] - var[0]) < 1e-12
print("ComputeVarianceBlockwise: OK")

covBlock = ot.StatisticsEngine.ComputeCovarianceBlockwise(sample)
for i in range(len(cov)):
    assert abs(covBlock[i] - cov[i]) < 1e-10
print("ComputeCovarianceBlockwise: OK")

print("All StatisticsEngine tests passed!")
print("\n=== ALL PYTHON TESTS PASSED ===")
