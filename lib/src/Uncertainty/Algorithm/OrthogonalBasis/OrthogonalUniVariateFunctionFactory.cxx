//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D function factories
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
#include "openturns/OrthogonalUniVariateFunctionFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariateFunctionFactory);

static const Factory<OrthogonalUniVariateFunctionFactory> RegisteredFactory;


/* Default constructor */
OrthogonalUniVariateFunctionFactory::OrthogonalUniVariateFunctionFactory()
  : PersistentObject()
  , measure_()
  , functionsCache_(0)
{
  // Nothing to do. The derived class will have to call initializeCaches().
}


/* Constructor */
OrthogonalUniVariateFunctionFactory::OrthogonalUniVariateFunctionFactory(const Distribution & measure)
  : PersistentObject()
  , measure_(measure)
  , functionsCache_(0)
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


/* The method to get the function of any order */
UniVariateFunction OrthogonalUniVariateFunctionFactory::build(const UnsignedInteger order) const
{
  throw NotYetImplementedException(HERE) << "OrthogonalUniVariateFunctionFactory::build";
}


/* Measure accessor */
Distribution OrthogonalUniVariateFunctionFactory::getMeasure() const
{
  return measure_;
}

/* Cache initialization */
void OrthogonalUniVariateFunctionFactory::initializeCache()
{

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
