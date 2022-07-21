//                                               -*- C++ -*-
/**
 *  @brief The class CorrelationMatrix implements correlation matrices
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CORRELATIONMATRIX_HXX
#define OPENTURNS_CORRELATIONMATRIX_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class CorrelationMatrix
 */

class OT_API CorrelationMatrix
  : public CovarianceMatrix
{
  CLASSNAME

public:
  typedef Collection<Scalar>       ScalarCollection;


  /** Default constructor */
  CorrelationMatrix();

  /** Constructor with implementation */
  CorrelationMatrix(const Implementation & i);

  /** Constructor with implementation */
  CorrelationMatrix(const MatrixImplementation & i);

  /** Constructor from SymmetricMatrix */
  explicit CorrelationMatrix(const SymmetricMatrix & sym);

  /** Constructor with size */
  explicit CorrelationMatrix(const UnsignedInteger dim);

  /** Constructor from external collection */
  CorrelationMatrix(const UnsignedInteger dim,
                    const ScalarCollection & elementsValues);

  /** String converter */
  String __repr__() const override;

  /** CorrelationMatrix transpose */
  CorrelationMatrix transpose () const;

  /** CorrelationMatrix multiplication (must have consistent dimensions) */
  using CovarianceMatrix::operator *;
  CorrelationMatrix operator * (const IdentityMatrix & m) const;
protected:


private:

}; /* class CorrelationMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMATRIX_HXX */
