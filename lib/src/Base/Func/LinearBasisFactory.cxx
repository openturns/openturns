//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all LinearBasisFactory
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Id      LinearBasisFactory.cxx 2392 2012-02-17 18:35:43Z schueller
 */
#include "LinearBasisFactory.hxx"
#include "PersistentObjectFactory.hxx"
#include "LinearNumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearBasisFactory);

static const Factory<LinearBasisFactory> RegisteredFactory;

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
  Basis basis;

  // constant term
  basis.add(LinearNumericalMathFunction (NumericalPoint(inputDimension_, 0.0), NumericalPoint(1, 1.0), Matrix(1, inputDimension_)));

  // linear term
  for ( UnsignedInteger i = 0; i < inputDimension_; ++ i )
  {
    Matrix linear(1, inputDimension_);
    linear(0, i) = 1.0;
    basis.add(LinearNumericalMathFunction (NumericalPoint(inputDimension_, 0.0), NumericalPoint(1, 0.0), linear));
  }

  return basis;
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
