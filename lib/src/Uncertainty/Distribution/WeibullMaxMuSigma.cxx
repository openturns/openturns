//                                               -*- C++ -*-
/**
 *  @brief WeibullMin distribution with mu and sigma as parameters
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/WeibullMaxFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/WeibullMaxMuSigma.hxx"
#include "openturns/WeibullMinMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeibullMaxMuSigma)
static const Factory<WeibullMaxMuSigma> Factory_WeibullMaxMuSigma;

/* Default constructor */
WeibullMaxMuSigma::WeibullMaxMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
  , gamma_(0.)
{
  // Nothing to do
}

WeibullMaxMuSigma::WeibullMaxMuSigma(const Scalar mu, const Scalar sigma, const Scalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
  if (!(mu < gamma)) throw InvalidArgumentException(HERE) << "mu must be lesser than gamma, here mu=" << mu << " and gamma=" << gamma;
}

/* Virtual constructor */
WeibullMaxMuSigma * WeibullMaxMuSigma::clone() const
{
  return new WeibullMaxMuSigma(*this);
}

/* Comparison operator */
Bool WeibullMaxMuSigma::operator ==(const WeibullMaxMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution WeibullMaxMuSigma::getDistribution() const
{
  Point newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  Point nativeParameters(operator()(newParameters));

  return WeibullMaxFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix WeibullMaxMuSigma::gradient() const
{
  Point newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  Matrix nativeParametersGradient(WeibullMinMuSigma(-mu_, sigma_, -gamma_).gradient());
  nativeParametersGradient(0, 0) *= -1.0;//dbeta/dmu
  nativeParametersGradient(0, 1) *= -1.0;//dalpha/dmu
  nativeParametersGradient(2, 0) *= -1.0;//dbeta/dgamma
  nativeParametersGradient(2, 1) *= -1.0;//dalpha/dgamma
  return nativeParametersGradient;
}


/* Conversion operator */
Point WeibullMaxMuSigma::operator () (const Point & inP) const
{
  Point inP2(inP);
  inP2[0] *= -1;//mu
  inP2[2] *= -1;// gamma
  Point nativeParameters(WeibullMinMuSigma().operator()(inP2));
  nativeParameters[2] *= -1;// gamma
  return nativeParameters;
}


Point WeibullMaxMuSigma::inverse(const Point & inP) const
{
  Point inP2(inP);
  inP2[2] *= -1;// gamma
  Point muSigmaParameters(WeibullMinMuSigma().inverse(inP2));
  muSigmaParameters[0] *= -1;// mu
  muSigmaParameters[2] *= -1;// gamma
  return muSigmaParameters;
}


/* Parameters value and description accessor */
void WeibullMaxMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
  gamma_ = inP[2];
}

Point WeibullMaxMuSigma::getValues() const
{
  Point point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  return point;
}

Description WeibullMaxMuSigma::getDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  return description;
}

/* String converter */
String WeibullMaxMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " gamma=" << gamma_;
  return oss;
}


String WeibullMaxMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
  return oss;
}

void WeibullMaxMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

void WeibullMaxMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "gamma_", gamma_ );
}

END_NAMESPACE_OPENTURNS
