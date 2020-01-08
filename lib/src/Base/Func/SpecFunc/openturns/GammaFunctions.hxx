//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized gamma function and related functions
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
#ifndef OPENTURNS_GAMMAFUNCTIONS_HXX
#define OPENTURNS_GAMMAFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace GammaFunctions
{
Scalar IncompleteGamma(const Scalar a,
                       const Scalar x,
                       const Bool tail = false);

Scalar IncompleteGammaInverse(const Scalar a,
                              const Scalar x,
                              const Bool tail = false);

Scalar RegularizedIncompleteGamma(const Scalar a,
                                  const Scalar x,
                                  const Bool tail = false);

Scalar RegularizedIncompleteGammaInverse(const Scalar a,
    const Scalar x,
    const Bool tail = false);

} // GammaFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAMMAFUNCTIONS_HXX */
