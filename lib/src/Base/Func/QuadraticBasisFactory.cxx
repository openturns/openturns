//                                               -*- C++ -*-
/**
 *  @brief Quadratic canonical basis factory
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
#include "openturns/QuadraticBasisFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/QuadraticFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuadraticBasisFactory)

static const Factory<QuadraticBasisFactory> Factory_QuadraticBasisFactory;

/* Default constructor */
QuadraticBasisFactory::QuadraticBasisFactory (const UnsignedInteger inputDimension)
  : BasisFactory()
  , inputDimension_(inputDimension)
{
  // Nothing to do
}

/* String converter */
String QuadraticBasisFactory::__repr__() const
{
  return OSS() << "class=" << QuadraticBasisFactory::GetClassName()
         << " name=" << getName();
}

/* Here is the interface that all derived class must implement */

QuadraticBasisFactory* QuadraticBasisFactory::clone() const
{
  return new QuadraticBasisFactory(*this);
}

Basis QuadraticBasisFactory::build() const
{
  Collection<Function> functions;
  // constant term
  functions.add(LinearFunction (Point(inputDimension_, 0.0), Point(1, 1.0), Matrix(1, inputDimension_)));

  // linear terms
  for ( UnsignedInteger i = 0; i < inputDimension_; ++ i )
  {
    Matrix linear(1, inputDimension_);
    linear(0, i) = 1.0;
    functions.add(LinearFunction (Point(inputDimension_, 0.0), Point(1, 0.0), linear));
  }

  // square terms
  for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
  {
    for (UnsignedInteger j = 0; j <= i; ++ j)
    {
      SymmetricTensor quadratic(inputDimension_, 1);
      quadratic(i, j, 0) = (i == j) ? 2.0 : 1.0;
      Point center(inputDimension_, 0.0);
      Point constant(1, 0.0);
      Matrix linear(inputDimension_, 1);
      functions.add(QuadraticFunction(center, constant, linear, quadratic));
    }
  }
  return Basis(functions);
}

/* Method save() stores the object through the StorageManager */
void QuadraticBasisFactory::save(Advocate & adv) const
{
  BasisFactory::save(adv);
  adv.saveAttribute("inputDimension_", inputDimension_);
}

/* Method load() reloads the object from the StorageManager */
void QuadraticBasisFactory::load(Advocate & adv)
{
  BasisFactory::load(adv);
  adv.loadAttribute("inputDimension_", inputDimension_);
}

END_NAMESPACE_OPENTURNS
