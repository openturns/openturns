//                                               -*- C++ -*-
/**
 *  @brief Beta distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "BetaFactory.hxx"
#include "SquareMatrix.hxx"
#include "BetaMuSigma.hxx"

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

BetaMuSigma::BetaMuSigma(const NumericalScalar mu, const NumericalScalar sigma, const NumericalScalar a, const NumericalScalar b)
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
  NumericalPoint newParameters(4);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = a_;
  newParameters[3] = b_;

  NumericalPointCollection nativeParameters(1, operator()(newParameters));

  return BetaFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix BetaMuSigma::gradient() const
{
  NumericalPoint newParameters(4);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = a_;
  newParameters[3] = b_;

  const NumericalScalar t = operator()(newParameters)[1];

  const NumericalScalar mu = mu_;
  const NumericalScalar sigma = sigma_;
  const NumericalScalar a = a_;
  const NumericalScalar b = b_;

  const NumericalScalar dtdmu = (a + b - 2.0 * mu) / (sigma * sigma);
  const NumericalScalar dtdsigma = 2.0 * (mu - a) * (mu - b) / (sigma * sigma * sigma);
  const NumericalScalar dtda = (mu - b) / (sigma * sigma);
  const NumericalScalar dtdb = (mu - a) / (sigma * sigma);
  const NumericalScalar drdmu = (t + dtdmu * (mu - a)) / (b - a);
  const NumericalScalar drdsigma = dtdsigma * (mu - a) / (b - a);
  const NumericalScalar drda = (a * a - 2.0 * a * b - sigma * sigma - mu * (mu - 2.0 * b)) * (mu - b) / ((b - a) * (b - a) * sigma * sigma);
  const NumericalScalar drdb = (sigma * sigma + (mu - a) * (mu - a)) * (mu - a) / ((b - a) * (b - a) * sigma * sigma);

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
NumericalPoint BetaMuSigma::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 4) throw InvalidArgumentException(HERE) << "the given point must have dimension=4, here dimension=" << inP.getDimension();
  const NumericalScalar mu = inP[0];
  const NumericalScalar sigma = inP[1];
  const NumericalScalar a = inP[2];
  const NumericalScalar b = inP[3];

  const NumericalScalar t = (b - mu) * (mu - a) / (sigma * sigma) - 1.0;
  const NumericalScalar r = t * (mu - a) / (b - a);

  NumericalPoint nativeParameters(inP);
  nativeParameters[0] = r;
  nativeParameters[1] = t;

  return nativeParameters;
}


/* Parameters value and description accessor */
BetaMuSigma::NumericalPointWithDescriptionCollection BetaMuSigma::getParametersCollection() const
{
  NumericalPointWithDescription point(4);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = a_;
  point[3] = b_;
  Description description(point.getDimension());
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "a";
  description[3] = "b";
  point.setDescription(description);

  return NumericalPointWithDescriptionCollection(1, point);
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