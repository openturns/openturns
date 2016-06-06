//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ConstantBasisFactory
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
 *  Id      ConstantBasisFactory.cxx 2392 2012-02-17 18:35:43Z schueller
 */
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearNumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantBasisFactory);

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
  Basis basis;
  NumericalPoint constant(1);
  constant[0] = 1.;
  basis.add(LinearNumericalMathFunction (NumericalPoint(inputDimension_, 0.0),
                                         constant,
                                         Matrix(1, inputDimension_)));
  return basis;
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
