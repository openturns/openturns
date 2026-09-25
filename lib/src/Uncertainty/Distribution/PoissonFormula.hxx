//                                               -*- C++ -*-
/**
 *  @brief Shared spectral machinery for distributions computed with
 *         Poisson's formula
 *
 *  Inversion of a sum of truncated univariate generating functions
 *  (phi_{0,a} and phi_{a,b}) and extraction of probability contents by a
 *  root of unity filter, as described in:
 *  R. Lebrun, "Efficient time/space algorithm to compute rectangular
 *  probabilities of multinomial, multivariate hypergeometric and
 *  multivariate Polya distributions", Statistics and Computing, submitted.
 *
 *  This private header is shared by the Multinomial, the
 *  MultivariateHypergeometric and the MultivariatePolya distributions.
 *  The only per-distribution ingredient is the univariate kernel model, so
 *  the truncation handling and the DFT loops are common.
 *
 *  Copyright 2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
 */

#ifndef OPENTURNS_POISSONFORMULA_HXX
#define OPENTURNS_POISSONFORMULA_HXX

#include <cmath>
#include "openturns/OTtypes.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace PoissonFormula
{

// Each kernel models the truncated sequence p(k) z^(k - a) of a single
// category. The recurrence factors are frame independent: up(w, k) maps the
// term at index k - 1 to the term at index k and down(w, k) the reverse.

/* Poisson kernel p(k) = exp(-lambda) lambda^k / k! */
struct PoissonKernel
{
  Scalar lambda_;
  Complex z_;
  Complex u_;
  Scalar mode_;

  // Constructor with the rate lambda and the generating-function argument z
  PoissonKernel(const Scalar lambda, const Complex & z)
    : lambda_(lambda)
    , z_(z)
    , u_(lambda * z)
    , mode_(lambda * std::abs(z))
  { }

  // Probability p(0) in the shifted frame, the seed at index 0
  Complex pmf0() const
  {
    return std::exp(-lambda_);
  }

  // p(k), the shifted frame seed at index k
  Complex pmfAt(const UnsignedInteger k) const
  {
    return std::exp(-lambda_ + (1.0 * k) * std::log(lambda_) - SpecFunc::LogGamma(1.0 * k + 1.0));
  }

  // p(k) z^k, the untruncated polynomial term
  Complex physical(const UnsignedInteger k) const
  {
    return std::exp(-lambda_ + (1.0 * k) * std::log(u_) - SpecFunc::LogGamma(1.0 * k + 1.0));
  }

  // Advance the term at index k - 1 to the term at index k
  Complex up(const Complex & term, const UnsignedInteger k) const
  {
    return term * u_ * (1.0 / k);
  }

  // Move the term at index k back to the term at index k - 1
  Complex down(const Complex & term, const UnsignedInteger k) const
  {
    return term * (1.0 * k) / u_;
  }

  // Full (untruncated) generating function
  Complex fullGF() const
  {
    return std::exp(-lambda_ + u_);
  }
};

/* Binomial kernel p(k) = Comb(c, k) t^k (1 - t)^(c - k), u = t z / (1 - t) */
struct BinomialKernel
{
  Scalar c_;
  Scalar t_;
  Complex z_;
  Complex u_;
  Scalar mode_;

  // Constructor with the size c, the success probability t and the argument z
  BinomialKernel(const Scalar c, const Scalar t, const Complex & z)
    : c_(c)
    , t_(t)
    , z_(z)
    , u_(t * z / (1.0 - t))
    , mode_((c + 1.0) * t * std::abs(z) / (1.0 - t + t * std::abs(z)))
  { }

  // Probability p(0) in the shifted frame, the seed at index 0
  Complex pmf0() const
  {
    return std::exp(c_ * SpecFunc::Log1p(-t_));
  }

  // p(k), the shifted frame seed at index k
  Complex pmfAt(const UnsignedInteger k) const
  {
    return std::exp(SpecFunc::LogGamma(c_ + 1.0) - SpecFunc::LogGamma(1.0 * k + 1.0)
                    - SpecFunc::LogGamma(c_ - 1.0 * k + 1.0) + (1.0 * k) * std::log(t_)
                    + (c_ - 1.0 * k) * SpecFunc::Log1p(-t_));
  }

  // p(k) z^k, the untruncated polynomial term
  Complex physical(const UnsignedInteger k) const
  {
    return std::exp(c_ * SpecFunc::Log1p(-t_) + SpecFunc::LogGamma(c_ + 1.0)
                    - SpecFunc::LogGamma(1.0 * k + 1.0) - SpecFunc::LogGamma(c_ - 1.0 * k + 1.0)
                    + (1.0 * k) * std::log(u_));
  }

  // Advance the term at index k - 1 to the term at index k
  Complex up(const Complex & term, const UnsignedInteger k) const
  {
    return term * u_ * (c_ - 1.0 * k + 1.0) * (1.0 / k);
  }

  // Move the term at index k back to the term at index k - 1
  Complex down(const Complex & term, const UnsignedInteger k) const
  {
    return term * (1.0 * k) / ((c_ - 1.0 * k + 1.0) * u_);
  }

  // Full (untruncated) generating function
  Complex fullGF() const
  {
    return std::exp(c_ * SpecFunc::Log1p(t_ * (z_ - 1.0)));
  }
};

/* Negative binomial kernel p(k) = Comb(k + u - 1, k) t^u (1 - t)^k,
   v = (1 - t) z */
struct NegativeBinomialKernel
{
  Scalar u_;
  Scalar t_;
  Complex z_;
  Complex v_;
  Scalar mode_;

  // Constructor with the shape u, the probability t and the argument z
  NegativeBinomialKernel(const Scalar u, const Scalar t, const Complex & z)
    : u_(u)
    , t_(t)
    , z_(z)
    , v_((1.0 - t) * z)
    , mode_((u - 1.0) * (1.0 - t) * std::abs(z) / (1.0 - (1.0 - t) * std::abs(z)))
  { }

  // Probability p(0) in the shifted frame, the seed at index 0
  Complex pmf0() const
  {
    return std::pow(t_, u_);
  }

  // p(k), the shifted frame seed at index k
  Complex pmfAt(const UnsignedInteger k) const
  {
    return std::exp(u_ * std::log(t_) + (1.0 * k) * SpecFunc::Log1p(-t_)
                    + SpecFunc::LogGamma(u_ + 1.0 * k) - SpecFunc::LogGamma(u_)
                    - SpecFunc::LogGamma(1.0 * k + 1.0));
  }

  // p(k) z^k, the untruncated polynomial term
  Complex physical(const UnsignedInteger k) const
  {
    return std::exp(u_ * std::log(t_) + (1.0 * k) * SpecFunc::Log1p(-t_)
                    + SpecFunc::LogGamma(u_ + 1.0 * k) - SpecFunc::LogGamma(u_)
                    - SpecFunc::LogGamma(1.0 * k + 1.0) + (1.0 * k) * std::log(z_));
  }

  // Advance the term at index k - 1 to the term at index k
  Complex up(const Complex & term, const UnsignedInteger k) const
  {
    return term * v_ * (u_ + 1.0 * k - 1.0) * (1.0 / k);
  }

  // Move the term at index k back to the term at index k - 1
  Complex down(const Complex & term, const UnsignedInteger k) const
  {
    return term * (1.0 * k) / ((u_ + 1.0 * k - 1.0) * v_);
  }

  // Full (untruncated) generating function
  Complex fullGF() const
  {
    return std::exp(u_ * (std::log(t_) - SpecFunc::Log1p(-v_)));
  }
};

/* Generating function of the truncated marginal sequence p(k) z^k over
   [0, a], phi_{0,a}(z) = sum_{k=0}^a p(k) z^k. */
template <class Kernel>
Complex localPhi0(const Complex & z, const Kernel & kernel, const UnsignedInteger a,
                  const Scalar smallA)
{
  if (z == 0.0) return kernel.pmf0();
  // Small value of a, evaluate the generating function as a polynomial
  if (a <= smallA)
  {
    Complex value(kernel.pmf0());
    Complex term(value);
    for (UnsignedInteger i = 1; i <= a; ++i)
    {
      term = kernel.up(term, i);
      value += term;
    }
    return value;
  } // smallA
  // If the truncation index is below the mode of the terms |p(k) z^k|, the
  // terms p(k) z^k for k > a first increase up to the mode then decrease, so
  // the relative early exit below would wrongly stop before the whole tail
  // mass has been removed. Evaluate the polynomial exactly by a decreasing
  // recurrence seeded at the truncation index.
  if ((1.0 * a) < kernel.mode_)
  {
    Complex value(kernel.physical(a));
    Complex term(value);
    for (UnsignedInteger i = a; i > 0; --i)
    {
      term = kernel.down(term, i);
      value += term;
    }
    return value;
  } // a below the mode
  // Large a, at or beyond the mode: the terms p(k) z^k decrease monotonically
  // for k > a, so the tail can be removed from the full generating function
  // with a relative early exit.
  Complex value(kernel.fullGF());
  UnsignedInteger i = a + 1;
  Complex term(kernel.physical(i));
  while (std::abs(term) > SpecFunc::Precision * std::abs(value))
  {
    value -= term;
    ++i;
    term = kernel.up(term, i);
  }
  return value - term;
} // localPhi0

/* Generating function of the truncated marginal sequence over [a, b],
   shifted back to the box lower bound:
   phi_{a,b}(z) = sum_{k=a}^b p(k) z^(k - a). */
template <class Kernel>
Complex localPhi(const Complex & z, const Kernel & kernel, const UnsignedInteger a,
                 const UnsignedInteger b, const Scalar smallA)
{
  if (a == 0) return localPhi0(z, kernel, b, smallA);
  if (z == 0.0) return 0.0;
  // Small value of b - a, evaluate the generating function as a polynomial
  if (b <= a + smallA)
  {
    Complex value(kernel.pmfAt(a));
    Complex term(value);
    for (UnsignedInteger i = 1; i <= b - a; ++i)
    {
      term = kernel.up(term, a + i);
      value += term;
    }
    return value;
  } // smallA
  // The relative early exits below are only valid when the removed terms
  // decrease monotonically, which requires the window to overlap the mode of
  // the terms |p(k) z^k|. Otherwise evaluate the polynomial directly,
  // whichever side of the mode the window lies on.
  const Scalar rho = kernel.mode_;
  if ((1.0 * b) < rho)
  {
    // The whole window lies below the mode: the terms p(k) z^(k - a) increase
    // from a, so start from a.
    Complex value(kernel.pmfAt(a));
    Complex term(value);
    for (UnsignedInteger i = 1; i <= b - a; ++i)
    {
      term = kernel.up(term, a + i);
      value += term;
    }
    return value;
  } // b below the mode
  if ((1.0 * a) > rho)
  {
    // The whole window lies above the mode: the terms p(k) z^(k - a) increase
    // as k decreases toward the mode, so start from b.
    Complex value(kernel.pmfAt(b) * std::pow(z, 1.0 * (b - a)));
    // A zero seed means the kernel is degenerate, so the whole window is empty
    if (std::abs(value) == 0.0) return 0.0;
    Complex term(value);
    for (UnsignedInteger i = b; i > a; --i)
    {
      term = kernel.down(term, i);
      value += term;
    }
    return value;
  } // a above the mode
  // Large b - a, and a <= rho <= b
  // Start from the non-truncated shifted generating function
  Complex value(kernel.fullGF() * std::pow(z, -1.0 * a));
  // And the lower terms
  SignedInteger i = a;
  Complex term(kernel.pmfAt(a));
  while (i >= 0 && std::abs(term) > SpecFunc::Precision * std::abs(value))
  {
    term = kernel.down(term, i);
    --i;
    value -= term;
  }
  // And the upper terms
  i = b;
  term = kernel.pmfAt(b) * std::pow(z, 1.0 * (b - a));
  while (std::abs(term) > SpecFunc::Precision * std::abs(value))
  {
    ++i;
    term = kernel.up(term, i);
    value -= term;
  }
  return value;
} // localPhi

/* Telescoped root of unity filter extracting P(W = n) from the generating
   function, used by the CDF computation. The sum is alternating and stops as
   soon as the partial sums become stable to a relative precision. */
template <typename Function>
inline Scalar computeCDF(const Function & phi,
                  const UnsignedInteger n,
                  const Scalar r,
                  const Scalar normalization,
                  const String & name)
{
  Complex phiK(phi(Complex(r, 0.0)));
  const Complex zetaN(std::exp(Complex(0.0, M_PI / n)));
  Complex phiKp1(phi(r * zetaN));
  Complex delta(phiK - phiKp1);
  Scalar value = delta.real();
  const Scalar dv0 = std::abs(delta);
  if (dv0 == 0.0)
  {
    LOGWARN("Underflow in " + name + "::computeCDF");
    return 0.0;
  }
  Scalar sign = -1.0;
  Complex t(zetaN);
  for (UnsignedInteger k = 1; k < n; ++k)
  {
    phiK = phiKp1;
    t *= zetaN;
    phiKp1 = phi(r * t);
    delta = phiK - phiKp1;
    value += sign * delta.real();
    const Scalar dv = std::abs(delta);
    if (dv < SpecFunc::Precision * dv0) break;
    sign = -sign;
  }
  // Due to round-off errors, the computed CDF can be slightly below 0 or over 1.
  return SpecFunc::Clip01(value * normalization);
} // computeCDF

/* Root of unity filter extracting the probability content of a box from the
   generating function, with the diametral term handling the odd coefficient. */
template <typename Function>
inline Scalar computeProbability(const Function & phi,
                          const UnsignedInteger nA,
                          const Scalar r,
                          const Scalar logCoefNorm,
                          const Scalar logDiametral)
{
  const Scalar coefNorm = std::exp(-logCoefNorm - logDiametral) / (2.0 * nA);
  Scalar value = coefNorm * phi(Complex(r, 0.0)).real();
  Scalar delta = SpecFunc::Infinity;
  Scalar sign2 = -2.0 * coefNorm;
  for (UnsignedInteger k = 1; k < nA; ++k)
  {
    if (std::abs(delta) <= SpecFunc::Precision * std::abs(value))
      break;
    const Complex zeta(r * std::exp(Complex(0.0, k * M_PI / nA)));
    delta = sign2 * phi(zeta).real();
    value += delta;
    sign2 = -sign2;
  }
  // Check if we have to take the last term into account
  if (std::abs(delta) > SpecFunc::Precision * std::abs(value))
  {
    delta = coefNorm * phi(Complex(-r, 0.0)).real();
    if (nA % 2 == 0) value += delta;
    else value -= delta;
  }
  return SpecFunc::Clip01(value);
} // computeProbability

} // namespace PoissonFormula

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_POISSONFORMULA_HXX