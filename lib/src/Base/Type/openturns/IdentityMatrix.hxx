//                                               -*- C++ -*-
/**
 *  @brief The class IdentityMatrix implements identity matrices
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_IDENTITYMATRIX_HXX
#define OPENTURNS_IDENTITYMATRIX_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CorrelationMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class IdentityMatrix
 */

class OT_API IdentityMatrix
  : public CorrelationMatrix
{
  CLASSNAME

public:


  /** Default constructor */
  IdentityMatrix();

  /** Constructor with size */
  explicit IdentityMatrix(const UnsignedInteger dim);


  /** String converter */
  String __repr__() const override;

  /** IdentityMatrix transpose */
  IdentityMatrix transpose () const;

#ifndef SWIG
  /** Operator () gives access to the elements of the matrix (read only) */
  /** The element of the matrix is designated by its row number i and its column number j */
  const Scalar & operator () (const UnsignedInteger i,
                              const UnsignedInteger j) const;
#endif

  /** Multiplications */
#ifdef _MSC_VER   // VS2010 does not like 'using' being called after overloads
  using CorrelationMatrix::operator *;
#endif

  Matrix operator * (const Matrix & m) const;
  SquareMatrix operator * (const SquareMatrix & m) const;
  SymmetricMatrix operator * (const SymmetricMatrix & m) const;
  CovarianceMatrix operator * (const CovarianceMatrix & m) const;
  CorrelationMatrix operator * (const CorrelationMatrix & m) const;
#ifndef _MSC_VER
  using CorrelationMatrix::operator *;
#endif

  /** Resolution of a linear system */
  Point solveLinearSystem(const Point & b,
                          const Bool keepIntact = true);

  Matrix solveLinearSystem(const Matrix & b,
                           const Bool keepIntact = true);

  /** Compute determinant */
  Scalar computeLogAbsoluteDeterminant(Scalar & signOut,
                                       const Bool keepIntact = true);
  Scalar computeDeterminant(const Bool keepIntact = true);

  /** Compute eigenvalues */
  Point computeEigenValues(const Bool keepIntact = true);
  Point computeEV(SquareMatrix & vOut,
                  const Bool keepIntact = true);

  /** Check if the matrix is SPD */
  Bool isPositiveDefinite() const override;

  /** Build the Cholesky factorization of the matrix */
  TriangularMatrix computeCholesky(const Bool keepIntact = true) override;

  /** Compute singular values */
  Point computeSingularValues(const Bool keepIntact = true);

  Point computeSVD(Matrix & uOut,
                   Matrix & vTOut,
                   const Bool fullSVD = false,
                   const Bool keepIntact = true);

protected:


  /** Constructor with implementation */
  IdentityMatrix(const Implementation & i);

private:

  /** Operator () gives access to the elements of the matrix (to modify these elements) */
  /** The element of the matrix is designated by its row number i and its column number j */
  Scalar & operator () (const UnsignedInteger i,
                        const UnsignedInteger j);
}; /* class IdentityMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMATRIX_HXX */
