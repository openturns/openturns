//                                               -*- C++ -*-
/**
 * @brief HSICVStat implements the computation of a biased HSIC index.
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
#ifndef OPENTURNS_HSICVSTAT_HXX
#define OPENTURNS_HSICVSTAT_HXX

#include "openturns/HSICStatImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HSICVStat
 *
 */
class OT_API HSICVStat: public HSICStatImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  HSICVStat();

  /** Virtual constructor */
  HSICVStat* clone() const override;

  /** Compute the HSIC index for one marginal*/
  Scalar computeHSICIndex(const CovarianceMatrix &CovMat1,
                          const CovarianceMatrix &CovMat2,
                          const Point &weights) const override;

  /** Compute the asymptotic p-value */
  Scalar computePValue(const Gamma & distribution,
                       const UnsignedInteger n,
                       const Scalar HSICObs,
                       const Scalar mHSIC) const override;

  /** Is compatible with a Conditional HSIC Estimator ? */
  Bool isCompatibleWithConditionalAnalysis() const override;
};

END_NAMESPACE_OPENTURNS
#endif /*OPENTURNS_HSICVSTAT_HXX*/
