//                                               -*- C++ -*-
/**
 *  @brief Bivariate Student CDF and rectangular probability
 *
 *  Implementation of BVTL from tvpack.f by Alan Genz.
 *  Based on Dunnett, C.W. and M. Sobel, (1954),
 *  "A bivariate generalization of Student's t-distribution
 *  with tables for certain special cases",
 *  Biometrika 41, pp. 153-169.
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

#include "openturns/Student2DCDF.hxx"
#include "openturns/Exception.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Function.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/Student.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace
{

// Helper to create a UniVariateFunction for GaussKronrod integration
class Student2DCDFKernelEvaluation
  : public UniVariateFunctionImplementation
{
public:
  Student2DCDFKernelEvaluation(const Scalar x1,
                               const Scalar x2,
                               const Scalar nu)
    : x1_(x1)
    , x2_(x2)
    , nu_(nu)
  {
    // Nothing to do
  }

  Student2DCDFKernelEvaluation * clone() const override
  {
    return new Student2DCDFKernelEvaluation(*this);
  }

  Scalar operator()(const Scalar theta) const override
  {
    const Scalar sinTheta = std::sin(theta);
    const Scalar cosTheta = std::cos(theta);
    const Scalar tNumer = x1_ * sinTheta - x2_;
    const Scalar t = tNumer * tNumer / (cosTheta * cosTheta) + x1_ * x1_;
    return 1.0 / std::pow(1.0 + t / nu_, 0.5 * nu_);
  }

private:
  const Scalar x1_;
  const Scalar x2_;
  const Scalar nu_;
};

// BVTL computes P(X < DH, Y < DK) for bivariate Student t
// Translated from the Fortran BVTL function in tvpack.f
Scalar BVTL(const UnsignedInteger nuInt,
            const Scalar dh,
            const Scalar dk,
            const Scalar r)
{
  if (!(std::abs(r) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient must be in [-1,1], here rho=" << r;

  const Scalar eps = SpecFunc::Precision;
  // Perfect positive correlation
  if (1.0 - r <= eps) return DistFunc::pStudent(nuInt, std::min(dh, dk));
  // Perfect negative correlation
  if (r + 1.0 <= eps)
  {
    if (dh > -dk) return SpecFunc::Clip01(DistFunc::pStudent(nuInt, dh) - DistFunc::pStudent(nuInt, -dk));
    return 0.0;
  }
  // General case: |r| < 1
  const Scalar pi = M_PI;
  const Scalar tpi = 2.0 * pi;
  const Scalar snu = std::sqrt(nuInt);
  const Scalar ors = 1.0 - r * r;
  const Scalar hrk = dh - r * dk;
  const Scalar krh = dk - r * dh;

  Scalar xnhk = 0.0;
  Scalar xnkh = 0.0;
  if (std::abs(hrk) + ors > 0.0)
  {
    xnhk = hrk * hrk / (hrk * hrk + ors * (nuInt + dk * dk));
    xnkh = krh * krh / (krh * krh + ors * (nuInt + dh * dh));
  }

  const Scalar hs = (dh >= r * dk) ? 1.0 : -1.0;
  const Scalar ks = (dk >= r * dh) ? 1.0 : -1.0;

  Scalar bvt = 0.0;
  if (nuInt % 2 == 0)
  {
    // Even degrees of freedom
    bvt = std::atan2(std::sqrt(ors), -r) / tpi;
    Scalar gmph = dh / std::sqrt(16.0 * (nuInt + dh * dh));
    Scalar gmpk = dk / std::sqrt(16.0 * (nuInt + dk * dk));
    Scalar btnckh = 2.0 * std::atan2(std::sqrt(xnkh), std::sqrt(1.0 - xnkh)) / pi;
    Scalar btpdkH = 2.0 * std::sqrt(xnkh * (1.0 - xnkh)) / pi;
    Scalar btnchk = 2.0 * std::atan2(std::sqrt(xnhk), std::sqrt(1.0 - xnhk)) / pi;
    Scalar btpdhK = 2.0 * std::sqrt(xnhk * (1.0 - xnhk)) / pi;

    for (UnsignedInteger j = 1; j <= nuInt / 2; ++j)
    {
      bvt += gmph * (1.0 + ks * btnckh);
      bvt += gmpk * (1.0 + hs * btnchk);
      btnckh += btpdkH;
      btpdkH = 2.0 * j * btpdkH * (1.0 - xnkh) / (2.0 * j + 1.0);
      btnchk += btpdhK;
      btpdhK = 2.0 * j * btpdhK * (1.0 - xnhk) / (2.0 * j + 1.0);
      gmph *= (2.0 * j - 1.0) / (2.0 * j * (1.0 + dh * dh / nuInt));
      gmpk *= (2.0 * j - 1.0) / (2.0 * j * (1.0 + dk * dk / nuInt));
    }
  }
  else
  {
    // Odd degrees of freedom
    const Scalar qhrk = std::sqrt(dh * dh + dk * dk - 2.0 * r * dh * dk + nuInt * ors);
    const Scalar hkrn = dh * dk + r * nuInt;
    const Scalar hkn = dh * dk - nuInt;
    const Scalar hpk = dh + dk;
    bvt = std::atan2(-snu * (hkn * qhrk + hpk * hkrn), hkn * hkrn - nuInt * hpk * qhrk) / tpi;
    if (bvt < -eps) bvt += 1.0;

    Scalar gmph = dh / (tpi * snu * (1.0 + dh * dh / nuInt));
    Scalar gmpk = dk / (tpi * snu * (1.0 + dk * dk / nuInt));
    Scalar btnckh = std::sqrt(xnkh);
    Scalar btpdkH = btnckh;
    Scalar btnchk = std::sqrt(xnhk);
    Scalar btpdhK = btnchk;

    for (UnsignedInteger j = 1; j <= (nuInt - 1) / 2; ++j)
    {
      bvt += gmph * (1.0 + ks * btnckh);
      bvt += gmpk * (1.0 + hs * btnchk);
      btpdkH = (2.0 * j - 1.0) * btpdkH * (1.0 - xnkh) / (2.0 * j);
      btnckh += btpdkH;
      btpdhK = (2.0 * j - 1.0) * btpdhK * (1.0 - xnhk) / (2.0 * j);
      btnchk += btpdhK;
      gmph = 2.0 * j * gmph / ((2.0 * j + 1.0) * (1.0 + dh * dh / nuInt));
      gmpk = 2.0 * j * gmpk / ((2.0 * j + 1.0) * (1.0 + dk * dk / nuInt));
    }
  }
  return SpecFunc::Clip01(bvt);
}

} // anonymous namespace

/* CDF */
Scalar Student2DCDF(const Scalar nu,
                    const Scalar x1,
                    const Scalar x2,
                    const Scalar rho,
                    const Bool tail)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  if (tail)
    return Student2DCDF(nu, -x1, -x2, rho, false);
  // Use BVTL only for integral nu
  const UnsignedInteger nuInt = static_cast<UnsignedInteger>(nu);
  if (nu == nuInt)
    return SpecFunc::Clip01(BVTL(nuInt, x1, x2, rho));
  // Non-integral nu
  Scalar p1 = 0.0;
  Scalar p2 = 0.0;
  Scalar hiLimit = 0.0;
  if (rho >= 0.0)
  {
    p1 = DistFunc::pStudent(nu, std::min(x1, x2));
    hiLimit = 0.5 * M_PI;
  }
  else
  {
    p1 = DistFunc::pStudent(nu, x1) - DistFunc::pStudent(nu, -x2);
    hiLimit = -0.5 * M_PI;
  }
  if (std::abs(rho) < 1.0)
  {
    const Scalar loLimit = std::asin(rho);
    if (std::isfinite(x1) && std::isfinite(x2))
    {
      const UniVariateFunction kernel(new Student2DCDFKernelEvaluation(x1, x2, nu));
      p2 = GaussKronrod().integrate(kernel, loLimit, hiLimit);
    }
  }
  return SpecFunc::Clip01(p1 - p2 / (2.0 * M_PI));
}

/* Rectangular probability */
Scalar Student2DRectangularProbability(const Scalar nu,
                                       const Scalar a1,
                                       const Scalar a2,
                                       const Scalar b1,
                                       const Scalar b2,
                                       const Scalar rho)
{
  if (!(std::abs(rho) <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the correlation coefficient must be in [-1,1], here rho=" << rho;
  if (a1 >= b1 || a2 >= b2) return 0.0;
  // Inclusion-exclusion using the bivariate CDF.
  const Scalar prob = SpecFunc::Clip01((Student2DCDF(nu, b1, b2, rho, false)
                                  - Student2DCDF(nu, a1, b2, rho, false))
                                 - (Student2DCDF(nu, b1, a2, rho, false)
                                    - Student2DCDF(nu, a1, a2, rho, false)));
  if (prob > 0.0) return prob;
  // Fallback to multivariate integration when prob == 0.0
  CorrelationMatrix R(2);
  R(0, 1) = rho;
  const Student distribution(nu, Point(2, 0.0), Point(2, 1.0), R);
  const Point lower({a1, a2});
  const Point upper({b1, b2});
  const Interval interval(Interval(lower, upper).intersect(distribution.getRange()));
  if (interval.isEmpty()) return 0.0;
  return IteratedQuadrature().integrate(distribution.getPDF(), interval)[0];
}

END_NAMESPACE_OPENTURNS
