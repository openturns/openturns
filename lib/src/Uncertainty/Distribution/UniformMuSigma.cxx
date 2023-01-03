//                                               -*- C++ -*-
/**
 *  @brief Uniform distribution with mu and sigma as parameters
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
#include "openturns/UniformFactory.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/UniformMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniformMuSigma)

static const Factory<UniformMuSigma> Factory_UniformMuSigma;
/* Default constructor */
UniformMuSigma::UniformMuSigma()
  : DistributionParametersImplementation()
  , mu_(0.0)
  , sigma_(2.0 / std::sqrt(12.0))
{
  // Nothing to do
}

UniformMuSigma::UniformMuSigma(const Scalar mu, const Scalar sigma)
  : DistributionParametersImplementation()
  , mu_(mu)
  , sigma_(sigma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;
}

/* Virtual constructor */
UniformMuSigma * UniformMuSigma::clone() const
{
  return new UniformMuSigma(*this);
}

/* Comparison operator */
Bool UniformMuSigma::operator ==(const UniformMuSigma & other) const
{
  return (this == &other);
}


/* Build a distribution based on a set of native parameters */
Distribution UniformMuSigma::getDistribution() const
{
  return UniformFactory().build(operator()({mu_, sigma_}));
}


/* Compute jacobian / native parameters */
Matrix UniformMuSigma::gradient() const
{
  const Scalar dadmu = 1.0;
  const Scalar dadsigma = -0.5 * std::sqrt(12.0);
  const Scalar dbdmu = 1.0;
  const Scalar dbdsigma = -dadsigma;

  Matrix nativeParametersGradient(2, 2);
  nativeParametersGradient(0, 0) = dadmu;
  nativeParametersGradient(1, 0) = dadsigma;

  nativeParametersGradient(0, 1) = dbdmu;
  nativeParametersGradient(1, 1) = dbdsigma;

  return nativeParametersGradient;
}


/* Conversion operator */
Point UniformMuSigma::operator () (const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar mu = inP[0];
  const Scalar sigma = inP[1];

  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "sigma must be > 0, here sigma=" << sigma;

  const Scalar a = mu - sigma * 0.5 * std::sqrt(12.0);
  const Scalar b = mu + sigma * 0.5 * std::sqrt(12.0);

  return {a, b};
}


Point UniformMuSigma::inverse(const Point & inP) const
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  const Scalar a = inP[0];
  const Scalar b = inP[1];

  if (a >= b) throw InvalidArgumentException(HERE) << "a must be smaller than b";

  const Scalar mu = 0.5 * (a + b);
  const Scalar sigma = (b - a) / std::sqrt(12.0);

  return {mu, sigma};
}


/* Parameters value and description accessor */
void UniformMuSigma::setValues(const Point & inP)
{
  if (inP.getDimension() != 2) throw InvalidArgumentException(HERE) << "the given point must have dimension=2, here dimension=" << inP.getDimension();
  mu_ = inP[0];
  sigma_ = inP[1];
}

Point UniformMuSigma::getValues() const
{
  return {mu_, sigma_};
}

Description UniformMuSigma::getDescription() const
{
  return {"mu", "sigma"};
}

/* String converter */
String UniformMuSigma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_;
  return oss;
}


String UniformMuSigma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ")";
  return oss;
}

void UniformMuSigma::save(Advocate & adv) const
{
  DistributionParametersImplementation::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
}

/* Method load() reloads the object from the StorageManager */
void UniformMuSigma::load(Advocate & adv)
{
  DistributionParametersImplementation::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
}

END_NAMESPACE_OPENTURNS
