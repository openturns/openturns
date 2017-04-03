//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the exponential integral function
 *         for complex arguments
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
#ifndef OPENTURNS_EXPONENTIALINTEGRALFUNCTIONS_HXX
#define OPENTURNS_EXPONENTIALINTEGRALFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace ExponentialIntegralFunctions
{

NumericalComplex Ei(const NumericalComplex & z);
Scalar Ei(const Scalar z);

NumericalComplex EiPowerSeries(const NumericalComplex & z);
Scalar EiPowerSeries(const Scalar z);

NumericalComplex EiAsymptoticSeries(const NumericalComplex & z);
Scalar EiAsymptoticSeries(const Scalar z);

NumericalComplex EiContinuedFractionBackward(const NumericalComplex & z);
Scalar EiContinuedFractionBackward(const Scalar z);

NumericalComplex EiContinuedFractionForward(const NumericalComplex & z);
Scalar EiContinuedFractionForward(const Scalar z);

Bool hasConverged(const NumericalComplex & current,
                  const NumericalComplex & previous);

} // ExponentialIntegralFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPONENTIALINTEGRALFUNCTIONS_HXX */
