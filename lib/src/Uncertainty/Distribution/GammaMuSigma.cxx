//                                               -*- C++ -*-
/**
 *  @brief Gamma distribution with mu and sigma as parameters
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

#include "GammaFactory.hxx"
#include "SquareMatrix.hxx"
#include "GammaMuSigma.hxx"
#include "PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GammaMuSigma);

static const Factory<GammaMuSigma> RegisteredFactory;

/* Default constructor */
GammaMuSigma::GammaMuSigma()
  : DistributionParametersImplementation()
  , mu_(1.0)
  , sigma_(1.0)
  , gamma_(0.)
{
  // Nothing to do
}

GammaMuSigma::GammaMuSigma(const NumericalScalar mu, const NumericalScalar sigma, const NumericalScalar gamma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
  , gamma_(gamma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
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
  NumericalPoint newParameters(3);
  newParameters[0] = mu_;
  newParameters[1] = sigma_;
  newParameters[2] = gamma_;

  NumericalPointCollection nativeParameters(1, operator()(newParameters));

  return GammaFactory().build(nativeParameters);
}


/* Compute jacobian / native parameters */
Matrix GammaMuSigma::gradient() const
{
  const NumericalScalar mu = mu_;
  const NumericalScalar sigma = sigma_;
  const NumericalScalar gamma = gamma_;

  const NumericalScalar dkdmu = 2.0 * (mu - gamma) / (sigma * sigma);
  const NumericalScalar dkdsigma = -2.0 * (mu - gamma) * (mu - gamma) / (sigma * sigma * sigma);
  const NumericalScalar dkdgamma = -dkdmu;
  const NumericalScalar dlambdadmu = 1.0 / (sigma * sigma);
  const NumericalScalar dlambdadsigma = -2.0 * (mu - gamma) / (sigma * sigma * sigma);
  const NumericalScalar dlambdadgamma = -dlambdadmu;

  SquareMatrix nativeParametersGradient(IdentityMatrix(3));
  nativeParametersGradient(0, 0) = dkdmu;
  nativeParametersGradient(1, 0) = dkdsigma;
  nativeParametersGradient(2, 0) = dkdgamma;

  nativeParametersGradient(0, 1) = dlambdadmu;
  nativeParametersGradient(1, 1) = dlambdadsigma;
  nativeParametersGradient(2, 1) = dlambdadgamma;

  return nativeParametersGradient;
}


/* Conversion operator */
NumericalPoint GammaMuSigma::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar mu = inP[0];
  const NumericalScalar sigma = inP[1];
  const NumericalScalar gamma = inP[2];

  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  NumericalScalar lambda = (mu - gamma) / sigma;
  const NumericalScalar k = lambda * lambda;
  lambda /= sigma;

  NumericalPoint nativeParameters(inP);
  nativeParameters[0] = k;
  nativeParameters[1] = lambda;

  return nativeParameters;
}


NumericalPoint GammaMuSigma::inverse(const NumericalPoint & inP) const
{
  if (inP.getDimension() != 3) throw InvalidArgumentException(HERE) << "the given point must have dimension=3, here dimension=" << inP.getDimension();
  const NumericalScalar k = inP[0];
  const NumericalScalar lambda = inP[1];
  const NumericalScalar gamma = inP[2];

  if (k <= 0.0) throw InvalidArgumentException(HERE) << "K MUST be positive";
  if (lambda <= 0.0) throw InvalidArgumentException(HERE) << "Lambda MUST be positive";

  NumericalScalar mu = gamma + k / lambda;
  const NumericalScalar sigma = std::sqrt(k) / lambda;

  NumericalPoint muSigmaParameters(inP);
  muSigmaParameters[0] = mu;
  muSigmaParameters[1] = sigma;

  return muSigmaParameters;
}


/* Parameters value and description accessor */
NumericalPointWithDescription GammaMuSigma::getParameters() const
{
  NumericalPointWithDescription point(3);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = gamma_;
  Description description(point.getDimension());
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "gamma";
  point.setDescription(description);
  return point;
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


String GammaMuSigma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", gamma = " << gamma_ << ")";
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