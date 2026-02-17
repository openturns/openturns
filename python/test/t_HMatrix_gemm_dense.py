#! /usr/bin/env python

import openturns as ot
import math as m
import itertools
import openturns.testing as ott


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


def compute_norm_error(
    side: str,
    trans_b: str,
    trans_a: str,
    alpha: float,
    hmat : ot.HMatrix,
    chol_dense: ot.TriangularMatrix,
    random_sample_mat,
) -> float:
    """Computes the norm error between the GEMM product from HMatrix and a reference matrix product."""
    # 1. Compute result using HMatrix method
    epsilon_hmat = hmat.gemm_dense(trans_b, trans_a, side, alpha, random_sample_mat)

    # 2. Determine transpose flags based on side
    transpose_result = False

    if side == "R":
        transpose_result = True
        trans_a_flag = trans_a != "T"
        trans_b_flag = trans_b != "T"
    else:
        trans_b_flag = trans_b == "T"
        trans_a_flag = trans_a == "T"

    # 3. Handle transpose for operand matrices
    left = ot.Matrix(chol_dense.transpose()) if trans_b_flag else ot.Matrix(chol_dense)
    right = ot.Matrix(random_sample_mat.transpose()) if trans_a_flag else ot.Matrix(random_sample_mat)

    # 4. Matrix multiplication for reference product
    epsilon_mat = left * right

    if transpose_result:
        epsilon_mat = epsilon_mat.transpose()

    # 5. Calculate norm difference: element-wise square, sum, and square root
    diff = epsilon_hmat - epsilon_mat
    diff.squareElements()

    sample_mat_rows = random_sample_mat.getNbRows()
    norm_diff = m.sqrt(diff.computeSumElements() / sample_mat_rows)
    return norm_diff


ot.ResourceMap.SetAsBool("HMatrix-ForceSequential", True)
ot.ResourceMap.SetAsUnsignedInteger("HMatrix-MaxLeafSize", 10)

ot.PlatformInfo.SetNumericalPrecision(3)

n = 10
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
simpleAssembly = TestHMatrixRealAssemblyFunction(vertices, 0.05)

hmat.assembleReal(simpleAssembly, "L")

hmatRef = ot.HMatrix(hmat)

hmat.factorize("LLt")

# Create a full dense matrix
fullDense = ot.CovarianceMatrix(mesh2D.getVertices().getSize())
for j in range(fullDense.getNbColumns()):
    for i in range(fullDense.getNbRows()):
        fullDense[i, j] = simpleAssembly(i, j)
# Reference cholsesky
cholDense = fullDense.computeCholesky()

side_transB_transA = itertools.product(['L', 'R'], ['N', 'T'], ['N', 'T'])

# First check with identity
randomSampleMat = ot.IdentityMatrix(hmat.getNbRows())

# For identity matrix, hmat * eye,  eye * hmat, hmat.T * eye, eye * hmat.T provide has the same norm
norms = []
for side, transB, transA in side_transB_transA:
    norm_diff = compute_norm_error(side, transB, transA, 1.0, hmat, cholDense, randomSampleMat)
    ott.assert_almost_equal(norm_diff, 2.e-5, 1e-3, 1e-3)

    norms.append(norm_diff)

# assert norm.std is 0
sigma = ot.Sample.BuildFromPoint(norms).computeStandardDeviation()[0]
ott.assert_almost_equal(sigma, 0, 1e-12, 1e-12)

# Now create a rectangular & random matrix
nSamples = 100
randomSample = ot.DistFunc.rNormal(nSamples * hmat.getNbColumns())
randomSampleMat = ot.Matrix(hmat.getNbColumns(), nSamples, randomSample)

# Case a : side = 'L', transA = 'N', transB = 'N'
norms = []
norm_diff = compute_norm_error('L', 'N', 'N', 1.0, hmat, cholDense, randomSampleMat)
norms.append(norm_diff)
ott.assert_almost_equal(norm_diff, 0.0020, 1e-3, 1e-3)


# Case b : side = 'R', transA = 'T', transB = 'T' (same result as case a expected)
norm_diff = compute_norm_error('R', 'T', 'T', 1.0, hmat, cholDense, randomSampleMat)
norms.append(norm_diff)
ott.assert_almost_equal(norm_diff, 0.0020, 1e-3, 1e-3)

# check the results are the same
sigma = ot.Sample.BuildFromPoint(norms).computeStandardDeviation()[0]
ott.assert_almost_equal(sigma, 0, 1e-12, 1e-12)

norms = []
# Case c : side = 'L', transA = 'N', transB = 'T'
norm_diff = compute_norm_error('L', 'T', 'N', 1.0, hmat, cholDense, randomSampleMat)
ott.assert_almost_equal(norm_diff, 0.0020, 1e-3, 1e-3)
norms.append(norm_diff)

# Case d : side = 'R', transA = 'T', transB = 'N' (same result as case c expected)
norm_diff = compute_norm_error('R', 'N', 'T', 1.0, hmat, cholDense, randomSampleMat)
ott.assert_almost_equal(norm_diff, 0.0020, 1e-3, 1e-3)
norms.append(norm_diff)
# check the results are the same
sigma = ot.Sample.BuildFromPoint(norms).computeStandardDeviation()[0]
ott.assert_almost_equal(sigma, 0, 1e-12, 1e-12)
