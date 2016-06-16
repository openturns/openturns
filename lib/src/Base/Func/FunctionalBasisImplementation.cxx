//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
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
#include "openturns/FunctionalBasisImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FunctionalBasisImplementation);

static const Factory<FunctionalBasisImplementation> Factory_FunctionalBasisImplementation;


/* Default constructor */
FunctionalBasisImplementation::FunctionalBasisImplementation()
  : BasisImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
FunctionalBasisImplementation * FunctionalBasisImplementation::clone() const
{
  return new FunctionalBasisImplementation(*this);
}

Bool FunctionalBasisImplementation::isFunctional() const
{
  return true;
}


String FunctionalBasisImplementation::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

void FunctionalBasisImplementation::save(Advocate & adv) const
{
  BasisImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void FunctionalBasisImplementation::load(Advocate & adv)
{
  BasisImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
