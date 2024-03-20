
//                                               -*- C++ -*-
/**
 * @brief HSICStat is the base class for atomic HSIC index.
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
#ifndef OPENTURNS_HSICSTAT_HXX
#define OPENTURNS_HSICSTAT_HXX

#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/HSICStatImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class HSICStat
 *
 */
class OT_API HSICStat: public TypedInterfaceObject<HSICStatImplementation>
{
  CLASSNAME
public:
  typedef Pointer<HSICStatImplementation> Implementation;

  /** Default constructor */
  HSICStat();

  /** Constructor from implementation */
  HSICStat(const HSICStatImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  HSICStat(HSICStatImplementation * p_implementation);
#endif

  /** Compute the HSIC index for one marginal*/
  virtual Scalar computeHSICIndex(const CovarianceMatrix & covarianceMatrix1,
                                  const CovarianceMatrix & covarianceMatrix2,
                                  const SquareMatrix & weightMatrix) const;

  /** Compute the HSIC index for one marginal*/
  virtual Scalar computeHSICIndex(const CovarianceMatrix &covarianceMatrix1,
                                  const CovarianceMatrix &covarianceMatrix2,
                                  const Point &weights) const;

  /** Compute the asymptotic p-value */
  virtual Scalar computePValue(const Gamma & distribution,
                               const UnsignedInteger n,
                               const Scalar HSICObs,
                               const Scalar mHSIC) const;

  /** Is compatible with a Conditional HSIC Estimator ? */
  virtual Bool isCompatibleWithConditionalAnalysis() const;

};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HSICSTAT_HXX */
