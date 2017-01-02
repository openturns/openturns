//                                               -*- C++ -*-
/**
 *  @brief The class CovarianceMatrix implements covariance matrices
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COVARIANCEMATRIX_HXX
#define OPENTURNS_COVARIANCEMATRIX_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceMatrix
 */

class OT_API CovarianceMatrix
  : public SymmetricMatrix
{
  CLASSNAME;

public:
  typedef Collection<NumericalScalar>       NumericalScalarCollection;


  /** Default constructor */
  CovarianceMatrix();

  /** Constructor with implementation */
  CovarianceMatrix(const Implementation & i);

  /** Constructor with implementation */
  CovarianceMatrix(const MatrixImplementation & i);

  /** Constructor with size */
  explicit CovarianceMatrix(const UnsignedInteger dim);

  /** Constructor from external collection */
  CovarianceMatrix(const UnsignedInteger dim,
                   const NumericalScalarCollection & elementsValues);

  /** String converter */
  virtual String __repr__() const;

  /** CovarianceMatrix transpose */
  CovarianceMatrix transpose () const;

  /** CovarianceMatrix addition (must have the same dimensions) */
  CovarianceMatrix operator + (const CovarianceMatrix & m) const;
  using SymmetricMatrix::operator +;

  /** CovarianceMatrix multiplication (must have consistent dimensions) */
  CovarianceMatrix operator * (const IdentityMatrix & m) const;
  using SymmetricMatrix::operator *;

  /** Check if the matrix is SPD */
  virtual Bool isPositiveDefinite(const Bool keepIntact = true);

  /** Build the Cholesky factorization of the matrix */
  virtual TriangularMatrix computeCholesky(const Bool keepIntact = true);

  /** Resolution of a linear system */
  NumericalPoint solveLinearSystem(const NumericalPoint & b,
                                   const Bool keepIntact = true);

  Matrix solveLinearSystem(const Matrix & b,
                           const Bool keepIntact = true);

protected:

private:

}; /* class CovarianceMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMATRIX_HXX */
