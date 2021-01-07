//                                               -*- C++ -*-
/**
 *  @brief Linear canonical basis factory
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LinearBasisFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearBasisFactory)

static const Factory<LinearBasisFactory> Factory_LinearBasisFactory;

/* Default constructor */
LinearBasisFactory::LinearBasisFactory (const UnsignedInteger inputDimension)
  : BasisFactory()
  , inputDimension_(inputDimension)
{
  // Nothing to do
}

/* String converter */
String LinearBasisFactory::__repr__() const
{
  return OSS() << "class=" << LinearBasisFactory::GetClassName()
         << " name=" << getName();
}

/* Here is the interface that all derived class must implement */

LinearBasisFactory* LinearBasisFactory::clone() const
{
  return new LinearBasisFactory(*this);
}

Basis LinearBasisFactory::build() const
{
  Collection<Function> functions;

  // constant term
  functions.add(LinearFunction (Point(inputDimension_, 0.0), Point(1, 1.0), Matrix(1, inputDimension_)));

  // linear term
  for ( UnsignedInteger i = 0; i < inputDimension_; ++ i )
  {
    Matrix linear(1, inputDimension_);
    linear(0, i) = 1.0;
    functions.add(LinearFunction (Point(inputDimension_, 0.0), Point(1, 0.0), linear));
  }

  return Basis(functions);
}

/* Method save() stores the object through the StorageManager */
void LinearBasisFactory::save(Advocate & adv) const
{
  BasisFactory::save(adv);
  adv.saveAttribute("inputDimension_", inputDimension_);
}

/* Method load() reloads the object from the StorageManager */
void LinearBasisFactory::load(Advocate & adv)
{
  BasisFactory::load(adv);
  adv.loadAttribute("inputDimension_", inputDimension_);
}

END_NAMESPACE_OPENTURNS
