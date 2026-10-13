//                                               -*- C++ -*-
/**
 *  @brief Standardization (mu-sigma) scaling
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MuSigmaScaling.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MuSigmaScaling)

static const Factory<MuSigmaScaling> Factory_MuSigmaScaling;

/* Default constructor */
MuSigmaScaling::MuSigmaScaling()
  : PersistentObject()
  , mu_(1, 0.0)
  , sigma_(1, 1.0)
{
  // Nothing to do
}

/* Parameter constructor */
MuSigmaScaling::MuSigmaScaling(const Point & mu,
                               const Point & sigma)
  : PersistentObject()
  , mu_(mu)
  , sigma_(sigma)
{
  if (mu.getDimension() != sigma.getDimension()) throw InvalidArgumentException(HERE) << "Error: mu dimension (" << mu.getDimension() << ") does not match sigma dimension (" << sigma.getDimension() << ")";
  if (!(mu.getDimension() > 0)) throw InvalidArgumentException(HERE) << "Error: mu and sigma must be non-empty";
  for (UnsignedInteger i = 0; i < sigma.getDimension(); ++i)
    if (!(sigma[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: sigma must have positive components, got sigma[" << i << "]=" << sigma[i];
  for (UnsignedInteger i = 0; i < sigma.getDimension(); ++i)
  {
    if (!std::isfinite(mu[i])) throw InvalidArgumentException(HERE) << "Error: mu must have finite components, got mu[" << i << "]=" << mu[i];
    if (!std::isfinite(sigma[i]) || !std::isfinite(1.0 / sigma[i])) throw InvalidArgumentException(HERE) << "Error: sigma must be finite with a finite reciprocal, got sigma[" << i << "]=" << sigma[i];
  }
}

/* Virtual constructor */
MuSigmaScaling * MuSigmaScaling::clone() const
{
  return new MuSigmaScaling(*this);
}

/* Mu accessor */
Point MuSigmaScaling::getMu() const
{
  return mu_;
}

/* Sigma accessor */
Point MuSigmaScaling::getSigma() const
{
  return sigma_;
}

/* Forward scaling accessor */
Function MuSigmaScaling::getTransform() const
{
  const UnsignedInteger dimension = mu_.getDimension();
  Matrix linear(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear(i, i) = 1.0 / sigma_[i];
  return LinearFunction(mu_, Point(dimension, 0.0), linear);
}

/* Inverse scaling accessor */
Function MuSigmaScaling::getInverseTransform() const
{
  const UnsignedInteger dimension = mu_.getDimension();
  Matrix linear(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear(i, i) = sigma_[i];
  return LinearFunction(Point(dimension, 0.0), mu_, linear);
}

/* String converter */
String MuSigmaScaling::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MuSigmaScaling::GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " sigma=" << sigma_;
  return oss;
}

/* String converter */
String MuSigmaScaling::__str__(const String & ) const
{
  OSS oss(false);
  oss << "MuSigmaScaling(mu=" << mu_.__str__()
      << ", sigma=" << sigma_.__str__() << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MuSigmaScaling::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("mu_", mu_);
  adv.saveAttribute("sigma_", sigma_);
}

/* Method load() reloads the object from the StorageManager */
void MuSigmaScaling::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("mu_", mu_);
  adv.loadAttribute("sigma_", sigma_);
}

END_NAMESPACE_OPENTURNS
