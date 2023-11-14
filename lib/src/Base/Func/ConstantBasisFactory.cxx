//                                               -*- C++ -*-
/**
 *  @brief Constant basis factory
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
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantBasisFactory)

static const Factory<ConstantBasisFactory> Factory_ConstantBasisFactory;

/* Default constructor */
ConstantBasisFactory::ConstantBasisFactory (const UnsignedInteger inputDimension)
  : BasisFactory()
  , inputDimension_(inputDimension)
{
  // Nothing to do
}

/* String converter */
String ConstantBasisFactory::__repr__() const
{
  return OSS() << "class=" << ConstantBasisFactory::GetClassName()
         << " name=" << getName();
}

/* Here is the interface that all derived class must implement */

ConstantBasisFactory* ConstantBasisFactory::clone() const
{
  return new ConstantBasisFactory(*this);
}

Basis ConstantBasisFactory::build() const
{
  Point constant(1);
  constant[0] = 1.;
  return Basis(Collection<Function>(1,
                                    LinearFunction (Point(inputDimension_, 0.0),
                                        constant,
                                        Matrix(1, inputDimension_))));
}

/* Method save() stores the object through the StorageManager */
void ConstantBasisFactory::save(Advocate & adv) const
{
  BasisFactory::save(adv);
  adv.saveAttribute("inputDimension_", inputDimension_);
}

/* Method load() reloads the object from the StorageManager */
void ConstantBasisFactory::load(Advocate & adv)
{
  BasisFactory::load(adv);
  adv.loadAttribute("inputDimension_", inputDimension_);
}

END_NAMESPACE_OPENTURNS
