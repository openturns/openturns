//                                               -*- C++ -*-
/**
 *  @brief The hessian part of linear combination of polynomials
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
#include "openturns/LinearCombinationHessianImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearCombinationHessianImplementation);

static const Factory<LinearCombinationHessianImplementation> Factory_LinearCombinationHessianImplementation;


/* Default constructor */
LinearCombinationHessianImplementation::LinearCombinationHessianImplementation()
  : NumericalMathHessianImplementation(),
    evaluation_()
{
  // Nothing to do
}


/* Parameters constructor */
LinearCombinationHessianImplementation::LinearCombinationHessianImplementation(const LinearCombinationEvaluationImplementation & evaluation)
  : NumericalMathHessianImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
LinearCombinationHessianImplementation * LinearCombinationHessianImplementation::clone() const
{
  return new LinearCombinationHessianImplementation(*this);
}


/* Hessian method */
SymmetricTensor LinearCombinationHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size = evaluation_.functionsCollection_.getSize();
  const UnsignedInteger sheetSize = evaluation_.getOutputDimension();
  SymmetricTensor result(evaluation_.getInputDimension(), sheetSize);
  // We work on a sheet basis because there is no tensor arithmetic
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const SymmetricTensor currentTensor(evaluation_.functionsCollection_[i].hessian(inP));
    const NumericalScalar factor = evaluation_.coefficients_[i];
    for (UnsignedInteger k = 0; k < sheetSize; ++k)
    {
      SymmetricMatrix sheet(result.getSheet(k) + factor * currentTensor.getSheet(k));
      result.setSheet(k, sheet);
    }
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger LinearCombinationHessianImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger LinearCombinationHessianImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* String converter */
String LinearCombinationHessianImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " evaluation=" << evaluation_;
}

String LinearCombinationHessianImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset;
  const UnsignedInteger size = evaluation_.functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) oss << (i > 0 ? "+" : "") << "(" << evaluation_.coefficients_[i] << ")*" << evaluation_.functionsCollection_[i].getGradient()->__str__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LinearCombinationHessianImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearCombinationHessianImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}



END_NAMESPACE_OPENTURNS
