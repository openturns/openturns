//                                               -*- C++ -*-
/**
 *  @brief Gumbel distribution with mu and sigma as parameters
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

#include "openturns/GumbelFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/GumbelMuSigma.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelMuSigma)

/* Default constructor */
GumbelMuSigma::GumbelMuSigma()
  : DistributionParametersImplementation()
  , mu_(SpecFunc::EulerConstant)
  , sigma_(SpecFunc::PI_SQRT6)
{
  // Nothing to do
}

GumbelMuSigma::GumbelMuSigma(const Scalar mu, const Scalar sigma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "Sigma must be > 0, here sigma=" << sigma;
}

/* Virtual constructor */
GumbelMuSigma * GumbelMuSigma::clone() const
{
  return new GumbelMuSigma(*this);
}

/* Comparison operator */
Bool GumbelMuSigma::operator ==(const GumbelMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution GumbelMuSigma::getDistribution() const
{
  Point newParameters(2);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;

  Point nativeParameters(operator()(newParameters));

  return GumbelFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix GumbelMuSigma::gradient() const
{
  const Scalar dalphadmu = 0.0;
  const Scalar dalphadsigma = -SpecFunc::PI_SQRT6 / (sigma_ * sigma_);
  const Scalar dbetadmu = 1.0;
  const Scalar dbetadsigma = -SpecFunc::EULERSQRT6_PI;

  SquareMatrix nativeParametersGradient(IdentityMatrix(2));
  nativeParametersGradient(0, 0) = dalphadmu;
  nativeParametersGradient(1, 0) = dalphadsigma;

  nativeParametersGradient(0, 1) = dbetadmu;
  nativeParametersGradient(1, 1) = dbetadsigma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point GumbelMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  const Scalar alpha = SpecFunc::PI_SQRT6 / sigma;
  const Scalar beta = mu - SpecFunc::EULERSQRT6_PI * sigma;

  Point nativeParameters(2);
  nativeParameters[0] = alpha;
  nativeParameters[1] = beta;

  return nativeParameters;
}


Point GumbelMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar alpha = inP[0];
  const Scalar beta = inP[1];

  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";

  const Scalar mu = beta + SpecFunc::EulerConstant / alpha;
  const Scalar sigma = SpecFunc::PI_SQRT6 / alpha;

  Point muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void GumbelMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
}

Point GumbelMuSigma::getValues() const
{
  Point point(2);
  point[0] = mu_;
  point[1] = sigma_;
  return point;
}

Description GumbelMuSigma::getDescription() const
{
  Description description(2);
  description[0] = "mu";
  description[1] = "sigma";
  return description;
}

/* String converter */
String GumbelMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_;
  return oss;
}


String GumbelMuSigma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS
