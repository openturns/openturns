//                                               -*- C++ -*-
/**
 *  @brief Beta distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cmath>
#include "openturns/BetaFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/BetaMuSigma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BetaMuSigma);

/* Default constructor */
BetaMuSigma::BetaMuSigma()
  : DistributionParametersImplementation()
  , mu_(0.5)
  , sigma_(1 / (2 * sqrt(5.0)))
  , a_(0.)
  , b_(1.)
{
  // Nothing to do
}

BetaMuSigma::BetaMuSigma(const Scalar mu, const Scalar sigma, const Scalar a, const Scalar b)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , a_(a)
  , b_(b)
{
  // Nothing to do
}

/* Virtual constructor */
BetaMuSigma * BetaMuSigma::clone() const
{
  return new BetaMuSigma(*this);
}

/* Comparison operator */
Bool BetaMuSigma::operator ==(const BetaMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution BetaMuSigma::getDistribution() const
{
  Point newParameters(4);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = a_;
  newParameters[3] = b_;

  Point nativeParameters(operator()(newParameters));

  return BetaFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix BetaMuSigma::gradient() const
{
  Point newParameters(4);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = a_;
  newParameters[3] = b_;

  const Scalar t = operator()(newParameters)[1];

  const Scalar mu = mu_;
  const Scalar sigma = sigma_;
  const Scalar a = a_;
  const Scalar b = b_;

  const Scalar dtdmu = (a + b - 2.0 * mu) / (sigma * sigma);
  const Scalar dtdsigma = 2.0 * (mu - a) * (mu - b) / (sigma * sigma * sigma);
  const Scalar dtda = (mu - b) / (sigma * sigma);
  const Scalar dtdb = (mu - a) / (sigma * sigma);
  const Scalar drdmu = (t + dtdmu * (mu - a)) / (b - a);
  const Scalar drdsigma = dtdsigma * (mu - a) / (b - a);
  const Scalar drda = (a * a - 2.0 * a * b - sigma * sigma - mu * (mu - 2.0 * b)) * (mu - b) / ((b - a) * (b - a) * sigma * sigma);
  const Scalar drdb = (sigma * sigma + (mu - a) * (mu - a)) * (mu - a) / ((b - a) * (b - a) * sigma * sigma);

  SquareMatrix nativeParametersGradient(IdentityMatrix(4));
  nativeParametersGradient(0, 0) = drdmu;
  nativeParametersGradient(1, 0) = drdsigma;
  nativeParametersGradient(2, 0) = drda;
  nativeParametersGradient(3, 0) = drdb;

  nativeParametersGradient(0, 1) = dtdmu;
  nativeParametersGradient(1, 1) = dtdsigma;
  nativeParametersGradient(2, 1) = dtda;
  nativeParametersGradient(3, 1) = dtdb;

  return nativeParametersGradient;
}


/* Conversion operator */
Point BetaMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 4) throw InvalidArgumentException(HERE) << "the given point must have dimension=4, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];
  const Scalar a = inP[2];
  const Scalar b = inP[3];

  const Scalar t = (b - mu) * (mu - a) / (sigma * sigma) - 1.0;
  const Scalar r = t * (mu - a) / (b - a);

  Point nativeParameters(inP);
  nativeParameters[0] = r;
  nativeParameters[1] = t;

  return nativeParameters;
}


Point BetaMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 4) throw InvalidArgumentException(HERE) << "the given point must have dimension=4, here dimension=" << inP.getDimension();
  const Scalar r = inP[0];
  const Scalar t = inP[1];
  const Scalar a = inP[2];
  const Scalar b = inP[3];

  if (!(r > 0.0)) throw InvalidArgumentException(HERE) << "R MUST be positive";
  if (!(t > 0.0)) throw InvalidArgumentException(HERE) << "T MUST be positive";
  if (t <= r) throw InvalidArgumentException(HERE) << "T MUST be greater than r, here t=" << t << " and r=" << r;

  const Scalar mu = a + (b - a) * r / t;
  const Scalar sigma = (b - a) / t * std::sqrt(r * (t - r) / (t + 1.0));

  Point muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}

/* Parameters value and description accessor */
void BetaMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 4) throw InvalidArgumentException(HERE) << "the given point must have dimension=4, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
  a_ = inP[2];
  b_ = inP[3];

}


Point BetaMuSigma::getValues() const
{
  Point point(4);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = a_;
  point[3] = b_;
  return point;
}

Description BetaMuSigma::getDescription() const
{
  Description description(4);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "a";
  description[3] = "b";
  return description;
}

/* Check if the distribution is elliptical */
Bool BetaMuSigma::isElliptical() const
{
  return b_ == 2.0 * mu_ - a_;
}


/* String converter */
String BetaMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " a=" << a_
      << " b=" << b_;
  return oss;
}


String BetaMuSigma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS
