//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for HMat
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrix)

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

/** Copy matrix */
HMatrix HMatrix::copy() const
{
  return HMatrix(new HMatrixImplementation(*getImplementation()));
}

void HMatrix::assemble(const HMatrixRealAssemblyFunction& f, char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, symmetry);
}

void HMatrix::assemble(const HMatrixRealAssemblyFunction &f,
                       const HMatrixParameters & parameters,
                      char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, parameters, symmetry);
}

void HMatrix::assemble(const HMatrixTensorRealAssemblyFunction& f, char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, symmetry);
}

void HMatrix::assemble(const HMatrixTensorRealAssemblyFunction &f,
                       const HMatrixParameters &parameters,
                       char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, parameters, symmetry);
}

void HMatrix::factorize(const String& method)
{
  copyOnWrite();
  getImplementation()->factorize(method);
}

/** Compute x <- alpha * this */
void HMatrix::scale(Scalar alpha)
{
  copyOnWrite();
  getImplementation()->scale(alpha);
}

/** Compute y <- alpha op(this) * x + beta * y */
void HMatrix::gemv(char trans, Scalar alpha, const Point& x, Scalar beta, Point& y) const
{
  getImplementation()->gemv(trans, alpha, x, beta, y);
}

/** Compute this <- alpha op(A) * p(B) + beta * this */
void HMatrix::gemm(char transA, char transB, Scalar alpha, const HMatrix& a, const HMatrix& b, Scalar beta)
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
Scalar HMatrix::norm() const
{
  return getImplementation()->norm();
}

/** Get the diagonal */
Point HMatrix::getDiagonal() const
{
  return getImplementation()->getDiagonal();
}

void HMatrix::addIdentity(OT::Scalar alpha)
{
  copyOnWrite();
  getImplementation()->addIdentity(alpha);
}

/** Solve system op(A)*X = b */
Point HMatrix::solve(const Point& b, Bool trans) const
{
  return getImplementation()->solve(b, trans);
}

/** Solve system op(A)*X = m */
Matrix HMatrix::solve(const Matrix& m, Bool trans) const
{
  return getImplementation()->solve(m, trans);
}

/** Solve system op(L)*X = b */
Point HMatrix::solveLower(const Point& b, Bool trans) const
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

/* Dump HMatrix onto file */
void HMatrix::dump(const String & name) const
{
  return getImplementation()->dump(name);
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
