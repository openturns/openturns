//                                               -*- C++ -*-
/**
 *  @brief Trivariate Student CDF and rectangular probability
 *
 *  Implementation of TVTL from tvpack.f by Alan Genz.
 *  Based on:
 *    R.L. Plackett, Biometrika 41(1954), pp. 351-360.
 *    Z. Drezner, Math. Comp. 62(1994), pp. 289-294.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/Student3DCDF.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/Student.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace
{

const Scalar STUDENT3DCDF_MIN_LOG = -745.13321;
const Scalar STUDENT3DCDF_EPS = 1.0e-14;
const UnsignedInteger STUDENT3DCDF_MAXINT = 100;

/*
  Computes sin(x) and cos(x)^2 with series approximation for |x| near pi/2
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
  Computes Plackett formula integrand with Student t support
*/
Scalar pntgnd_nu(const Scalar nu,
                 const Scalar ba,
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
    Scalar ft = delta * delta / rr + bb * bb;
    // Student case: STUDNT(NU, BT/FT) / FT**NU where FT = sqrt(1 + FT/NU)
    ft = std::sqrt(1.0 + ft / nu);
    result = DistFunc::pStudent(nu, bt / ft);
    if (ft > 0.0) result /= std::pow(ft, nu);
  }
  return result;
}

/*
  Computes Plackett formula integrands (TVTMFN in Fortran)
*/
Scalar tvtmfn(const Scalar x,
              const Scalar nu,
              const Scalar h1,
              const Scalar h2,
              const Scalar h3,
              const Scalar r23,
              const Scalar rua,
              const Scalar rub,
              const Scalar ar,
              const Scalar ruc)
{
  Scalar result = 0.0;
  Scalar r12 = -1.0;
  Scalar rr2 = -1.0;
  sincs(rua * x, r12, rr2);
  Scalar r13 = -1.0;
  Scalar rr3 = -1.0;
  sincs(rub * x, r13, rr3);
  if (std::abs(rua) > 0.0) result += rua * pntgnd_nu(nu, h1, h2, h3, r13, r23, r12, rr2);
  if (std::abs(rub) > 0.0) result += rub * pntgnd_nu(nu, h1, h3, h2, r12, r23, r13, rr3);
  Scalar r = -1.0;
  Scalar rr = -1.0;
  sincs(ar + ruc * x, r, rr);
  result -= ruc * pntgnd_nu(nu, h2, h3, h1, 0.0, 0.0, r, rr);
  return result;
}

/*
  Kronrod rule
*/
void krnrdt_nu(const Scalar a,
               const Scalar b,
               const Scalar nu,
               const Scalar h1,
               const Scalar h2,
               const Scalar h3,
               const Scalar r23,
               const Scalar rua,
               const Scalar rub,
               const Scalar ar,
               const Scalar ruc,
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
  Scalar fc = tvtmfn(cen, nu, h1, h2, h3, r23, rua, rub, ar, ruc);
  Scalar resg = fc * wg0;
  resk = fc * wgk0;
  Scalar t = 0.0;
  for (UnsignedInteger j = 0; j < 5; ++j)
  {
    t = wid * xgk[2 * j];
    fc = tvtmfn(cen - t, nu, h1, h2, h3, r23, rua, rub, ar, ruc) + tvtmfn(cen + t, nu, h1, h2, h3, r23, rua, rub, ar, ruc);
    resk += wgk[2 * j] * fc;
    t = wid * xgk[2 * j + 1];
    fc = tvtmfn(cen - t, nu, h1, h2, h3, r23, rua, rub, ar, ruc) + tvtmfn(cen + t, nu, h1, h2, h3, r23, rua, rub, ar, ruc);
    resk += wgk[2 * j + 1] * fc;
    resg += wg[j] * fc;
  }
  t = wid * xgk[10];
  fc = tvtmfn(cen - t, nu, h1, h2, h3, r23, rua, rub, ar, ruc) + tvtmfn(cen + t, nu, h1, h2, h3, r23, rua, rub, ar, ruc);
  resk = wid * (resk + wgk[10] * fc);
  err = std::abs(resk - wid * resg);
}

/*
  1D adaptive integration
*/
Scalar adonet_nu(const Scalar nu,
                 const Scalar h1,
                 const Scalar h2,
                 const Scalar h3,
                 const Scalar r23,
                 const Scalar rua,
                 const Scalar rub,
                 const Scalar ar,
                 const Scalar ruc)
{
  Scalar result = 0.0;
  Scalar ai[STUDENT3DCDF_MAXINT];
  ai[0] = 0.0;
  Scalar bi[STUDENT3DCDF_MAXINT];
  bi[0] = 1.0;
  Scalar fi[STUDENT3DCDF_MAXINT];
  Scalar ei[STUDENT3DCDF_MAXINT];
  UnsignedInteger ip = 0;
  UnsignedInteger im = 0;
  Scalar err = 1.0;
  while ((err > 0.25 * STUDENT3DCDF_EPS) && (im < STUDENT3DCDF_MAXINT - 1))
  {
    ++im;
    bi[im] = bi[ip];
    ai[im] = 0.5 * (ai[ip] + bi[ip]);
    bi[ip] = ai[im];
    krnrdt_nu(ai[ip], bi[ip], nu, h1, h2, h3, r23, rua, rub, ar, ruc, fi[ip], ei[ip]);
    krnrdt_nu(ai[im], bi[im], nu, h1, h2, h3, r23, rua, rub, ar, ruc, fi[im], ei[im]);
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
  }
  return result;
}

} // anonymous namespace

/*
  Trivariate Student CDF
*/
Scalar Student3DCDF(const Scalar nu,
                    const Scalar x1,
                    const Scalar x2,
                    const Scalar x3,
                    const Scalar rho12,
                    const Scalar rho13,
                    const Scalar rho23,
                    const Bool tail)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  if (!(std::abs(rho12) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient rho12 must be in [-1, 1], here rho12=" << rho12;
  if (!(std::abs(rho13) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient rho13 must be in [-1, 1], here rho13=" << rho13;
  if (!(std::abs(rho23) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient rho23 must be in [-1, 1], here rho23=" << rho23;
  const Scalar delta = rho12 * rho12 + rho13 * rho13 + rho23 * rho23 - 2.0 * rho12 * rho13 * rho23;
  if (!(delta <= 1.0)) throw InvalidArgumentException(HERE) << "Error: delta=rho12^2+rho13^2+rho23^2-2*rho12*rho13*rho23 must be less or equal to 1, here delta=" << delta;

  if (tail) return Student3DCDF(nu, -x1, -x2, -x3, rho12, rho13, rho23, false);

  if ((x1 <= DistFunc::qStudent(nu, STUDENT3DCDF_EPS)) || (x2 <= DistFunc::qStudent(nu, STUDENT3DCDF_EPS)) || (x3 <= DistFunc::qStudent(nu, STUDENT3DCDF_EPS))) return 0.0;

  // Reduce dimension when arguments go to +inf
  if (x1 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true))
  {
    if (x2 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true))
    {
      if (x3 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true)) return 1.0;
      else return DistFunc::pStudent(nu, x3);
    }
    if (x3 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true)) return DistFunc::pStudent(nu, x2);
    else return DistFunc::pStudent2D(nu, x2, x3, rho23);
  }
  if (x2 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true))
  {
    if (x3 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true)) return DistFunc::pStudent(nu, x1);
    else return DistFunc::pStudent2D(nu, x1, x3, rho13);
  }
  if (x3 >= DistFunc::qStudent(nu, STUDENT3DCDF_EPS, true)) return DistFunc::pStudent2D(nu, x1, x2, rho12);

  // Near-origin case
  if (std::abs(x1) + std::abs(x2) + std::abs(x3) < STUDENT3DCDF_EPS)
    return SpecFunc::Clip01(0.125 * (1.0 + 2.0 * (std::asin(rho12) + std::asin(rho13) + std::asin(rho23)) / M_PI));

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

  // Perfect correlation cases
  if (1.0 - r23 < STUDENT3DCDF_EPS)
    return DistFunc::pStudent2D(nu, h1, std::min(h2, h3), r12);
  if ((r23 + 1.0 < STUDENT3DCDF_EPS) && (h2 > -h3))
    return SpecFunc::Clip01(DistFunc::pStudent2D(nu, h1, h2, r12) - DistFunc::pStudent2D(nu, h1, -h3, r12));

  // General case: compute singular TVT value + adaptive integration
  const Scalar pi = M_PI;
  const Scalar halfPi = 0.5 * pi;
  Scalar tvt = 0.0;

  if (r23 >= 0.0)
  {
    tvt = DistFunc::pStudent2D(nu, h1, std::min(h2, h3), 0.0);
  }
  else if (h2 > -h3)
  {
    tvt = DistFunc::pStudent2D(nu, h1, h2, 0.0) - DistFunc::pStudent2D(nu, h1, -h3, 0.0);
  }

  const Scalar rua = std::asin(r12);
  const Scalar rub = std::asin(r13);
  const Scalar ar = std::asin(r23);
  const Scalar ruc = (ar >= 0.0 ? halfPi : -halfPi) - ar;

  tvt += adonet_nu(nu, h1, h2, h3, r23, rua, rub, ar, ruc) / (2.0 * pi);
  return SpecFunc::Clip01(tvt);
}

/*
  Trivariate Student rectangular probability via inclusion-exclusion
*/
Scalar Student3DRectangularProbability(const Scalar nu,
                                       const Scalar a1,
                                       const Scalar a2,
                                       const Scalar a3,
                                       const Scalar b1,
                                       const Scalar b2,
                                       const Scalar b3,
                                       const Scalar rho12,
                                       const Scalar rho13,
                                       const Scalar rho23)
{
  if ((a1 >= b1) || (a2 >= b2) || (a3 >= b3)) return 0.0;
  Scalar prob = 0.0;
  const Scalar x1[2] = {b1, a1};
  const Scalar x2[2] = {b2, a2};
  const Scalar x3[2] = {b3, a3};
  for (UnsignedInteger i = 0; i < 2; ++i)
  {
    for (UnsignedInteger j = 0; j < 2; ++j)
    {
      for (UnsignedInteger k = 0; k < 2; ++k)
      {
        const Scalar sign = ((i + j + k) % 2 == 0) ? 1.0 : -1.0;
        prob += sign * Student3DCDF(nu, x1[i], x2[j], x3[k], rho12, rho13, rho23, false);
      }
    }
  }
  prob = SpecFunc::Clip01(prob);
  if (prob > 0.0) return prob;
  // Fallback to multivariate integration when prob == 0.0
  CorrelationMatrix R(3);
  R(0, 1) = rho12;
  R(0, 2) = rho13;
  R(1, 2) = rho23;
  const Student distribution(nu, Point(3, 0.0), Point(3, 1.0), R);
  const Point lower({a1, a2, a3});
  const Point upper({b1, b2, b3});
  const Interval interval(Interval(lower, upper).intersect(distribution.getRange()));
  if (interval.isEmpty()) return 0.0;
  return IteratedQuadrature().integrate(distribution.getPDF(), interval)[0];
}

END_NAMESPACE_OPENTURNS
