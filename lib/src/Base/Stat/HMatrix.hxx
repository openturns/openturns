//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for HMat
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_HMATRIX_HXX
#define OPENTURNS_HMATRIX_HXX

#include "HMatrixImplementation.hxx"
#include "TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class HMatrix
 *
 * HMatrix implements hierarchical matrices
 */

class OT_API HMatrix :
  public TypedInterfaceObject<HMatrixImplementation>
{
  CLASSNAME;

  /** Default constructor */
  HMatrix();

  /** Constructor with implementation */
  HMatrix(const Implementation & i);

  /** Constructor with implementation */
  HMatrix(const HMatrixImplementation & i);

  /** Get the dimensions of the matrix */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;

  void assemble(const HMatrixRealAssemblyFunction& f, char symmetry);

  void assemble(const HMatrixTensorRealAssemblyFunction& f, char symmetry);

  void factorize(const String& method);

  /** Compute y <- alpha op(this) * x + beta * y */
  void gemv(char trans, NumericalScalar alpha, const NumericalPoint& x, NumericalScalar beta, NumericalPoint& y) const;

  /** Compute this <- alpha op(A) * p(B) + beta * this */
  void gemm(char transA, char transB, NumericalScalar alpha, const HMatrix& a, const HMatrix& b, NumericalScalar beta);

  /** Transpose matrix */
  void transpose();

  /** Get the Frobenius norm */
  NumericalScalar norm() const;

  /** Get the diagonal */
  NumericalPoint getDiagonal() const;

  /** Solve system op(A)*X = b */
  NumericalPoint solve(const NumericalPoint& b, Bool trans = false) const;

  /** Solve system op(A)*X = m */
  Matrix solve(const Matrix& m, Bool trans = false) const;

  /** Solve system op(L)*X = b */
  NumericalPoint solveLower(const NumericalPoint& b, Bool trans = false) const;

  /** Solve system op(L)*X = m */
  Matrix solveLower(const Matrix& m, Bool trans = false) const;

  /** Get number of HMatrix elements in compressed and uncompressed forms */
  std::pair<size_t, size_t> compressionRatio() const;

  /** Get number of HMatrix elements in full blocks and low rank blocks */
  std::pair<size_t, size_t> fullrkRatio() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Comparison operators */
  Bool operator == (const HMatrix & rhs) const;

}; /* class HMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HMATRIX_HXX */
