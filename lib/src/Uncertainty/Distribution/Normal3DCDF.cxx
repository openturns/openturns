//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the CDF computation for a bi-dimensional
 *         Normal distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/Normal3DCDF.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

Scalar adonet(const Scalar h1,
              const Scalar h2,
              const Scalar h3,
              const Scalar r23,
              const Scalar a12,
              const Scalar a13);

Scalar tvnf(const Scalar x,
            const Scalar h1,
            const Scalar h2,
            const Scalar h3,
            const Scalar r23,
            const Scalar a12,
            const Scalar a13);

void sincs(const Scalar x,
           Scalar & sx,
           Scalar & cs);

Scalar pntgnd(const Scalar ba,
              const Scalar bb,
              const Scalar bc,
              const Scalar ra,
              const Scalar rb,
              const Scalar r,
              const Scalar rr);

void krnrdt(const Scalar a,
            const Scalar b,
            const Scalar h1,
            const Scalar h2,
            const Scalar h3,
            const Scalar r23,
            const Scalar a12,
            const Scalar a13,
            Scalar & resk,
            Scalar & err);

/*
  Translated in C++ from Alan Genz's matlab routine tvnl.m
  The routine tvnl.m has the following copyright:
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
// Opposite convention for INF wrt Normal2DCDF
/*
  #define NORMAL3DCDF_MINUS_INF -37.5193794
  #define NORMAL3DCDF_PLUS_INF 8.29236108
  #define NORMAL3DCDF_MIN_LOG -745.13321
  #define NORMAL3DCDF_EPS 1.0e-14
  #define NORMAL3DCDF_MAXINT 100
*/
#define NORMAL3DCDF_MINUS_INF -37.5193794
#define NORMAL3DCDF_PLUS_INF 8.29236108
#define NORMAL3DCDF_MIN_LOG -745.13321
#define NORMAL3DCDF_EPS 1.0e-14
#define NORMAL3DCDF_MAXINT 100

Scalar Normal3DCDF(const Scalar x1,
                   const Scalar x2,
                   const Scalar x3,
                   const Scalar rho12,
                   const Scalar rho13,
                   const Scalar rho23,
                   const Bool tail)
{
  if (!(std::abs(rho12) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient rho12 must be in [-1, 1], here rho12=" << rho12;
  if (!(std::abs(rho13) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient rho13 must be in [-1, 1], here rho12=" << rho13;
  if (!(std::abs(rho23) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient rho23 must be in [-1, 1], here rho23=" << rho23;
  const Scalar delta = rho12 * rho12 + rho13 * rho13 + rho23 * rho23 - 2.0 * rho12 * rho13 * rho23;
  if (!(delta <= 1.0)) throw InvalidArgumentException(HERE) << "Error: delta=rho12^2+rho13^2+rho23^2-2*rho12*rho13*rho23 must be in less or equal to 1, here delta=" << delta;
  if (tail) return Normal3DCDF(-x1, -x2, -x3, rho12, rho13, rho23, false);
  if ((x1 <= NORMAL3DCDF_MINUS_INF) || (x2 <= NORMAL3DCDF_MINUS_INF) || (x3 <= NORMAL3DCDF_MINUS_INF)) return 0.0;
  if (x1 >= NORMAL3DCDF_PLUS_INF)
  {
    if (x2 >= NORMAL3DCDF_PLUS_INF)
    {
      if (x3 >= NORMAL3DCDF_PLUS_INF) return 1.0;
      else return DistFunc::pNormal(x3);
    } // x2 >= NORMAL3DCDF_PLUS_INF
    // Here, x2 < NORMAL3DCDF_PLUS_INF
    if (x3 >= NORMAL3DCDF_PLUS_INF) return DistFunc::pNormal(x2);
    else return DistFunc::pNormal2D(x2, x3, rho23);
  }  // x1 >= NORMAL3DCDF_PLUS_INF
  // Here, x1 < NORMAL3DCDF_PLUS_INF
  if (x2 >= NORMAL3DCDF_PLUS_INF)
  {
    if (x3 >= NORMAL3DCDF_PLUS_INF) return DistFunc::pNormal(x1);
    else return DistFunc::pNormal2D(x1, x3, rho13);
  } // x2 >= NORMAL3DCDF_PLUS_INF
  // Here, x1 < NORMAL3DCDF_PLUS_INF and x2 < NORMAL3DCDF_PLUS_INF
  if (x3 >= NORMAL3DCDF_PLUS_INF) return DistFunc::pNormal2D(x1, x2, rho12);
  // Here, we have to do some work!
  // Probability of the negative orthant
  if (std::abs(x1) + std::abs(x2) + std::abs(x3) < NORMAL3DCDF_EPS) return std::max(0.0, std::min(1.0, 0.125 * (1.0 + 2.0 * (std::asin(rho12) + std::asin(rho13) + std::asin(rho23)) / M_PI)));
  Scalar h1 = x1;
  Scalar h2 = x2;
  Scalar h3 = x3;
  Scalar r12 = rho12;
  Scalar r13 = rho13;
  Scalar r23 = rho23;
  // Sort R's and check for special correlation structure
  if (std::abs(r12) > std::abs(r13))
  {
    h2 = h3;
    h3 = x2;
    r12 = r13;
    r13 = rho12;
  }
  if (std::abs(r13) > std::abs(r23))
  {
    h1 = h2;
    h2 = x1;
    r23 = r13;
    r13 = rho23;
  }
  if (std::abs(r12) + std::abs(r13) < NORMAL3DCDF_EPS) return DistFunc::pNormal(h1) * DistFunc::pNormal2D(h2, h3, r23);
  if (std::abs(r13) + std::abs(r23) < NORMAL3DCDF_EPS) return DistFunc::pNormal(h3) * DistFunc::pNormal2D(h1, h2, r12);
  if (std::abs(r12) + std::abs(r23) < NORMAL3DCDF_EPS) return DistFunc::pNormal(h2) * DistFunc::pNormal2D(h1, h3, r13);
  if (1.0 - r23 < NORMAL3DCDF_EPS) return DistFunc::pNormal2D(h1, std::min(h2, h3), r12);
  if ((r23 + 1.0 < NORMAL3DCDF_EPS) &&  (h2 > -h3)) return std::max(0.0, std::min(1.0, DistFunc::pNormal2D(h1, h2, r12) - DistFunc::pNormal2D(h1, -h3, r12)));
  // At last, the general case
  const Scalar a12 = std::asin(r12);
  const Scalar a13 = std::asin(r13);
  return std::max(0.0, std::min(1.0, adonet(h1, h2, h3, r23, a12, a13) / (2.0 * M_PI) + DistFunc::pNormal(h1) * DistFunc::pNormal2D(h2, h3, r23)));
}

/*
  Computes Plackett formula integrands
*/
Scalar tvnf(const Scalar x,
            const Scalar h1,
            const Scalar h2,
            const Scalar h3,
            const Scalar r23,
            const Scalar a12,
            const Scalar a13)
{
  Scalar result = 0.0;
  Scalar r12 = -1.0;
  Scalar rr2 = -1.0;
  sincs(a12 * x, r12, rr2);
  Scalar r13 = -1.0;
  Scalar rr3 = -1.0;
  sincs(a13 * x, r13, rr3);
  if (std::abs(a12) > 0.0) result  = a12 * pntgnd(h1, h2, h3, r13, r23, r12, rr2);
  if (std::abs(a13) > 0.0) result += a13 * pntgnd(h1, h3, h2, r12, r23, r13, rr3);
  return result;
}

/*
  Computes accurately sin(x), cos(x)^2 for |x| near pi/2
*/
void sincs(const Scalar x,
           Scalar & sx,
           Scalar & cs)
{
  const Scalar e = 0.5 * M_PI - std::abs(x);
  const Scalar ee = e * e;
  if (ee < 5.0e-5)
  {
    cs = ee * (1.0 - ee * (1.0 - 2.0 * ee / 15.0) / 3.0);
    sx = (1.0 - 0.5 * ee * (1.0 - ee / 12.0));
    if (x < 0.0) sx = -sx;
    return;
  }
  sx = std::sin(x);
  cs = 1.0 - sx * sx;
}

/*
  Computes Plackett formula integrand
*/
Scalar pntgnd(const Scalar ba,
              const Scalar bb,
              const Scalar bc,
              const Scalar ra,
              const Scalar rb,
              const Scalar r,
              const Scalar rr)
{
  Scalar result = 0.0;
  const Scalar dt = rr * (rr - (ra - rb) * (ra - rb) - 2.0 * ra * rb * (1.0 - r));
  if (dt > 0.0)
  {
    const Scalar bt = (bc * rr + ba * (r * rb - ra) + bb * (r * ra - rb)) / std::sqrt(dt);
    const Scalar delta = ba - r * bb;
    const Scalar ft = delta * delta / rr + bb * bb;
    if ((ft < -2.0 * NORMAL3DCDF_MIN_LOG) && (bt > NORMAL3DCDF_MINUS_INF))
    {
      result = std::exp(-0.5 * ft);
      if (bt < NORMAL3DCDF_PLUS_INF) result *= DistFunc::pNormal(bt);
    }
  }
  return result;
}

/*
  1D adaptive integration
*/
Scalar adonet(const Scalar h1,
              const Scalar h2,
              const Scalar h3,
              const Scalar r23,
              const Scalar a12,
              const Scalar a13)
{
  Scalar result = 0.0;
  Scalar ai[NORMAL3DCDF_MAXINT];
  ai[0] = 0.0;
  Scalar bi[NORMAL3DCDF_MAXINT];
  bi[0] = 1.0;
  Scalar fi[NORMAL3DCDF_MAXINT];
  Scalar ei[NORMAL3DCDF_MAXINT];
  UnsignedInteger ip = 0;
  UnsignedInteger im = 0;
  Scalar err = 1.0;
  while ((err > 0.25 * NORMAL3DCDF_EPS) && (im < NORMAL3DCDF_MAXINT - 1))
  {
    ++im;
    bi[im] = bi[ip];
    ai[im] = 0.5 * (ai[ip] + bi[ip]);
    bi[ip] = ai[im];
    krnrdt(ai[ip], bi[ip], h1, h2, h3, r23, a12, a13, fi[ip], ei[ip]);
    krnrdt(ai[im], bi[im], h1, h2, h3, r23, a12, a13, fi[im], ei[im]);
    UnsignedInteger iErrMax = 0;
    Scalar errMax = 0.0;
    err = 0.0;
    result = 0.0;
    for (UnsignedInteger i = 0; i <= im; ++i)
    {
      const Scalar localError = ei[i];
      result += fi[i];
      err += localError * localError;
      if (localError > errMax)
      {
        errMax = localError;
        iErrMax = i;
      }
    }
    ip = iErrMax;
    err = std::sqrt(err);
  } // while (err >...)
  return result;
}

/*
  Kronrod rule
*/
void krnrdt(const Scalar a,
            const Scalar b,
            const Scalar h1,
            const Scalar h2,
            const Scalar h3,
            const Scalar r23,
            const Scalar a12,
            const Scalar a13,
            Scalar & resk,
            Scalar & err)
{
  static Scalar wg0(0.2729250867779007);
  static Scalar wg[5] = {0.05566856711617449, 0.1255803694649048, 0.1862902109277352, 0.2331937645919914, 0.2628045445102478};
  static Scalar xgk[11] = {0.9963696138895427, 0.9782286581460570, 0.9416771085780681, 0.8870625997680953, 0.8160574566562211, 0.7301520055740492, 0.6305995201619651, 0.5190961292068118, 0.3979441409523776, 0.2695431559523450, 0.1361130007993617};
  static Scalar wgk0(0.1365777947111183);
  static Scalar wgk[11] = {0.00976544104596129, 0.02715655468210443, 0.04582937856442671, 0.06309742475037484, 0.07866457193222764, 0.09295309859690074, 0.1058720744813894, 0.1167395024610472, 0.1251587991003195, 0.1312806842298057, 0.1351935727998845};
  const Scalar wid = 0.5 * (b - a);
  const Scalar cen = 0.5 * (a + b);
  Scalar fc = tvnf(cen, h1, h2, h3, r23, a12, a13);
  Scalar resg = fc * wg0;
  resk = fc * wgk0;
  Scalar t = 0.0;
  for (UnsignedInteger j = 0; j < 5; ++j)
  {
    t = wid * xgk[2 * j];
    fc = tvnf(cen - t, h1, h2, h3, r23, a12, a13) + tvnf(cen + t, h1, h2, h3, r23, a12, a13);
    resk += wgk[2 * j] * fc;
    t = wid * xgk[2 * j + 1];
    fc = tvnf(cen - t, h1, h2, h3, r23, a12, a13) + tvnf(cen + t, h1, h2, h3, r23, a12, a13);
    resk += wgk[2 * j + 1] * fc;
    resg += wg[j] * fc;
  }
  t = wid * xgk[10];
  fc = tvnf(cen - t, h1, h2, h3, r23, a12, a13) + tvnf(cen + t, h1, h2, h3, r23, a12, a13);
  resk = wid * (resk + wgk[10] * fc);
  err = std::abs(resk - wid * resg);
}

#undef NORMAL3DCDF_INF
#undef NORMAL3DCDF_LOG
#undef NORMAL3DCDF_EPS
#undef NORMAL3DCDF_MAXINT

END_NAMESPACE_OPENTURNS
