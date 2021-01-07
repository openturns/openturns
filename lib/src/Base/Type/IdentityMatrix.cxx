//                                               -*- C++ -*-
/**
 *  @brief The class IdentityMatrix implements identity matrices
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
#include "openturns/IdentityMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(IdentityMatrix)

/* Default constructor */
IdentityMatrix::IdentityMatrix()
  : CorrelationMatrix(0)
{
  // Nothing to do
}

/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
IdentityMatrix::IdentityMatrix(const UnsignedInteger dim)
  : CorrelationMatrix(dim)
{
  // Nothing to do
}

/* Constructor with implementation */
IdentityMatrix::IdentityMatrix(const Implementation & i)
  : CorrelationMatrix(i)
{
  // Nothing to do
}

/* String converter */
String IdentityMatrix::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

/* Operator () : should do nothing, in order to prevent any modification on the matrix */
Scalar & IdentityMatrix::operator() (const UnsignedInteger i,
                                     const UnsignedInteger j)
{
  copyOnWrite();
  return (*getImplementation())(i, j);
}

/* Operator () gives access to the elements of the matrix (read only) */
/* The element of the matrix is designated by its row number i and its column number j */
const Scalar & IdentityMatrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  return (*getImplementation())(i, j);
}

/* IdentityMatrix transpose */
IdentityMatrix IdentityMatrix::transpose () const
{
  return *this;
}

/* Multiplications */
Matrix IdentityMatrix::operator * (const Matrix & m) const
{
  return m;
}

SquareMatrix IdentityMatrix::operator * (const SquareMatrix & m) const
{
  return m;
}

SymmetricMatrix IdentityMatrix::operator * (const SymmetricMatrix & m) const
{
  return m;
}

CovarianceMatrix IdentityMatrix::operator * (const CovarianceMatrix & m) const
{
  return m;
}

CorrelationMatrix IdentityMatrix::operator * (const CorrelationMatrix & m) const
{
  return m;
}

/* Resolution of a linear system */
Point IdentityMatrix::solveLinearSystem(const Point & b,
                                        const Bool )
{
  return b;
}

Matrix IdentityMatrix::solveLinearSystem(const Matrix & b,
    const Bool )
{
  return b;
}

/* Compute determinant */
Scalar IdentityMatrix::computeLogAbsoluteDeterminant(Scalar & sign,
    const Bool )
{
  sign = 1.0;
  return 0.0;
}

Scalar IdentityMatrix::computeDeterminant(const Bool )
{
  return 1.0;
}

/* Compute eigenvalues */
Point IdentityMatrix::computeEigenValues(const Bool )
{
  return Point(getNbRows(), 1.0);
}

Point IdentityMatrix::computeEV(SquareMatrix & v,
                                const Bool )
{
  v = *this;
  return Point(getNbRows(), 1.0);
}

/* Compute singular values */
Point IdentityMatrix::computeSingularValues(const Bool )
{
  return Point(getNbRows(), 1.0);
}

/* Compute singular values */
Point IdentityMatrix::computeSVD(Matrix & u,
                                 Matrix & vT,
                                 const Bool,
                                 const Bool )
{
  u = *this;
  vT = *this;
  return Point(getNbRows(), 1.0);
}

/* Check if the matrix is SPD */
Bool IdentityMatrix::isPositiveDefinite() const
{
  return true;
}

/* Build the Cholesky factorization of the matrix */
TriangularMatrix IdentityMatrix::computeCholesky(const Bool )
{
  return getImplementation();
}

END_NAMESPACE_OPENTURNS
