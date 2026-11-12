//                                               -*- C++ -*-
/**
 *  @brief The test file of class SparseMatrix for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;

  /* Default constructor */
  {
    SparseMatrix matrix;
    assert_equal(matrix.getNbRows(), static_cast<UnsignedInteger>(0), "default rows");
    assert_equal(matrix.getNbColumns(), static_cast<UnsignedInteger>(0), "default columns");
    assert_equal(matrix.getNbNonZeros(), static_cast<UnsignedInteger>(0), "default non zeros");
    assert_equal(Indices({0}), matrix.getColumnPointer(), "default column pointer");
  }

  /* Constructor with size, filling operator and accessors */
  {
    SparseMatrix matrix(4, 5);
    matrix(0, 2) = 5.0;
    matrix(1, 0) = 8.0;
    matrix(2, 4) = 2.0;
    matrix(3, 2) = 9.0;
    assert_equal(matrix.getNbRows(), static_cast<UnsignedInteger>(4), "rows");
    assert_equal(matrix.getNbColumns(), static_cast<UnsignedInteger>(5), "columns");
    assert_equal(matrix.getNbNonZeros(), static_cast<UnsignedInteger>(4), "non zeros");
    assert_equal(Indices({0, 1, 1, 3, 3, 4}), matrix.getColumnPointer(), "column pointer");
    assert_equal(Indices({1, 0, 3, 2}), matrix.getRowIndices(), "row indices");
    assert_almost_equal(Point({8.0, 5.0, 9.0, 2.0}), matrix.getValues(), 1.0e-5, 1.0e-8, "values");
    assert_almost_equal(matrix(1, 0), 8.0, 1.0e-5, 1.0e-8, "coefficient");
    // read through a const reference: the mutable accessor inserts the entry
    const SparseMatrix & constMatrix(matrix);
    assert_almost_equal(constMatrix(1, 1), 0.0, 1.0e-5, 1.0e-8, "implicit zero");
    Matrix dense(4, 5);
    dense(0, 2) = 5.0;
    dense(1, 0) = 8.0;
    dense(2, 4) = 2.0;
    dense(3, 2) = 9.0;
    assert_almost_equal(dense, matrix.asDenseMatrix(), 1.0e-5, 1.0e-8, "dense conversion");
    // overwriting an existing coefficient does not add an entry
    matrix(1, 0) = 3.0;
    assert_equal(matrix.getNbNonZeros(), static_cast<UnsignedInteger>(4), "non zeros after overwrite");
    assert_almost_equal(matrix(1, 0), 3.0, 1.0e-5, 1.0e-8, "overwritten coefficient");
  }

  /* Canonical triplet constructor: rows are sorted and duplicates summed */
  {
    SparseMatrix matrix(3, 2,
                        Indices({2, 0, 0, 1}),
                        Indices({1, 0, 0, 0}),
                        Point({2.0, 3.0, 1.0, 4.0}));
    assert_equal(matrix.getNbNonZeros(), static_cast<UnsignedInteger>(3), "non zeros");
    assert_equal(Indices({0, 2, 3}), matrix.getColumnPointer(), "column pointer");
    assert_equal(Indices({0, 1, 2}), matrix.getRowIndices(), "row indices");
    assert_almost_equal(Point({4.0, 4.0, 2.0}), matrix.getValues(), 1.0e-5, 1.0e-8, "values");
    Matrix dense(3, 2);
    dense(0, 0) = 4.0;
    dense(1, 0) = 4.0;
    dense(2, 1) = 2.0;
    assert_almost_equal(dense, matrix.asDenseMatrix(), 1.0e-5, 1.0e-8, "summed duplicates");

    // copy constructor and clone
    SparseMatrix copy(matrix);
    assert_equal(copy.getColumnPointer(), matrix.getColumnPointer(), "copy");
    SparseMatrix * p_clone = matrix.clone();
    assert_equal(p_clone->getRowIndices(), matrix.getRowIndices(), "clone");
    assert_almost_equal(p_clone->getValues(), matrix.getValues(), 1.0e-5, 1.0e-8, "clone values");
    delete p_clone;
  }

  /* Invalid triplets are rejected */
  {
    Bool raised = false;
    try
    {
      const SparseMatrix matrix(2, 2, Indices({2}), Indices({0}), Point({1.0}));
      (void) matrix;
    }
    catch (const OutOfBoundException &)
    {
      raised = true;
    }
    assert_equal(raised, true, "out of bound row index");

    raised = false;
    try
    {
      const SparseMatrix matrix(2, 2, Indices({0}), Indices({2}), Point({1.0}));
      (void) matrix;
    }
    catch (const OutOfBoundException &)
    {
      raised = true;
    }
    assert_equal(raised, true, "out of bound column index");

    raised = false;
    try
    {
      const SparseMatrix matrix(2, 2, Indices({0}), Indices({0, 1}), Point({1.0}));
      (void) matrix;
    }
    catch (const InvalidArgumentException &)
    {
      raised = true;
    }
    assert_equal(raised, true, "mismatched triplets size");
  }

  /* Dense to sparse conversion skips the zero coefficients */
  {
    Matrix dense(4, 5);
    dense(0, 2) = 5.0;
    dense(1, 0) = 8.0;
    dense(2, 4) = 2.0;
    dense(3, 2) = 9.0;
    const SparseMatrix matrix(SparseMatrix::FromDenseMatrix(dense));
    assert_equal(matrix.getNbNonZeros(), static_cast<UnsignedInteger>(4), "non zeros");
    assert_equal(Indices({0, 1, 1, 3, 3, 4}), matrix.getColumnPointer(), "column pointer");
    assert_equal(Indices({1, 0, 3, 2}), matrix.getRowIndices(), "row indices");
    assert_almost_equal(dense, matrix.asDenseMatrix(), 1.0e-5, 1.0e-8, "dense round trip");
    assert_equal(SparseMatrix::FromDenseMatrix(Matrix(2, 2)).getNbNonZeros(), static_cast<UnsignedInteger>(0), "empty dense matrix");
  }

  /* Transposition */
  {
    Matrix dense(3, 2);
    dense(0, 0) = 4.0;
    dense(1, 0) = 4.0;
    dense(2, 1) = 2.0;
    const SparseMatrix matrix(SparseMatrix::FromDenseMatrix(dense));
    const SparseMatrix transposed(matrix.transpose());
    assert_equal(transposed.getNbRows(), static_cast<UnsignedInteger>(2), "transposed rows");
    assert_equal(transposed.getNbColumns(), static_cast<UnsignedInteger>(3), "transposed columns");
    assert_equal(Indices({0, 1, 2, 3}), transposed.getColumnPointer(), "transposed column pointer");
    assert_equal(Indices({0, 0, 1}), transposed.getRowIndices(), "transposed row indices");
    assert_almost_equal(Point({4.0, 4.0, 2.0}), transposed.getValues(), 1.0e-5, 1.0e-8, "transposed values");
    assert_almost_equal(dense.transpose(), transposed.asDenseMatrix(), 1.0e-5, 1.0e-8, "transposed dense");
  }

  /* Products */
  {
    const SparseMatrix matrix(3, 2,
                              Indices({0, 1, 2}),
                              Indices({0, 0, 1}),
                              Point({1.0, 2.0, 3.0}));
    assert_almost_equal(Point({4.0, 8.0, 9.0}), matrix * Point({4.0, 3.0}), 1.0e-5, 1.0e-8, "sparse * point");

    Matrix dense(2, 2);
    dense(0, 0) = 1.0;
    dense(0, 1) = 2.0;
    dense(1, 0) = 3.0;
    dense(1, 1) = 4.0;
    assert_almost_equal(matrix.asDenseMatrix() * dense, matrix * dense, 1.0e-5, 1.0e-8, "sparse * dense");

    Matrix left(2, 3);
    left(0, 0) = 1.0;
    left(0, 1) = 2.0;
    left(0, 2) = 3.0;
    left(1, 0) = 4.0;
    left(1, 1) = 5.0;
    left(1, 2) = 6.0;
    assert_almost_equal(left * matrix.asDenseMatrix(), matrix.multiplyByDenseOnLeft(left), 1.0e-5, 1.0e-8, "dense * sparse");

    SymmetricMatrix sym(3);
    sym(0, 0) = 2.0;
    sym(1, 1) = 3.0;
    sym(2, 2) = 4.0;
    sym(1, 0) = 1.0;
    sym(2, 0) = 2.0;
    sym(2, 1) = 3.0;
    const Matrix denseSym(sym);
    assert_almost_equal(denseSym * matrix.asDenseMatrix(), matrix.multiplyBySymmetricOnLeft(sym), 1.0e-5, 1.0e-8, "symmetric * sparse");
  }

  /* String converter */
  {
    const SparseMatrix matrix(3, 2,
                              Indices({2, 0, 0, 1}),
                              Indices({1, 0, 0, 0}),
                              Point({2.0, 3.0, 1.0, 4.0}));
    assert_equal(matrix.__repr__(),
                 String("class=SparseMatrix rows=3 columns=2 triplets=[[0,0,4],[1,0,4],[2,1,2]]"),
                 "string converter");
  }

  return ExitCode::Success;
}
