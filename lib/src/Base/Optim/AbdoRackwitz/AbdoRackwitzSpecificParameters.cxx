//                                               -*- C++ -*-
/**
 *  @brief AbdoRackwitz specific parameters.
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
#include <cstdlib>

#include "AbdoRackwitzSpecificParameters.hxx"
#include "ResourceMap.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(AbdoRackwitzSpecificParameters);

static Factory<AbdoRackwitzSpecificParameters> RegisteredFactory("AbdoRackwitzSpecificParameters");


/* Default onstructor */
AbdoRackwitzSpecificParameters::AbdoRackwitzSpecificParameters()
  : PersistentObject(),
    tau_(ResourceMap::GetAsNumericalScalar( "AbdoRackwitz-DefaultTau" )),
    omega_(ResourceMap::GetAsNumericalScalar( "AbdoRackwitz-DefaultOmega" )),
    smooth_(ResourceMap::GetAsNumericalScalar( "AbdoRackwitz-DefaultSmooth" ))
{
  // Nothing to do
}



AbdoRackwitzSpecificParameters::AbdoRackwitzSpecificParameters(const NumericalScalar tau,
    const NumericalScalar omega,
    const NumericalScalar smooth)
  : PersistentObject(),
    tau_(tau),
    omega_(omega),
    smooth_(smooth)
{
  // Nothing to do
}


/* Virtual constructor */
AbdoRackwitzSpecificParameters * AbdoRackwitzSpecificParameters::clone() const
{
  return new AbdoRackwitzSpecificParameters(*this);
}

/* Tau accessor */
NumericalScalar AbdoRackwitzSpecificParameters::getTau() const
{
  return tau_;
}

/* Tau accessor */
void AbdoRackwitzSpecificParameters::setTau(const NumericalScalar tau)
{
  tau_ = tau;
}

/* Omega accessor */
NumericalScalar AbdoRackwitzSpecificParameters::getOmega() const
{
  return omega_;
}

/* Omega accessor */
void AbdoRackwitzSpecificParameters::setOmega(const NumericalScalar omega)
{
  omega_ = omega;
}

/* Smooth accessor */
NumericalScalar AbdoRackwitzSpecificParameters::getSmooth() const
{
  return smooth_;
}

/* Smooth accessor */
void AbdoRackwitzSpecificParameters::setSmooth(const NumericalScalar smooth)
{
  smooth_ = smooth;
}

/* String converter */
String AbdoRackwitzSpecificParameters::__repr__() const
{
  OSS oss;
  oss << "class=" << AbdoRackwitzSpecificParameters::GetClassName()
      << " tau=" << tau_
      << " omega=" << omega_
      << " smooth=" << smooth_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void AbdoRackwitzSpecificParameters::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("tau_", tau_);
  adv.saveAttribute("omega_", omega_);
  adv.saveAttribute("smooth_", smooth_);
}

/* Method load() reloads the object from the StorageManager */
void AbdoRackwitzSpecificParameters::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("tau_", tau_);
  adv.loadAttribute("omega_", omega_);
  adv.loadAttribute("smooth_", smooth_);
}

END_NAMESPACE_OPENTURNS



