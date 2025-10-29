//                                               -*- C++ -*-
/**
 *  @brief This is a finite orthogonal set of functions wrt a given
 *         distribution.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FiniteOrthogonalFunctionFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteOrthogonalFunctionFactory)

static const Factory<FiniteOrthogonalFunctionFactory> Factory_FiniteOrthogonalFunctionFactory;

/* Default constructor */
FiniteOrthogonalFunctionFactory::FiniteOrthogonalFunctionFactory()
  : OrthogonalFunctionFactory()
  , functions_()
{
  // Nothing to do
}


/* Constructor */
FiniteOrthogonalFunctionFactory::FiniteOrthogonalFunctionFactory(const FunctionCollection & functions,
								 const Distribution & measure)
  : OrthogonalFunctionFactory(measure)
  , functions_()
{
  // Check the given functions wrt the measure
  setFunctionsCollection(functions);
}

/* Virtual constructor */
FiniteOrthogonalFunctionFactory * FiniteOrthogonalFunctionFactory::clone() const
{
  return new FiniteOrthogonalFunctionFactory(*this);
}


/* Build the Function of the given index */
Function FiniteOrthogonalFunctionFactory::build(const UnsignedInteger index) const
{
  if (index >= functions_.getSize()) throw InvalidArgumentException(HERE) << "Error: the given index=" << index << " is greater than the size of the functions set=" << functions_.getSize();
  return functions_[index];
}

/* Functions accessor */
void FiniteOrthogonalFunctionFactory::setFunctionsCollection(const FunctionCollection & functions)
{
  const UnsignedInteger size = functions.getSize();
  const UnsignedInteger dimension = measure_.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      if (functions[i].getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
      if (functions[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
    }
  functions_ = functions;
}

FiniteOrthogonalFunctionFactory::FunctionCollection FiniteOrthogonalFunctionFactory::getFunctionsCollection() const
{
  return functions_;
}

  
/* String converter */
String FiniteOrthogonalFunctionFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " functions=" << functions_
      << " measure=" << measure_;
  return oss;
}

String FiniteOrthogonalFunctionFactory::__str__(const String & /*offset*/) const
{
  OSS oss(false);
  oss << getClassName()
      << "(functions=" << functions_
      << ", measure=" << measure_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FiniteOrthogonalFunctionFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute( "functions_", functions_ );
}


/* Method load() reloads the object from the StorageManager */
void FiniteOrthogonalFunctionFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute( "functions_", functions_ );
}



END_NAMESPACE_OPENTURNS
