//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D function factories
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OrthogonalUniVariateFunctionFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariateFunctionFactory)

static const Factory<OrthogonalUniVariateFunctionFactory> Factory_OrthogonalUniVariateFunctionFactory;


/* Default constructor */
OrthogonalUniVariateFunctionFactory::OrthogonalUniVariateFunctionFactory()
  : UniVariateFunctionFactory()
  , measure_()
{
  // Nothing to do. The derived class will have to call initializeCaches().
}


/* Constructor */
OrthogonalUniVariateFunctionFactory::OrthogonalUniVariateFunctionFactory(const Distribution & measure)
  : UniVariateFunctionFactory()
  , measure_(measure)
{
  // Nothing to do. The derived class will have to call initializeCaches().
}


/* Virtual constructor */
OrthogonalUniVariateFunctionFactory * OrthogonalUniVariateFunctionFactory::clone() const
{
  return new OrthogonalUniVariateFunctionFactory(*this);
}


/* String converter */
String OrthogonalUniVariateFunctionFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}

/* Measure accessor */
Distribution OrthogonalUniVariateFunctionFactory::getMeasure() const
{
  return measure_;
}

/* Method save() stores the object through the StorageManager */
void OrthogonalUniVariateFunctionFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("measure_", measure_);
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalUniVariateFunctionFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("measure_", measure_);
}

END_NAMESPACE_OPENTURNS
