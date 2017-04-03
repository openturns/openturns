//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all QuadraticBasisFactory
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  Id      QuadraticBasisFactory.cxx 2392 2012-02-17 18:35:43Z schueller
 */
#include "openturns/QuadraticBasisFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/QuadraticFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuadraticBasisFactory);

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
  Basis basis;
  // constant term
  basis.add(LinearFunction (Point(inputDimension_, 0.0), Point(1, 1.0), Matrix(1, inputDimension_)));

  // linear terms
  for ( UnsignedInteger i = 0; i < inputDimension_; ++ i )
  {
    Matrix linear(1, inputDimension_);
    linear(0, i) = 1.0;
    basis.add(LinearFunction (Point(inputDimension_, 0.0), Point(1, 0.0), linear));
  }

  // square terms
  for ( UnsignedInteger i = 0; i < inputDimension_; ++ i )
  {
    for ( UnsignedInteger j = i; j < inputDimension_; ++ j )
    {
      SymmetricTensor quadratic(inputDimension_, 1);
      quadratic(i, j, 0) = 1.0;
      Point center(inputDimension_, 0.0);
      Point constant(1, 0.0);
      Matrix linear(inputDimension_, 1);
      basis.add(QuadraticFunction(center, constant, linear, quadratic));
    }
  }
  return basis;
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
