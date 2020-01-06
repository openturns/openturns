//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the CDF computation for a bi-dimensional
 *         Normal distribution
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
#include <cmath>

#include "openturns/Normal2DCDF.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
  Translated in C++ from Alan Genz's matlab routine bvnl.m. This routine has no
  explicit copyright, but is also part of the tvnl.m routine from the same author,
  with the following copyright:
  %   Copyright (C) 2011, Alan Genz,  All rights reserved.
  %
  %   Redistribution and use in source and binary forms, with or without
  %   modification, are permitted provided the following conditions are met:
  %     1. Redistributions of source code must retain the above copyright
  %        notice, this list of conditions and the following disclaimer.
  %     2. Redistributions in binary form must reproduce the above copyright
  %        notice, this list of conditions and the following disclaimer in the
  %        documentation and/or other materials provided with the distribution.
  %     3. The contributor name(s) may not be used to endorse or promote
  %        products derived from this software without specific prior written
  %        permission.
  %   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  %   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  %   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  %   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  %   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  %   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  %   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  %   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  %   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  %   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
  %   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  %
*/
#define NORMAL2DCDF_MINUS_INF -8.29236108
#define NORMAL2DCDF_PLUS_INF 37.5193794
#define NORMAL2DCDF_MIN_LOG -745.13321
#define NORMAL2DCDF_EPS 1.0e-15

Scalar Normal2DCDF(const Scalar x1,
                   const Scalar x2,
                   const Scalar rho,
                   const Bool tail)
{
  Scalar absRho = std::abs(rho);
  if (!(absRho <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient must be in [-1,1], here rho=" << rho;
  // Use the relation Normal2DCDF(x1, x2, false) = Normal2DCDF(-x1, -x2, true)
  if (!tail) return Normal2DCDF(-x1, -x2, rho, true);
  // The special cases: zero mass, unit mass, marginal mass
  if ((x1 >= NORMAL2DCDF_PLUS_INF) || (x2 >= NORMAL2DCDF_PLUS_INF)) return 0.0;
  if (x1 <= NORMAL2DCDF_MINUS_INF)
  {
    if (x2 <= NORMAL2DCDF_MINUS_INF) return 1.0;
    else return DistFunc::pNormal(-x2);
  }
  if (x2 <= NORMAL2DCDF_MINUS_INF) return DistFunc::pNormal(-x1);
  // The special case: no correlation
  if (absRho <= NORMAL2DCDF_EPS) return DistFunc::pNormal(-x1) * DistFunc::pNormal(-x2);
  // Nodes and weights for Legendre integration
  static Scalar nodes[19] = {0.9324695142031522, 0.6612093864662647, 0.2386191860831970, 0.9815606342467191, 0.9041172563704750, 0.7699026741943050, 0.5873179542866171, 0.3678314989981802, 0.1252334085114692, 0.9931285991850949, 0.9639719272779138, 0.9122344282513259, 0.8391169718222188, 0.7463319064601508, 0.6360536807265150, 0.5108670019508271, 0.3737060887154196, 0.2277858511416451, 0.07652652113349733};
  static Scalar weights[19] = {0.1713244923791705, 0.3607615730481384, 0.4679139345726904, 0.04717533638651177, 0.1069393259953183, 0.1600783285433464, 0.2031674267230659, 0.2334925365383547, 0.2491470458134029, 0.01761400713915212, 0.04060142980038694, 0.06267204833410906, 0.08327674157670475, 0.1019301198172404, 0.1181945319615184, 0.1316886384491766, 0.1420961093183821, 0.1491729864726037, 0.1527533871307259};
  UnsignedInteger shift = 0;
  UnsignedInteger size = 0;
  if (absRho <= 0.3)
  {
    shift = 0;
    size = 3;
  }
  else if (absRho <= 0.75)
  {
    shift = 3;
    size = 6;
  }
  else
  {
    shift = 9;
    size = 10;
  }
  Scalar cdf = 0.0;
  Scalar u1 = x1;
  Scalar u2 = x2;
  Scalar u1u2 = u1 * u2;
  // Not too much correlation
  if (absRho <= 0.925)
  {
    const Scalar halfSquare = 0.5 * (u1 * u1 + u2 * u2);
    const Scalar arcSinRho = std::asin(rho);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar sinValueMinus = std::sin(0.5 * arcSinRho * (1.0 - nodes[shift + i]));
      const Scalar sinValuePlus = std::sin(0.5 * arcSinRho * (1.0 + nodes[shift + i]));
      cdf += weights[shift + i] * (std::exp((sinValueMinus * u1u2 - halfSquare) / (1.0 - sinValueMinus * sinValueMinus)) + std::exp((sinValuePlus * u1u2 - halfSquare) / (1.0 - sinValuePlus * sinValuePlus)));
    }
    cdf *= arcSinRho / (4.0 * M_PI);
    cdf += DistFunc::pNormal(-u1) * DistFunc::pNormal(-u2);
    return std::max(0.0, std::min(1.0, cdf));
  } // absRho < 0.925
  if (rho < 0.0)
  {
    u2 = -u2;
    u1u2 = -u1u2;
  }
  if (absRho < 1.0)
  {
    const Scalar aSquare = (1.0 - rho) * (1.0 + rho);
    Scalar a = std::sqrt(aSquare);
    const Scalar b = std::abs(u1 - u2);
    const Scalar bSquare = b * b;
    const Scalar c = 0.5 - 0.125 * u1u2;
    const Scalar d = 0.75 - 0.06125 * u1u2;
    const Scalar aSquareReduced = -0.5 * (bSquare / aSquare + u1u2);
    const Scalar firstTerm = c * (1.0 - 0.2 * d * bSquare) / 3.0;
    if (aSquareReduced > NORMAL2DCDF_MIN_LOG) cdf = a * std::exp(aSquareReduced) * (1.0 - (bSquare - aSquare) * firstTerm + 0.2 * c * d * aSquare * aSquare);
    if (u1u2 < -2.0 * NORMAL2DCDF_MIN_LOG)
    {
      const Scalar sp = std::sqrt(2.0 * M_PI) * DistFunc::pNormal(-b / a);
      cdf -= std::exp(-0.5 * u1u2) * sp * b * (1.0 - bSquare * firstTerm);
    } // u1u2 < -2.0 * NORMAL2DCDF_MIN_LOG
    a *= 0.5;
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < 2; ++j)
      {
        const Scalar x = a * (1.0 + (-1.0 + 2.0 * j) * nodes[shift + i]);
        const Scalar xSquare = x * x;
        const Scalar asr = -0.5 * (bSquare / xSquare + u1u2);
        if (asr > NORMAL2DCDF_MIN_LOG)
        {
          const Scalar rs = std::sqrt(1.0 - xSquare);
          const Scalar sp = 1.0 + c * xSquare * (1.0 + d * xSquare);
          const Scalar ep = std::exp(-u1u2 * (1.0 - rs) / (2.0 * (1.0 + rs))) / rs;
          cdf += a * weights[shift + i] * std::exp(asr) * (ep - sp);
        } // asr > NORMAL2DCDF_MIN_LOG
      } // j
    } // i
  } // absRho < 1.0
  cdf /= -2.0 * M_PI;
  if (rho > 0.0) cdf = cdf + DistFunc::pNormal(-std::max(u1, u2));
  else cdf = -cdf + std::max(0.0, DistFunc::pNormal(-u1) - DistFunc::pNormal(-u2));
  return std::max(0.0, std::min(1.0, cdf));
}

#undef NORMAL2DCDF_INF
#undef NORMAL2DCDF_EPS
#undef NORMAL2DCDF_LOG

END_NAMESPACE_OPENTURNS
