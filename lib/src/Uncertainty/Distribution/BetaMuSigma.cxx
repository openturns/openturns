//                                               -*- C++ -*-
/**
 *  @brief Beta distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BetaFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/BetaMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BetaMuSigma)
static const Factory<BetaMuSigma> Factory_BetaMuSigma;

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

  const Scalar mu = mu_;
  const Scalar sigma = sigma_;
  const Scalar a = a_;
  const Scalar b = b_;

  const Scalar alphadmu = (sigma * sigma + (a - mu) * (b - mu) + (a - mu) * (a + b - 2 * mu)) / (sigma * sigma * (a - b));
  const Scalar dalphadsigma = 2 * (a - mu) * (a - mu) * (b - mu) / (sigma * sigma * sigma * (a - b));
  const Scalar dalphada = (-(a - b) * (sigma * sigma + 2 * (a - mu) * (b - mu)) + (a - mu) * (sigma * sigma + (a - mu) * (b - mu))) / (sigma * sigma * (a - b) * (a - b));
  const Scalar dalphadb = -(a - mu) * (sigma * sigma + (a - b) * (a - mu) + (a - mu) * (b - mu)) / (sigma * sigma * (a - b) * (a - b));
  const Scalar dbetadmu = -(sigma * sigma + (a - mu) * (b - mu) + (b - mu) * (a + b - 2 * mu)) / (sigma * sigma * (a - b));
  const Scalar dbetadsigma = 2 * (-a + mu) * (b - mu) * (b - mu) / (sigma * sigma * sigma * (a - b));
  const Scalar dbetada = (b - mu) * (-sigma * sigma + (a - b) * (b - mu) - (a - mu) * (b - mu)) / (sigma * sigma * (a - b) * (a - b));
  const Scalar dbetadb = ((a - b) * (sigma * sigma + 2 * (a - mu) * (b - mu)) + (b - mu) * (sigma * sigma + (a - mu) * (b - mu))) / (sigma * sigma * (a - b) * (a - b));

  Matrix nativeParametersGradient(IdentityMatrix(4));
  nativeParametersGradient(0, 0) = alphadmu;
  nativeParametersGradient(1, 0) = dalphadsigma;
  nativeParametersGradient(2, 0) = dalphada;
  nativeParametersGradient(3, 0) = dalphadb;

  nativeParametersGradient(0, 1) = dbetadmu;
  nativeParametersGradient(1, 1) = dbetadsigma;
  nativeParametersGradient(2, 1) = dbetada;
  nativeParametersGradient(3, 1) = dbetadb;

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

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "Sigma MUST be positive";

  const Scalar alpha = ((mu - a) / (b - a)) * (((b - mu) * (mu - a)) / (sigma * sigma) - 1.0);
  const Scalar beta = (((b - mu) * (mu - a)) / (sigma * sigma) - 1.0) * ((b - mu) / (b - a));

  Point nativeParameters(inP);
  nativeParameters[0] = alpha;
  nativeParameters[1] = beta;

  return nativeParameters;
}


Point BetaMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 4) throw InvalidArgumentException(HERE) << "the given point must have dimension=4, here dimension=" << inP.getDimension();
  const Scalar alpha = inP[0];
  const Scalar beta = inP[1];
  const Scalar a = inP[2];
  const Scalar b = inP[3];

  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";

  const Scalar mu = a + (b - a) * alpha / (alpha + beta);
  const Scalar sigma = (b - a) / (alpha + beta) * std::sqrt(alpha * beta / (alpha + beta + 1.0));

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


String BetaMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

void BetaMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
}

void BetaMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
}

END_NAMESPACE_OPENTURNS
