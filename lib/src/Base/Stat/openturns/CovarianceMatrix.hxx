//                                               -*- C++ -*-
/**
 *  @brief The class CovarianceMatrix implements covariance matrices
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
  CLASSNAME

public:
  typedef Collection<Scalar>       ScalarCollection;


  /** Default constructor */
  CovarianceMatrix();

  /** Constructor with implementation */
  CovarianceMatrix(const Implementation & i);

  /** Constructor with implementation */
  CovarianceMatrix(const MatrixImplementation & i);

  /** Constructor from SymmetricMatrix */
  explicit CovarianceMatrix(const SymmetricMatrix & sym);

  /** Constructor with size */
  explicit CovarianceMatrix(const UnsignedInteger dim);

  /** Constructor from external collection */
  CovarianceMatrix(const UnsignedInteger dim,
                   const ScalarCollection & elementsValues);

  /** String converter */
  String __repr__() const override;

  /** CovarianceMatrix transpose */
  CovarianceMatrix transpose () const;

  /** CovarianceMatrix addition (must have the same dimensions) */
  using SymmetricMatrix::operator +;
  CovarianceMatrix operator + (const CovarianceMatrix & m) const;

  /** CovarianceMatrix multiplication (must have consistent dimensions) */
  using SymmetricMatrix::operator *;
  CovarianceMatrix operator * (const IdentityMatrix & m) const;

  /** Check if the matrix is SPD */
  virtual Bool isPositiveDefinite() const;

  /** Build the Cholesky factorization of the matrix */
  virtual TriangularMatrix computeCholesky(const Bool keepIntact = true);

  /** Build the regularize Cholesky factorization of the matrix */
  virtual TriangularMatrix computeRegularizedCholesky() const;

  /** Resolution of a linear system */
  Point solveLinearSystemInPlace(const Point & b);
  Point solveLinearSystem(const Point & b) const;
  Matrix solveLinearSystemInPlace(const Matrix & b);
  Matrix solveLinearSystem(const Matrix & b) const;

protected:

private:

}; /* class CovarianceMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMATRIX_HXX */
