//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "ExponentialCauchy.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"
#include "AbsoluteExponential.hxx"
#include "CauchyModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExponentialCauchy
 */

CLASSNAMEINIT(ExponentialCauchy);

static const Factory<ExponentialCauchy> RegisteredFactory;

/* Constructor with parameters */
ExponentialCauchy::ExponentialCauchy()
  : SecondOrderModelImplementation(AbsoluteExponential(), CauchyModel())
{
  // Nothing to do
}

ExponentialCauchy::ExponentialCauchy(const NumericalPoint & amplitude,
                                     const NumericalPoint & scale)
  : SecondOrderModelImplementation(AbsoluteExponential(scale, amplitude), CauchyModel(amplitude, scale))
{
  // Nothing to do
}

/* Virtual constructor */
ExponentialCauchy * ExponentialCauchy::clone() const
{
  return new ExponentialCauchy(*this);
}

/* String converter */
String ExponentialCauchy::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ExponentialCauchy::GetClassName()
      << " derived from " << SecondOrderModelImplementation::__repr__();
  return oss;
}

/* String converter */
String ExponentialCauchy::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << ExponentialCauchy::GetClassName();
  oss << " amplitude=" << getAmplitude()
      << " scale=" << getScale();
  return oss;
}

/* Amplitude accessor */
NumericalPoint ExponentialCauchy::getAmplitude() const
{
  return static_cast<AbsoluteExponential*>(covarianceModel_.getImplementation().get())->getAmplitude();
}

/* Scale accessor */
NumericalPoint ExponentialCauchy::getScale() const
{
  return static_cast<AbsoluteExponential*>(covarianceModel_.getImplementation().get())->getScale();
}

/* Method save() stores the object through the StorageManager */
void ExponentialCauchy::save(Advocate & adv) const
{
  SecondOrderModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ExponentialCauchy::load(Advocate & adv)
{
  SecondOrderModelImplementation::load(adv);
}
END_NAMESPACE_OPENTURNS
