//                                               -*- C++ -*-
/**
 *  @brief Genz algorithm to compute rectangular probabilities of multivariate
 *         Normal and Student distributions.
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

#include "openturns/Genz.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/LowDiscrepancySequence.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/HaltonSequence.hxx"
#include "openturns/ReverseHaltonSequence.hxx"
#include "openturns/FaureSequence.hxx"
#include "openturns/HaselgroveSequence.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace {

Scalar mvn_genz(
    const Point & a,
    const Point & b,
    const TriangularMatrix & L,
    const Sample & sobol,
    const UnsignedInteger k,
    const UnsignedInteger offset,
    const UnsignedInteger d)
{
  Scalar p = 1.0;
  Point y(d, 0.0);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    // Degenerate dimension: the calling code should have marginalized it
    if (L(i, i) <= 0.0) return 0.0;
    const Scalar ai = (a[i] - y[i]) / L(i, i);
    const Scalar bi = (b[i] - y[i]) / L(i, i);
    const Scalar ei = DistFunc::pNormal(ai);
    const Scalar fi = DistFunc::pNormal(bi);
    p *= (fi - ei);
    if (p <= 0.0) break;
    if (i < d - 1)
    {
      const Scalar zi = DistFunc::qNormal(ei + sobol(k, i + offset) * (fi - ei));
      for (UnsignedInteger j = i + 1; j < d; ++j)
      {
        y[j] += L(j, i) * zi;
      }
    }
  }
  return SpecFunc::Clip01(p);
}

} // anonymous namespace

namespace
{
LowDiscrepancySequence GetGenzSequence(const UnsignedInteger dimension)
{
  const String name(ResourceMap::GetAsString("Genz-LowDiscrepancySequence"));
  if (name == "HaltonSequence") return HaltonSequence(dimension);
  if (name == "ReverseHaltonSequence") return ReverseHaltonSequence(dimension);
  if (name == "FaureSequence") return FaureSequence(dimension);
  if (name == "HaselgroveSequence") return HaselgroveSequence(dimension);
  // default: SobolSequence, also handles "SobolSequence", with HaltonSequence as a fallback if the dimension is too large for SobolSequence
  if (dimension <= SobolSequence::MaximumDimension)
    return SobolSequence(dimension, "MULTIDIGIT");
  return HaltonSequence(dimension, "RANDOM");
}
} // anonymous namespace

Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const TriangularMatrix & L,
    const UnsignedInteger n)
{
  const UnsignedInteger d = L.getDimension();

  if (d == 0) throw InvalidDimensionException(HERE) << "Error: the dimension must be at least 1, here d=" << d;
  if (a.getDimension() != d) throw InvalidDimensionException(HERE) << "Error: the lower bound a must have dimension=" << d << ", here dimension=" << a.getDimension();
  if (b.getDimension() != d) throw InvalidDimensionException(HERE) << "Error: the upper bound b must have dimension=" << d << ", here dimension=" << b.getDimension();
  if (mu.getDimension() != d) throw InvalidDimensionException(HERE) << "Error: the mean mu must have dimension=" << d << ", here dimension=" << mu.getDimension();
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (a[i] > b[i]) throw InvalidArgumentException(HERE) << "Error: the lower bound a must be less or equal to the upper bound b, here a[" << i << "]=" << a[i] << " > b[" << i << "]=" << b[i];
  }
  if (n < 1) throw InvalidArgumentException(HERE) << "Error: the sample size n must be positive, here n=" << n;
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (L(i, i) <= 0.0) throw InvalidArgumentException(HERE) << "Error: the diagonal of the Cholesky factor must be strictly positive, here L(" << i << "," << i << ")=" << L(i, i);
  }

  Point a0(d);
  Point b0(d);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    a0[i] = a[i] - mu[i];
    b0[i] = b[i] - mu[i];
  }

  if (d == 1)
  {
    return DistFunc::pNormal(b0[0] / L(0, 0)) -
           DistFunc::pNormal(a0[0] / L(0, 0));
  }

  const LowDiscrepancySequence seq(GetGenzSequence(d - 1));
  const Sample sobolPoints(seq.generate(n));

  struct Reduce
  {
    const Sample & sobolPoints_;
    const Point & a0_;
    const Point & b0_;
    const TriangularMatrix & L_;
    const UnsignedInteger d_;
    Scalar sum_;

    Reduce(const Sample & sp,
           const Point & a0,
           const Point & b0,
           const TriangularMatrix & L,
           const UnsignedInteger d)
      : sobolPoints_(sp)
      , a0_(a0)
      , b0_(b0)
      , L_(L)
      , d_(d)
      , sum_(0.0)
    {
      // Nothing to do
    }

    Reduce(const Reduce & other,
           TBBImplementation::Split)
      : sobolPoints_(other.sobolPoints_)
      , a0_(other.a0_)
      , b0_(other.b0_)
      , L_(other.L_)
      , d_(other.d_)
      , sum_(0.0)
    {
      // Nothing to do
    }

    void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r)
    {
      for (UnsignedInteger k = r.begin(); k != r.end(); ++k)
      {
        sum_ += mvn_genz(a0_, b0_, L_, sobolPoints_, k, 0, d_);
      }
    }

    void join(const Reduce& other) { sum_ += other.sum_; }
  };

  Reduce body(sobolPoints, a0, b0, L, d);
  TBBImplementation::ParallelReduce(0, n, body);
  return SpecFunc::Clip01(body.sum_ / n);
}

Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const CovarianceMatrix & sigma,
    const UnsignedInteger n)
{
  return mvn_rectangular_probability(a, b, mu, sigma.computeCholesky(), n);
}

Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const CovarianceMatrix & sigma,
    const UnsignedInteger n)
{
  const UnsignedInteger d = sigma.getDimension();
  return mvn_rectangular_probability(a, b, Point(d, 0.0), sigma, n);
}

Scalar mvn_rectangular_probability(
    const Point & a,
    const Point & b,
    const TriangularMatrix & L,
    const UnsignedInteger n)
{
  const UnsignedInteger d = L.getDimension();
  return mvn_rectangular_probability(a, b, Point(d, 0.0), L, n);
}

Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const TriangularMatrix & L,
    const Scalar nu,
    const UnsignedInteger n)
{
  const UnsignedInteger d = L.getDimension();

  if (d == 0) throw InvalidDimensionException(HERE) << "Error: the dimension must be at least 1, here d=" << d;
  if (a.getDimension() != d) throw InvalidDimensionException(HERE) << "Error: the lower bound a must have dimension=" << d << ", here dimension=" << a.getDimension();
  if (b.getDimension() != d) throw InvalidDimensionException(HERE) << "Error: the upper bound b must have dimension=" << d << ", here dimension=" << b.getDimension();
  if (mu.getDimension() != d) throw InvalidDimensionException(HERE) << "Error: the mean mu must have dimension=" << d << ", here dimension=" << mu.getDimension();
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (a[i] > b[i]) throw InvalidArgumentException(HERE) << "Error: the lower bound a must be less or equal to the upper bound b, here a[" << i << "]=" << a[i] << " > b[" << i << "]=" << b[i];
  }
  if (n < 1) throw InvalidArgumentException(HERE) << "Error: the sample size n must be positive, here n=" << n;
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: the degrees of freedom nu must be positive, here nu=" << nu;
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (L(i, i) <= 0.0) throw InvalidArgumentException(HERE) << "Error: the diagonal of the Cholesky factor must be strictly positive, here L(" << i << "," << i << ")=" << L(i, i);
  }

  Point a0(d);
  Point b0(d);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    a0[i] = a[i] - mu[i];
    b0[i] = b[i] - mu[i];
  }

  const LowDiscrepancySequence seq(GetGenzSequence(d));
  const Sample sobolPoints(seq.generate(n));

  struct Reduce
  {
    const Sample & sobolPoints_;
    const Point & a0_;
    const Point & b0_;
    const TriangularMatrix & L_;
    const UnsignedInteger d_;
    const Scalar nu_;
    Scalar sum_;

    Reduce(const Sample & sp,
           const Point & a0,
           const Point & b0,
           const TriangularMatrix & L,
           const UnsignedInteger d,
           const Scalar nu)
      : sobolPoints_(sp)
      , a0_(a0)
      , b0_(b0)
      , L_(L)
      , d_(d)
      , nu_(nu)
      , sum_(0.0)
    {
      // Nothing to do
    }

    Reduce(const Reduce & other,
           TBBImplementation::Split)
      : sobolPoints_(other.sobolPoints_)
      , a0_(other.a0_)
      , b0_(other.b0_)
      , L_(other.L_)
      , d_(other.d_)
      , nu_(other.nu_)
      , sum_(0.0)
    {}

    void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r)
    {
      Point a_s(d_);
      Point b_s(d_);
      for (UnsignedInteger k = r.begin(); k != r.end(); ++k)
      {
        const Scalar s = DistFunc::qChi(nu_, sobolPoints_(k, 0)) / std::sqrt(nu_);
        for (UnsignedInteger i = 0; i < d_; ++i)
        {
          a_s[i] = a0_[i] * s;
          b_s[i] = b0_[i] * s;
        }
        sum_ += mvn_genz(a_s, b_s, L_, sobolPoints_, k, 1, d_);
      }
    }

    void join(const Reduce& other) { sum_ += other.sum_; }
  };

  Reduce body(sobolPoints, a0, b0, L, d, nu);
  TBBImplementation::ParallelReduce(0, n, body);
  return SpecFunc::Clip01(body.sum_ / n);
}

Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const Point & mu,
    const CovarianceMatrix & sigma,
    const Scalar nu,
    const UnsignedInteger n)
{
  return mvt_rectangular_probability(a, b, mu, sigma.computeCholesky(), nu, n);
}

Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const CovarianceMatrix & sigma,
    const Scalar nu,
    const UnsignedInteger n)
{
  const UnsignedInteger d = sigma.getDimension();
  return mvt_rectangular_probability(a, b, Point(d, 0.0), sigma, nu, n);
}

Scalar mvt_rectangular_probability(
    const Point & a,
    const Point & b,
    const TriangularMatrix & L,
    const Scalar nu,
    const UnsignedInteger n)
{
  const UnsignedInteger d = L.getDimension();
  return mvt_rectangular_probability(a, b, Point(d, 0.0), L, nu, n);
}

END_NAMESPACE_OPENTURNS
