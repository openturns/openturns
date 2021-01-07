//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized beta function and related functions
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
#ifndef OPENTURNS_BETAFUNCTIONS_HXX
#define OPENTURNS_BETAFUNCTIONS_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace BetaFunctions
{
Scalar IncompleteBeta(const Scalar a,
                      const Scalar b,
                      const Scalar x,
                      const Bool tail = false);

Scalar IncompleteBetaInverse(const Scalar a,
                             const Scalar b,
                             const Scalar x,
                             const Bool tail = false);

Scalar IncompleteBetaP(const Scalar a,
                       const Scalar b,
                       const Scalar x);

Scalar IncompleteBetaQ(const Scalar a,
                       const Scalar b,
                       const Scalar x);

Scalar RegularizedIncompleteBeta(const Scalar a,
                                 const Scalar b,
                                 const Scalar x,
                                 const Bool tail = false);

Scalar RegularizedIncompleteBetaSlatec(const Scalar a,
                                       const Scalar b,
                                       const Scalar x,
                                       const Bool tail);

Scalar RegularizedIncompleteBetaInverse(const Scalar a,
                                        const Scalar b,
                                        const Scalar x,
                                        const Bool tail = false);

Scalar RegularizedIncompleteBetaP(const Scalar a,
                                  const Scalar b,
                                  const Scalar x);

Scalar RegularizedIncompleteBetaQ(const Scalar a,
                                  const Scalar b,
                                  const Scalar x);

Scalar RegularizedIncompleteBetaContinuedFraction(const Scalar a,
    const Scalar b,
    const Scalar x);

Scalar BRATIO(const Scalar a,
              const Scalar b,
              const Scalar x,
              const Bool tail);

Scalar BPSER(const Scalar a,
             const Scalar b,
             const Scalar x);

Scalar BUP(const Scalar a,
           const Scalar b,
           const Scalar x,
           const Scalar y,
           const UnsignedInteger n);

Scalar ALGDIV(const Scalar a,
              const Scalar b);

Scalar BGRAT(const Scalar a,
             const Scalar b,
             const Scalar x,
             const Scalar y,
             const Scalar w);

} // BetaFunctions

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BETAFUNCTIONS_HXX */
