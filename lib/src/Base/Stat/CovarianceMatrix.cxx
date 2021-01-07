//                                               -*- C++ -*-
/**
 *  @brief The class CovarianceMatrix implements blank free samples
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
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CovarianceMatrix)

/* Default constructor */
CovarianceMatrix::CovarianceMatrix()
  : SymmetricMatrix(0)
{
  // Nothing to do
}

/* Constructor with implementation */
CovarianceMatrix::CovarianceMatrix(const Implementation & i)
  : SymmetricMatrix(i)
{
  // Nothing to do
}

/* Constructor with implementation */
CovarianceMatrix::CovarianceMatrix(const MatrixImplementation & i)
  : SymmetricMatrix(i)
{
  // Nothing to do
}

/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
CovarianceMatrix::CovarianceMatrix(const UnsignedInteger dim)
  : SymmetricMatrix(dim)
{
  // Initialize the correlation matrix to the identity matrix
  for(UnsignedInteger i = 0; i < dim; ++i) operator()(i, i) = 1.0;
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
CovarianceMatrix::CovarianceMatrix(const UnsignedInteger dim,
                                   const Collection<Scalar> &elementsValues)
  : SymmetricMatrix(dim, elementsValues)
{
  // Nothing to do
}

/* String converter */
String CovarianceMatrix::__repr__() const
{
  checkSymmetry();
  return OSS() << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

/* CovarianceMatrix transpose */
CovarianceMatrix CovarianceMatrix::transpose () const
{
  return *this;
}

/* CovarianceMatrix addition (must have the same dimensions) */
CovarianceMatrix CovarianceMatrix::operator + (const CovarianceMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* CovarianceMatrix multiplication (must have consistent dimensions) */
CovarianceMatrix CovarianceMatrix::operator * (const IdentityMatrix & ) const
{
  return *this;
}

/* Check if the matrix is SPD */
Bool CovarianceMatrix::isPositiveDefinite() const
{
  return getImplementation()->isPositiveDefinite();
}

/* Build the Cholesky factorization of the matrix */
TriangularMatrix CovarianceMatrix::computeCholesky(const Bool keepIntact)
{
  return Implementation(getImplementation()->computeCholesky(keepIntact).clone());
}


/* Resolution of a linear system */
Point CovarianceMatrix::solveLinearSystem(const Point & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemCov(b, keepIntact);
}

Matrix CovarianceMatrix::solveLinearSystem(const Matrix & b,
    const Bool keepIntact)
{
  return Implementation(getImplementation()->solveLinearSystemCov(*b.getImplementation(), keepIntact).clone());
}

END_NAMESPACE_OPENTURNS
