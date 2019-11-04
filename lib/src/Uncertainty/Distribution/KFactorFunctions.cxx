//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the k factor
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
#include "openturns/KFactorFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/NonCentralChiSquare.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace KFactorFunctions
{

#define KFactorFunctions_RMAX 8.5

class KernelFunction: public EvaluationImplementation
{
public:
  KernelFunction(const Scalar nu,
                 const Scalar p,
                 const Scalar n)
    : EvaluationImplementation()
    , nu_(nu)
    , p_(p)
    , x_(1.0)
    , n_(n)
    , chiSquare_(nu)
    , normal_(0.0, 1.0 / std::sqrt(n))
  {
    // Nothing to do
  }

  KernelFunction * clone() const
  {
    return new KernelFunction(*this);
  }

  Point operator() (const Point & point) const
  {
    const Scalar z = point[0];
    return Point(1, chiSquare_.computeComplementaryCDF(nu_ * NonCentralChiSquare(1.0, z * z).computeQuantile(p_) / (x_ * x_)) * normal_.computePDF(z));
  }

  void setX(const Scalar x)
  {
    x_ = x;
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  String __repr__() const
  {
    OSS oss(true);
    oss << "class=KernelFunction"
        << " nu=" << nu_
        << " p=" << p_
        << " x=" << x_
        << " n=" << n_
        << " chiSquare=" << chiSquare_
        << " normal=" << normal_;
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss(false);
    oss << "KernelFunction("
        << "nu=" << nu_
        << ", p=" << p_
        << ", x=" << n_
        << ", n=" << n_
        << ", chiSquare=" << chiSquare_
        << ", normal=" << normal_
        << ")";
    return oss;
  }

private:
  Scalar nu_;
  Scalar p_;
  Scalar x_;
  Scalar n_;
  ChiSquare chiSquare_;
  Normal normal_;
}; // KernelFunction

class ConstraintFunction: public EvaluationImplementation
{
public:
  ConstraintFunction(const Scalar nu,
                     const Scalar p,
                     const Scalar n)
    : EvaluationImplementation()
    , nu_(nu)
    , p_(p)
    , n_(n)
    , kernel_(nu, p, n)
  {
    // Nothing to do
  }

  ConstraintFunction * clone() const
  {
    return new ConstraintFunction(*this);
  }

  Point operator() (const Point & point) const
  {
    const Scalar x = point[0];
    kernel_.setX(x);
    return GaussKronrod(ResourceMap::GetAsUnsignedInteger("KFactor-DefaultIntegrationNodesNumber"), ResourceMap::GetAsScalar("KFactor-Precision"), GaussKronrodRule(GaussKronrodRule::G7K15)).integrate(kernel_, Interval(0.0, KFactorFunctions_RMAX / std::sqrt(n_)));
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  String __repr__() const
  {
    OSS oss(true);
    oss << "class=ConstraintFunction"
        << " nu=" << nu_
        << " p=" << p_
        << " n=" << n_
        << " kernel=" << kernel_;
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss(false);
    oss << "ConstraintFunction("
        << "nu=" << nu_
        << ", p=" << p_
        << ", n=" << n_
        << ", kernel=" << kernel_
        << ")";
    return oss;
  }

private:
  Scalar nu_;
  Scalar p_;
  Scalar n_;
  mutable KernelFunction kernel_;
}; // ConstraintFunction

Scalar KFactor(const Scalar n,
               const Scalar nu,
               const Scalar p,
               const Scalar alpha)
{
  if (!(n >= 1.0)) throw InvalidArgumentException(HERE) << "Error: n must be greater than 1, here n=" << n;
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive, here nu=" << nu;
  if (!((p >= 0.0) && (p <= 1.0))) throw InvalidArgumentException(HERE) << "Error: p must be in [0, 1], here p=" << p;
  if (!((alpha >= 0.0) && (alpha <= 1.0))) throw InvalidArgumentException(HERE) << "Error: alpha must be in [0, 1], here alpha=" << alpha;
  const ConstraintFunction constraint(nu, p, n);
  const Brent solver(ResourceMap::GetAsScalar("KFactor-Precision"), ResourceMap::GetAsScalar("KFactor-Precision"), ResourceMap::GetAsScalar("KFactor-Precision"), ResourceMap::GetAsUnsignedInteger("KFactor-MaximumIteration"));
  return solver.solve(constraint, 0.5 * (1.0 - alpha), SpecFunc::ScalarEpsilon, KFactorFunctions_RMAX);
}

} // KFactorFunctions

END_NAMESPACE_OPENTURNS
