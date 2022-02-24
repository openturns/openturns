
//                                               -*- C++ -*-
/**
 * @brief HSICStat is the base class for atomic HSIC index.
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
#include "openturns/Pointer.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HSICStat.hxx"
BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HSICStat)
/* Default constructor */
HSICStat::HSICStat()
  : TypedInterfaceObject<HSICStatImplementation>(new HSICStatImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
HSICStat::HSICStat(const HSICStatImplementation & implementation)
  : TypedInterfaceObject<HSICStatImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Compute the HSIC index for one marginal*/
Scalar HSICStat::computeHSICIndex(const Sample & inSample, 
                                  const Sample & outSample, 
                                  const CovarianceModel & inCovariance, 
                                  const CovarianceModel & outCovariance, 
                                  const SquareMatrix & weightMatrix) const
{
  return getImplementation()->computeHSICIndex(inSample, outSample, inCovariance, outCovariance, weightMatrix);
}

/* Is compatible with a Conditional HSIC Estimator ? */
Bool HSICStat::isCompatibleWithConditionalAnalysis() const
{
  return getImplementation()->isCompatibleWithConditionalAnalysis();
}

/* Compute the asymptotic p-value */
Scalar HSICStat::computePValue(const Gamma &dist, 
                               const UnsignedInteger n, 
                               const Scalar HSIC_obs, 
                               const Scalar mHSIC) const
{
  return getImplementation()->computePValue(dist, n, HSIC_obs, mHSIC);
}

END_NAMESPACE_OPENTURNS
