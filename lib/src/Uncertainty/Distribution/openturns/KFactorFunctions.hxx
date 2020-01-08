//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the k factor
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
#ifndef OPENTURNS_KFACTORFUNCTIONS_HXX
#define OPENTURNS_KFACTORFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace KFactorFunctions
{
Scalar KFactor(const Scalar n,
               const Scalar nu,
               const Scalar p,
               const Scalar alpha);
} // KFactorFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KFACTORFUNCTIONS_HXX */
