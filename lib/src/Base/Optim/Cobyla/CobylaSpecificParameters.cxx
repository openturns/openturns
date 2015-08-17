//                                               -*- C++ -*-
/**
 *  @brief CobylaSpecificParameters is an encapsulation of the Cobyla specific
 *         parameters.
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

#include "CobylaObsolete.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CobylaSpecificParameters);

static Factory<CobylaSpecificParameters> RegisteredFactory("CobylaSpecificParameters");


/* Default with parameters */
CobylaSpecificParameters::CobylaSpecificParameters():
  PersistentObject(),
  rhoBeg_(ResourceMap::GetAsNumericalScalar( "Cobyla-DefaultRhoBeg" ))
{
  // Nothing to do
} /* SpecificParametersCobyla */

/* Parameters with parameters */
CobylaSpecificParameters::CobylaSpecificParameters(const NumericalScalar rhoBeg):
  PersistentObject()
  , rhoBeg_(rhoBeg)
{
  // Nothing to do
} /* SpecificParametersCobyla */

/* Virtual constructor */
CobylaSpecificParameters * CobylaSpecificParameters::clone() const
{
  return new CobylaSpecificParameters(*this);
}

/* RhoBeg accessor */
NumericalScalar CobylaSpecificParameters::getRhoBeg() const
{
  return rhoBeg_;
}

/* RhoBeg accessor */
void CobylaSpecificParameters::setRhoBeg(const NumericalScalar rhoBeg)
{
  rhoBeg_ = rhoBeg;
}

/* String converter */
String CobylaSpecificParameters::__repr__() const
{
  OSS oss;
  oss << "class=" << CobylaSpecificParameters::GetClassName()
      << " rhoBeg=" << rhoBeg_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CobylaSpecificParameters::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("rhoBeg_", rhoBeg_);
}

/* Method load() reloads the object from the StorageManager */
void CobylaSpecificParameters::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("rhoBeg_", rhoBeg_);
}

END_NAMESPACE_OPENTURNS



