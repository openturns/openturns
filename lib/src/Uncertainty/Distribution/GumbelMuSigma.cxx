//                                               -*- C++ -*-
/**
 *  @brief Gumbel distribution with mu and sigma as parameters
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

#include "openturns/GumbelFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/GumbelMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelMuSigma)
static const Factory<GumbelMuSigma> Factory_GumbelMuSigma;

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
  const Scalar dbetadmu = 0.0;
  const Scalar dbetadsigma = 1.0 / SpecFunc::PI_SQRT6;
  const Scalar dgammadmu = 1.0;
  const Scalar dgammadsigma = -SpecFunc::EULERSQRT6_PI;

  Matrix nativeParametersGradient(IdentityMatrix(2));
  nativeParametersGradient(0, 0) = dbetadmu;
  nativeParametersGradient(1, 0) = dbetadsigma;

  nativeParametersGradient(0, 1) = dgammadmu;
  nativeParametersGradient(1, 1) = dgammadsigma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point GumbelMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  const Scalar beta = sigma / SpecFunc::PI_SQRT6;
  const Scalar gamma = mu - SpecFunc::EULERSQRT6_PI * sigma;

  Point nativeParameters(2);
  nativeParameters[0] = beta;
  nativeParameters[1] = gamma;

  return nativeParameters;
}


Point GumbelMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar beta = inP[0];
  const Scalar gamma = inP[1];

  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";

  const Scalar mu = gamma + beta * SpecFunc::EulerConstant;
  const Scalar sigma = beta * SpecFunc::PI_SQRT6;

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


String GumbelMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ")";
  return oss;
}

void GumbelMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
}

void GumbelMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
}

END_NAMESPACE_OPENTURNS
