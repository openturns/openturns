#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import os
import re
import struct

ot.TESTPREAMBLE()

# default constructor
matrix0 = otexp.SparseMatrix()
assert matrix0.getNbRows() == 0, "rows"
assert matrix0.getNbColumns() == 0, "columns"
assert matrix0.getNbNonZeros() == 0, "non zeros"
assert matrix0.getColumnPointer() == [0], "column pointer"

# constructor with size, item accessors
matrix1 = otexp.SparseMatrix(4, 5)
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

# negative indices are counted from the end, as for a dense matrix
assert matrix1[-3, 0] == 8.0, "negative row index"
assert matrix1[-2, -1] == 2.0, "negative column index"
negativeMatrix = otexp.SparseMatrix(4, 5)
negativeMatrix[-1, -1] = 4.0
assert negativeMatrix[3, 4] == 4.0, "negative index assignment"

# canonical triplet constructor: unsorted rows are sorted and duplicates summed
matrix2 = otexp.SparseMatrix(
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

# duplicate coordinates are summed in input order, whatever their values
cancelling = otexp.SparseMatrix(
    1, 1, [0, 0, 0], [0, 0, 0], [1e16, -1e16, 1.0]
)
assert cancelling[0, 0] == 1.0, "duplicates are summed in input order"

# the string converter has no trailing comma when the last columns are empty
trailing = otexp.SparseMatrix(2, 3, [0], [1], [1.0])
assert (
    str(trailing) == "class=SparseMatrix rows=2 columns=3 triplets=[[0,1,1]]"
), "string converter without trailing comma"
assert (
    str(otexp.SparseMatrix()) == "class=SparseMatrix rows=0 columns=0 triplets=[]"
), "string converter of the default matrix"

# copy constructor
matrix2b = otexp.SparseMatrix(matrix2)
assert matrix2b is not matrix2, "copy"
assert matrix2b.getColumnPointer() == matrix2.getColumnPointer(), "copy"
assert matrix2b.getRowIndices() == matrix2.getRowIndices(), "copy"
ott.assert_almost_equal(matrix2b.getValues(), matrix2.getValues())
matrix2b[0, 0] = -1.0
assert matrix2[0, 0] == 4.0, "copy is independent"

# assignment inserts the coefficient at its sorted place, and overwrites it
ordered = otexp.SparseMatrix(3, 1)
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
matrix3 = otexp.SparseMatrix.FromDenseMatrix(matrix1.asDenseMatrix())
assert matrix3.getNbNonZeros() == 4, "non zeros"
assert matrix3.getColumnPointer() == matrix1.getColumnPointer(), "column pointer"
assert matrix3.getRowIndices() == matrix1.getRowIndices(), "row indices"
ott.assert_almost_equal(matrix3.getValues(), matrix1.getValues())
ott.assert_almost_equal(matrix3.asDenseMatrix(), matrix1.asDenseMatrix())
assert otexp.SparseMatrix.FromDenseMatrix(ot.Matrix(2, 2)).getNbNonZeros() == 0

# transposition
matrix4 = matrix2.transpose()
assert matrix4.getNbRows() == 2, "rows"
assert matrix4.getNbColumns() == 3, "columns"
assert matrix4.getColumnPointer() == [0, 1, 2, 3], "column pointer"
assert matrix4.getRowIndices() == [0, 0, 1], "row indices"
ott.assert_almost_equal(
    matrix4.asDenseMatrix(), ot.Matrix([[4.0, 4.0, 0.0], [0.0, 0.0, 2.0]])
)
emptyTranspose = otexp.SparseMatrix(0, 3).transpose()
assert emptyTranspose.getNbRows() == 3, "transposed rows"
assert emptyTranspose.getNbColumns() == 0, "transposed columns"
assert emptyTranspose.getColumnPointer() == [0], "transposed column pointer"

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
zeroMatrix = otexp.SparseMatrix(2, 2, [0, 1], [0, 1], [0.0, 2.0])
assert zeroMatrix.getNbNonZeros() == 2, "non zeros"
ott.assert_almost_equal(zeroMatrix.asDenseMatrix(), ot.Matrix([[0.0, 0.0], [0.0, 2.0]]))
emptyMatrix = otexp.SparseMatrix(0, 3)
assert emptyMatrix.getColumnPointer() == [0, 0, 0, 0], "column pointer"
assert emptyMatrix.getRowIndices() == [], "row indices"
assert emptyMatrix.getValues() == [], "values"
assert (
    str(emptyMatrix) == "class=SparseMatrix rows=0 columns=3 triplets=[]"
), "string converter of an empty matrix"

# out of bound indices are rejected
with ott.assert_raises(IndexError):
    otexp.SparseMatrix(2, 2, [2], [0], [1.0])
with ott.assert_raises(IndexError):
    otexp.SparseMatrix(2, 2, [0], [2], [1.0])
with ott.assert_raises(TypeError):
    otexp.SparseMatrix(2, 2, [0], [0, 1], [1.0])
with ott.assert_raises(IndexError):
    matrix2[3, 0] = 1.0
with ott.assert_raises(IndexError):
    matrix2[0, 2]
with ott.assert_raises(IndexError):
    matrix2[-4, 0]
with ott.assert_raises(IndexError):
    matrix2[0, -3]
with ott.assert_raises(TypeError):
    matrix2[0]
with ott.assert_raises(TypeError):
    matrix2[0.0, 0]
with ott.assert_raises(TypeError):
    matrix2[0] = 1.0
with ott.assert_raises(TypeError):
    matrix2[0.0, 0] = 1.0

# persistence
if ot.PlatformInfo.HasFeature("libxml2"):
    fileName = "sparse_matrix_study.xml"

    def saveMatrixContent():
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(fileName))
        study.add("matrix", matrix2)
        study.save()
        with open(fileName) as stream:
            return stream.read()

    def getMemberId(content, memberName):
        return re.search(
            r'<object member="%s" id="(\d+)"/>' % memberName, content
        ).group(1)

    def replaceIndices(content, objectId, entries):
        body = "\n".join(
            '    <unsignedlong index="%d">%d</unsignedlong>' % (index, value)
            for index, value in enumerate(entries)
        )
        if entries:
            element = (
                '  <object class="Indices" id="%s" size="%d" '
                'StudyVisible="false" StudyLabel="">\n%s\n  </object>'
                % (objectId, len(entries), body)
            )
        else:
            element = (
                '  <object class="Indices" id="%s" size="0" '
                'StudyVisible="false" StudyLabel=""/>' % objectId
            )
        return re.sub(
            r'  <object class="Indices" id="%s"[^>]*(?:/>|>.*?</object>)' % objectId,
            element,
            content,
            flags=re.S,
        )

    def replaceValues(content, objectId, entries):
        body = "\n".join(
            '    <numericalscalar index="%d">%r</numericalscalar>' % (index, value)
            for index, value in enumerate(entries)
        )
        element = (
            '  <object class="Point" id="%s" size="%d" '
            'StudyVisible="false" StudyLabel="">\n%s\n  </object>'
            % (objectId, len(entries), body)
        )
        return re.sub(
            r'  <object class="Point" id="%s"[^>]*(?:/>|>.*?</object>)' % objectId,
            element,
            content,
            flags=re.S,
        )

    def assertRejectedStudy(content, rejectedMatrix):
        with open(fileName, "w") as stream:
            stream.write(content)
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(fileName))
        study.load()
        with ott.assert_raises(Exception):
            study.fillObject("matrix", rejectedMatrix)

    savedContent = saveMatrixContent()
    columnPointerId = getMemberId(savedContent, "columnPointer_")
    rowIndexId = getMemberId(savedContent, "rowIndex_")
    valuesId = getMemberId(savedContent, "values_")

    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))
    study.load()
    matrix5 = otexp.SparseMatrix()
    study.fillObject("matrix", matrix5)
    assert matrix5.getColumnPointer() == matrix2.getColumnPointer(), "column pointer"
    assert matrix5.getRowIndices() == matrix2.getRowIndices(), "row indices"
    ott.assert_almost_equal(matrix5.getValues(), matrix2.getValues())

    # a corrupted study cannot make the CSC accessors read out of bounds: an
    # empty column pointer with the largest column count used to wrap the
    # columnPointer.getSize() == nbColumns + 1 check
    maxUnsignedInteger = 2 ** (8 * struct.calcsize("L")) - 1
    overflowContent = re.sub(
        r'(<object class="SparseMatrix"[^>]*nbColumns_=")\d+(")',
        r"\g<1>%d\g<2>" % maxUnsignedInteger,
        savedContent,
    )
    unchangedMatrix = otexp.SparseMatrix(3, 2)
    unchangedMatrix[1, 0] = 5.0
    assertRejectedStudy(replaceIndices(overflowContent, columnPointerId, []), unchangedMatrix)
    # a failed load leaves the target matrix unchanged
    assert unchangedMatrix.getNbNonZeros() == 1, "unchanged non zeros"
    assert unchangedMatrix[1, 0] == 5.0, "unchanged value"

    # the column pointer count must match the column dimension
    wrongColumnCountContent = re.sub(
        r'(<object class="SparseMatrix"[^>]*nbColumns_=")\d+(")',
        r"\g<1>1\g<2>",
        savedContent,
    )
    assertRejectedStudy(wrongColumnCountContent, otexp.SparseMatrix())

    # the column pointer must start at zero, increase, and end on the number of
    # stored coefficients
    assertRejectedStudy(
        replaceIndices(savedContent, columnPointerId, [1, 2, 3]), otexp.SparseMatrix()
    )
    assertRejectedStudy(
        replaceIndices(savedContent, columnPointerId, [0, 3, 2]), otexp.SparseMatrix()
    )
    assertRejectedStudy(
        replaceIndices(savedContent, columnPointerId, [0, 1, 2]), otexp.SparseMatrix()
    )

    # the values, the non-zero count and the row indices must be consistent
    assertRejectedStudy(
        replaceValues(savedContent, valuesId, [4.0, 4.0]), otexp.SparseMatrix()
    )
    wrongSizeContent = re.sub(
        r'(<object class="SparseMatrix"[^>]*size_=")\d+(")', r"\g<1>2\g<2>", savedContent
    )
    assertRejectedStudy(wrongSizeContent, otexp.SparseMatrix())
    assertRejectedStudy(
        replaceIndices(savedContent, rowIndexId, [0, 1, 99]), otexp.SparseMatrix()
    )
    # the canonical layout needs strictly increasing row indices in each column:
    # unsorted rows or duplicate coordinates would break the mutable accessor
    # and the compressed model handed over to HiGHS
    assertRejectedStudy(
        replaceIndices(savedContent, rowIndexId, [1, 0, 2]), otexp.SparseMatrix()
    )
    assertRejectedStudy(
        replaceIndices(savedContent, rowIndexId, [0, 0, 2]), otexp.SparseMatrix()
    )
    os.remove(fileName)

print("OK")
