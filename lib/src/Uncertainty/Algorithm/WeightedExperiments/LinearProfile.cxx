//                                               -*- C++ -*-
/**
 *  @brief LinearProfile
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
#include "openturns/LinearProfile.hxx"
#include "openturns/PersistentObjectFactory.hxx"


namespace OT
{

CLASSNAMEINIT(LinearProfile)

static const Factory<LinearProfile> Factory_LinearProfile;


/* Default constructor */
LinearProfile::LinearProfile(const Scalar T0, const UnsignedInteger iMax)
  : TemperatureProfileImplementation(T0, iMax)
{
  if (iMax_ == 0)
    iMaxInv_ = 1.0;
  else
    iMaxInv_ = 1.0 / static_cast<Scalar>(iMax_);
}

/* Virtual constructor method */
LinearProfile * LinearProfile::clone() const
{
  return new LinearProfile(*this);
}

/** Compute temperature T(i) */
Scalar LinearProfile::operator()(UnsignedInteger i) const
{
  if (i >= iMax_) return 0.0;

  return T0_ * (1.0 - i * iMaxInv_);
}

/* String converter */
String LinearProfile::__repr__() const
{
  OSS oss;
  oss << "class=" << LinearProfile::GetClassName()
      << " name=" << getName()
      << " T0=" << T0_
      << " iMax=" << iMax_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LinearProfile::save(Advocate & adv) const
{
  TemperatureProfileImplementation::save( adv );
}

/* Method load() reloads the object from the StorageManager */
void LinearProfile::load(Advocate & adv)
{
  TemperatureProfileImplementation::load( adv );
  if (iMax_ == 0)
    iMaxInv_ = 1.0;
  else
    iMaxInv_ = 1.0 / static_cast<Scalar>(iMax_);
}


} /* namespace OT */
