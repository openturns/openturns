//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for orthogonal basis
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OrthogonalFunctionFactory)

static const Factory<OrthogonalFunctionFactory> Factory_OrthogonalFunctionFactory;


/* Default constructor */
OrthogonalFunctionFactory::OrthogonalFunctionFactory()
  : FunctionalBasisImplementation()
  , measure_()
{
  // Nothing to do
}


/* Parameter constructor */
OrthogonalFunctionFactory::OrthogonalFunctionFactory(const Distribution & measure)
  : FunctionalBasisImplementation()
  , measure_(measure)
{
  // Nothing to do
}


/* Virtual constructor */
OrthogonalFunctionFactory * OrthogonalFunctionFactory::clone() const
{
  return new OrthogonalFunctionFactory(*this);
}


/* Build the Function of the given index */
Function OrthogonalFunctionFactory::build(const UnsignedInteger index) const
{
  throw NotYetImplementedException(HERE) << "In OrthogonalFunctionFactory::build(const UnsignedInteger index) const";
}


/* Return the measure upon which the basis is orthogonal */
Distribution OrthogonalFunctionFactory::getMeasure() const
{
  return measure_;
}

/* Return the enumerate function that translate unidimensional indices into multidimensional indices */
EnumerateFunction OrthogonalFunctionFactory::getEnumerateFunction() const
{
  throw NotYetImplementedException(HERE) << "In OrthogonalFunctionFactory::getEnumerateFunction() const";
}


Bool OrthogonalFunctionFactory::isOrthogonal() const
{
  return true;
}


/* String converter */
String OrthogonalFunctionFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}

/* Method save() stores the object through the StorageManager */
void OrthogonalFunctionFactory::save(Advocate & adv) const
{
  FunctionalBasisImplementation::save(adv);
  adv.saveAttribute( "measure_", measure_ );
  adv.saveAttribute( "enumerateFunction_", enumerateFunction_ );
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalFunctionFactory::load(Advocate & adv)
{
  FunctionalBasisImplementation::load(adv);
  adv.loadAttribute( "measure_", measure_ );
  adv.loadAttribute( "enumerateFunction_", enumerateFunction_ );
}


END_NAMESPACE_OPENTURNS
