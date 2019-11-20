//                                               -*- C++ -*-
/**
 *  @brief Gamma distribution with mu and sigma as parameters
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

#include "openturns/GammaFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/GammaMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GammaMuSigma)

static const Factory<GammaMuSigma> Factory_GammaMuSigma;

/* Default constructor */
GammaMuSigma::GammaMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
  , gamma_(0.)
{
  // Nothing to do
}

GammaMuSigma::GammaMuSigma(const Scalar mu, const Scalar sigma, const Scalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
}

/* Virtual constructor */
GammaMuSigma * GammaMuSigma::clone() const
{
  return new GammaMuSigma(*this);
}

/* Comparison operator */
Bool GammaMuSigma::operator ==(const GammaMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution GammaMuSigma::getDistribution() const
{
  Point newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  Point nativeParameters(operator()(newParameters));

  return GammaFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix GammaMuSigma::gradient() const
{
  const Scalar mu = mu_;
  const Scalar sigma = sigma_;
  const Scalar gamma = gamma_;

  const Scalar dkdmu = 2.0 * (mu - gamma) / (sigma * sigma);
  const Scalar dkdsigma = -2.0 * (mu - gamma) * (mu - gamma) / (sigma * sigma * sigma);
  const Scalar dkdgamma = -dkdmu;
  const Scalar dlambdadmu = 1.0 / (sigma * sigma);
  const Scalar dlambdadsigma = -2.0 * (mu - gamma) / (sigma * sigma * sigma);
  const Scalar dlambdadgamma = -dlambdadmu;

  Matrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(0, 0) = dkdmu;
  nativeParametersGradient(1, 0) = dkdsigma;
  nativeParametersGradient(2, 0) = dkdgamma;

  nativeParametersGradient(0, 1) = dlambdadmu;
  nativeParametersGradient(1, 1) = dlambdadsigma;
  nativeParametersGradient(2, 1) = dlambdadgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point GammaMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];
  const Scalar gamma = inP[2];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  Scalar lambda = (mu - gamma) / sigma;
  const Scalar k = lambda * lambda;
  lambda /= sigma;

  Point nativeParameters(inP);
  nativeParameters[0] = k;
  nativeParameters[1] = lambda;

  return nativeParameters;
}


Point GammaMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const Scalar k = inP[0];
  const Scalar lambda = inP[1];
  const Scalar gamma = inP[2];

  if (!(k > 0.0)) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";

  Scalar mu = gamma + k / lambda;
  const Scalar sigma = std::sqrt(k) / lambda;

  Point muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
void GammaMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
  gamma_ = inP[2];
}

Point GammaMuSigma::getValues() const
{
  Point point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  return point;
}

Description GammaMuSigma::getDescription() const
{
  Description description(3);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  return description;
}

/* String converter */
String GammaMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " gamma=" << gamma_;
  return oss;
}


String GammaMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
  return oss;
}


/* Method save() stores the object through the StorageManager */
void GammaMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute("mu_", mu_);
  adv.saveAttribute("sigma_", sigma_);
  adv.saveAttribute("gamma_", gamma_);
}

/* Method load() reloads the object from the StorageManager */
void GammaMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute("mu_", mu_);
  adv.loadAttribute("sigma_", sigma_);
  adv.loadAttribute("gamma_", gamma_);
}


END_NAMESPACE_OPENTURNS
