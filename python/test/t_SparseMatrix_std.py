#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# default constructor
matrix0 = ot.SparseMatrix()
assert matrix0.getNbRows() == 0, "rows"
assert matrix0.getNbColumns() == 0, "columns"
assert matrix0.getNbNonZeros() == 0, "non zeros"
assert matrix0.getColumnPointer() == [0], "column pointer"

# constructor with size, item accessors
matrix1 = ot.SparseMatrix(4, 5)
matrix1[0, 2] = 5.0
matrix1[1, 0] = 8.0
matrix1[2, 4] = 2.0
matrix1[3, 2] = 9.0
assert matrix1.getNbNonZeros() == 4, "non zeros"
assert matrix1[1, 0] == 8.0, "value"
assert matrix1[1, 1] == 0.0, "zero value"
assert matrix1.getColumnPointer() == [0, 1, 1, 3, 3, 4], "column pointer"
assert matrix1.getRowIndices() == [1, 0, 3, 2], "row indices"
ott.assert_almost_equal(matrix1.getValues(), [8.0, 5.0, 9.0, 2.0])
ott.assert_almost_equal(
    matrix1.asDenseMatrix(),
    ot.Matrix([[0.0, 0.0, 5.0, 0.0, 0.0], [8.0, 0.0, 0.0, 0.0, 0.0],
               [0.0, 0.0, 0.0, 0.0, 2.0], [0.0, 0.0, 9.0, 0.0, 0.0]]),
)

# canonical triplet constructor: unsorted rows are sorted and duplicates summed
matrix2 = ot.SparseMatrix(
    3, 2, [2, 0, 0, 1], [1, 0, 0, 0], [2.0, 3.0, 1.0, 4.0]
)
assert matrix2.getNbNonZeros() == 3, "non zeros"
assert matrix2.getColumnPointer() == [0, 2, 3], "column pointer"
assert matrix2.getRowIndices() == [0, 1, 2], "row indices"
ott.assert_almost_equal(matrix2.getValues(), [4.0, 4.0, 2.0])
ott.assert_almost_equal(
    matrix2.asDenseMatrix(), ot.Matrix([[4.0, 0.0], [4.0, 0.0], [0.0, 2.0]])
)
assert matrix2[0, 0] == 4.0, "summed duplicate"
assert (
    str(matrix2)
    == "class=SparseMatrix rows=3 columns=2 triplets=[[0,0,4],[1,0,4],[2,1,2]]"
), "string converter"

# copy constructor
matrix2b = ot.SparseMatrix(matrix2)
assert matrix2b is not matrix2, "copy"
assert matrix2b.getColumnPointer() == matrix2.getColumnPointer(), "copy"
assert matrix2b.getRowIndices() == matrix2.getRowIndices(), "copy"
ott.assert_almost_equal(matrix2b.getValues(), matrix2.getValues())
matrix2b[0, 0] = -1.0
assert matrix2[0, 0] == 4.0, "copy is independent"

# assignment inserts the coefficient at its sorted place, and overwrites it
ordered = ot.SparseMatrix(3, 1)
ordered[2, 0] = 3.0
ordered[0, 0] = 1.0
ordered[1, 0] = 2.0
assert ordered.getRowIndices() == [0, 1, 2], "row indices"
ott.assert_almost_equal(ordered.getValues(), [1.0, 2.0, 3.0])
assert ordered.getNbNonZeros() == 3, "non zeros"
ordered[1, 0] = 5.0
assert ordered.getNbNonZeros() == 3, "non zeros"
ott.assert_almost_equal(ordered.getValues(), [1.0, 5.0, 3.0])

# conversion from a dense matrix, zeros are skipped
matrix3 = ot.SparseMatrix.FromDenseMatrix(matrix1.asDenseMatrix())
assert matrix3.getNbNonZeros() == 4, "non zeros"
assert matrix3.getColumnPointer() == matrix1.getColumnPointer(), "column pointer"
assert matrix3.getRowIndices() == matrix1.getRowIndices(), "row indices"
ott.assert_almost_equal(matrix3.getValues(), matrix1.getValues())
ott.assert_almost_equal(matrix3.asDenseMatrix(), matrix1.asDenseMatrix())
assert ot.SparseMatrix.FromDenseMatrix(ot.Matrix(2, 2)).getNbNonZeros() == 0

# transposition
matrix4 = matrix2.transpose()
assert matrix4.getNbRows() == 2, "rows"
assert matrix4.getNbColumns() == 3, "columns"
assert matrix4.getColumnPointer() == [0, 1, 2, 3], "column pointer"
assert matrix4.getRowIndices() == [0, 0, 1], "row indices"
ott.assert_almost_equal(
    matrix4.asDenseMatrix(), ot.Matrix([[4.0, 4.0, 0.0], [0.0, 0.0, 2.0]])
)

# products
ott.assert_almost_equal(matrix2 * ot.Point([1.0, 2.0]), [4.0, 4.0, 4.0])
dense = ot.Matrix([[1.0, 2.0], [3.0, 4.0]])
ott.assert_almost_equal(matrix2 * dense, matrix2.asDenseMatrix() * dense)
left = ot.Matrix([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
ott.assert_almost_equal(
    matrix2.multiplyByDenseOnLeft(left), left * matrix2.asDenseMatrix()
)
sym = ot.SymmetricMatrix(3)
for i in range(3):
    sym[i, i] = 2.0
sym[0, 1] = 1.0
sym[0, 2] = 1.0
sym[1, 2] = 1.0
symDense = ot.Matrix([[2.0, 1.0, 1.0], [1.0, 2.0, 1.0], [1.0, 1.0, 2.0]])
ott.assert_almost_equal(
    matrix2.multiplyBySymmetricOnLeft(sym), symDense * matrix2.asDenseMatrix()
)

# explicit zeros are stored as triplets, empty matrices are supported
zeroMatrix = ot.SparseMatrix(2, 2, [0, 1], [0, 1], [0.0, 2.0])
assert zeroMatrix.getNbNonZeros() == 2, "non zeros"
ott.assert_almost_equal(zeroMatrix.asDenseMatrix(), ot.Matrix([[0.0, 0.0], [0.0, 2.0]]))
emptyMatrix = ot.SparseMatrix(0, 3)
assert emptyMatrix.getColumnPointer() == [0, 0, 0, 0], "column pointer"
assert emptyMatrix.getRowIndices() == [], "row indices"
assert emptyMatrix.getValues() == [], "values"

# out of bound triplets are rejected
with ott.assert_raises(IndexError):
    ot.SparseMatrix(2, 2, [2], [0], [1.0])
with ott.assert_raises(IndexError):
    ot.SparseMatrix(2, 2, [0], [2], [1.0])
with ott.assert_raises(TypeError):
    ot.SparseMatrix(2, 2, [0], [0, 1], [1.0])
with ott.assert_raises(IndexError):
    matrix2[3, 0] = 1.0
with ott.assert_raises(IndexError):
    matrix2[0, 2]

# persistence
if ot.PlatformInfo.HasFeature("libxml2"):
    fileName = "sparse_matrix_study.xml"
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.add("matrix", matrix2)
    study.save()
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.load()
    matrix5 = ot.SparseMatrix()
    study.fillObject("matrix", matrix5)
    os.remove(fileName)
    assert matrix5.getColumnPointer() == matrix2.getColumnPointer(), "column pointer"
    assert matrix5.getRowIndices() == matrix2.getRowIndices(), "row indices"
    ott.assert_almost_equal(matrix5.getValues(), matrix2.getValues())

print("OK")
