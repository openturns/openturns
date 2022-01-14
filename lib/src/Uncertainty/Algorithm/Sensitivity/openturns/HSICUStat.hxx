//                                               -*- C++ -*-
/**
 * @brief HSICUStat implements the computation of an unbiased HSIC index.
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
#ifndef OPENTURNS_HSICUSTAT_HXX
#define OPENTURNS_HSICUSTAT_HXX

#include "openturns/HSICStatImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HSICUStat
 *
 */
class OT_API HSICUStat: public HSICStatImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  HSICUStat();

  /** Virtual constructor */
  HSICUStat* clone() const override;

  /** Compute the HSIC index for one marginal*/
  virtual Scalar computeHSICIndex(const Sample & inSample, const Sample & outSample, const CovarianceModel & inCovariance, const CovarianceModel & outCovariance, const SquareMatrix & weightMatrix) const override;

  /** Compute the asymptotic p-value */
  virtual Scalar computePValue(const Gamma &dist, const UnsignedInteger n, const Scalar HSIC_obs, const Scalar mHSIC) const override;

  /** Is compatible with a Conditional HSIC Estimator ? */
  virtual Bool isCompatibleWithConditionalAnalysis() const override;
};

END_NAMESPACE_OPENTURNS
#endif /*OPENTURNS_HSICUSTAT_HXX*/
