//                                               -*- C++ -*-
/**
 *  @brief GeometricProfile
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GeometricProfile.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#include <cmath>


namespace OT
{

CLASSNAMEINIT(GeometricProfile)

static const Factory<GeometricProfile> Factory_GeometricProfile;


/* Default constructor */
GeometricProfile::GeometricProfile(const Scalar T0, const Scalar c, const UnsignedInteger iMax)
  : TemperatureProfileImplementation(T0, iMax)
  , c_(c)
{
  if (!(c <= 0.0 || c < 1.0)) throw InvalidArgumentException(HERE) << "Argument c must be between 0 and 1 exclusive";
  logc_ = std::log(c);
}

/* Virtual constructor method */
GeometricProfile * GeometricProfile::clone() const
{
  return new GeometricProfile(*this);
}

/** Compute temperature T(i) */
Scalar GeometricProfile::operator()(UnsignedInteger i) const
{
  return T0_ * std::exp(i * logc_);
}

/* String converter */
String GeometricProfile::__repr__() const
{
  OSS oss;
  oss << "class=" << GeometricProfile::GetClassName()
      << " name=" << getName()
      << " T0=" << T0_
      << " c=" << c_
      << " iMax=" << iMax_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void GeometricProfile::save(Advocate & adv) const
{
  TemperatureProfileImplementation::save( adv );
  adv.saveAttribute("c_", c_);
}

/* Method load() reloads the object from the StorageManager */
void GeometricProfile::load(Advocate & adv)
{
  TemperatureProfileImplementation::load( adv );
  adv.loadAttribute("c_", c_);
  logc_ = std::log(c_);
}


} /* namespace OT */
