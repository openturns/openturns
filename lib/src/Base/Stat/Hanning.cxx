//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/Hanning.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Hanning
 */

CLASSNAMEINIT(Hanning)

static const Factory<Hanning> Factory_Hanning;

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
String Hanning::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << Hanning::GetClassName();
  return oss;
}

/* Evaluation of Hanning filter */
Scalar Hanning::operator()(const Scalar t) const
{
  if ((t <= 0.0) || (t >= 1.0))
    return 0.0;
  const Scalar sinTheta = sin(M_PI * t);
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
