//                                               -*- C++ -*-
/**
 *  @brief
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
#include "Hanning.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Hanning
 */

CLASSNAMEINIT(Hanning);

static const Factory<Hanning> RegisteredFactory;

/* Constructor with parameters */
Hanning::Hanning()
  : FilteringWindowsImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
Hanning * Hanning::clone() const
{
  return new Hanning(*this);
}

/* String converter */
String Hanning::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Hanning::GetClassName();
  return oss;
}

/* String converter */
String Hanning::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << Hanning::GetClassName();
  return oss;
}

/* Evaluation of Hanning filter */
NumericalScalar Hanning::operator()(const NumericalScalar t) const
{
  if ((t <= 0.0) || (t >= 1.0))
    return 0.0;
  const NumericalScalar sinTheta(sin(M_PI * t));
  // 1.632993161855452065464856 = sqrt(8/3)
  return 1.632993161855452065464856 * sinTheta * sinTheta;
}

/* Method save() stores the object through the StorageManager */
void Hanning::save(Advocate & adv) const
{
  FilteringWindowsImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void Hanning::load(Advocate & adv)
{
  FilteringWindowsImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
