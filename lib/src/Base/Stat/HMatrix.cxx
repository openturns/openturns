//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for HMat
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "HMatrix.hxx"
#include "HMatrixImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrix);

/* Default constructor */
HMatrix::HMatrix()
  : TypedInterfaceObject<HMatrixImplementation>(new HMatrixImplementation())
{
  // Nothing to do
}

/* Constructor with implementation */
HMatrix::HMatrix(const Implementation & i)
  : TypedInterfaceObject<HMatrixImplementation>(i)
{
  // Nothing to do
}

/* Constructor with implementation */
HMatrix::HMatrix(const HMatrixImplementation & i)
  : TypedInterfaceObject<HMatrixImplementation>(i.clone())
{
  // Nothing to do
}

void HMatrix::assemble(const HMatrixRealAssemblyFunction& f, char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, symmetry);
}

void HMatrix::assemble(const HMatrixTensorRealAssemblyFunction& f, char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, symmetry);
}

void HMatrix::factorize(const String& method)
{
  copyOnWrite();
  getImplementation()->factorize(method);
}

/** Compute y <- alpha op(this) * x + beta * y */
void HMatrix::gemv(char trans, NumericalScalar alpha, const NumericalPoint& x, NumericalScalar beta, NumericalPoint& y) const
{
  getImplementation()->gemv(trans, alpha, x, beta, y);
}

/** Compute this <- alpha op(A) * p(B) + beta * this */
void HMatrix::gemm(char transA, char transB, NumericalScalar alpha, const HMatrix& a, const HMatrix& b, NumericalScalar beta)
{
  copyOnWrite();
  getImplementation()->gemm(transA, transB, alpha, *a.getImplementation(), *b.getImplementation(), beta);
}

/** Transpose matrix */
void HMatrix::transpose()
{
  copyOnWrite();
  getImplementation()->transpose();
}

/** Get the Frobenius norm */
NumericalScalar HMatrix::norm() const
{
  return getImplementation()->norm();
}

/** Get the diagonal */
NumericalPoint HMatrix::getDiagonal() const
{
  return getImplementation()->getDiagonal();
}

/** Solve system op(A)*X = b */
NumericalPoint HMatrix::solve(const NumericalPoint& b, Bool trans) const
{
  return getImplementation()->solve(b, trans);
}

/** Solve system op(A)*X = m */
Matrix HMatrix::solve(const Matrix& m, Bool trans) const
{
  return getImplementation()->solve(m, trans);
}

/** Solve system op(L)*X = b */
NumericalPoint HMatrix::solveLower(const NumericalPoint& b, Bool trans) const
{
  return getImplementation()->solveLower(b, trans);
}


/** Solve system op(L)*X = m */
Matrix HMatrix::solveLower(const Matrix& m, Bool trans) const
{
  return getImplementation()->solveLower(m, trans);
}

/** Get number of HMatrix elements in compressed and uncompressed forms */
std::pair<size_t, size_t> HMatrix::compressionRatio() const
{
  return getImplementation()->compressionRatio();
}

/** Get number of HMatrix elements in full blocks and low rank blocks */
std::pair<size_t, size_t> HMatrix::fullrkRatio() const
{
  return getImplementation()->fullrkRatio();
}

/* String converter */
String HMatrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}


String HMatrix::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Get the dimensions of the matrix : number of rows */
UnsignedInteger HMatrix::getNbRows() const
{
  return getImplementation()->getNbRows();
}

/* Get the dimensions of the matrix : number of columns */
UnsignedInteger HMatrix::getNbColumns() const
{
  return getImplementation()->getNbColumns();
}

/* Comparison operator */
Bool HMatrix::operator == (const HMatrix & rhs) const
{
  const HMatrix &lhs(*this);
  return (*(lhs.getImplementation()) == *(rhs.getImplementation()) );
}


END_NAMESPACE_OPENTURNS
