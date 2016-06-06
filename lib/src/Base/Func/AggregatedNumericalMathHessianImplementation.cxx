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
#include "openturns/AggregatedNumericalMathHessianImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedNumericalMathHessianImplementation);

static const Factory<AggregatedNumericalMathHessianImplementation> Factory_AggregatedNumericalMathHessianImplementation;


/* Default constructor */
AggregatedNumericalMathHessianImplementation::AggregatedNumericalMathHessianImplementation()
  : NumericalMathHessianImplementation()
  , evaluation_()
{
  // Nothing to do
}


/* Parameters constructor */
AggregatedNumericalMathHessianImplementation::AggregatedNumericalMathHessianImplementation(const AggregatedNumericalMathEvaluationImplementation & evaluation)
  : NumericalMathHessianImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
AggregatedNumericalMathHessianImplementation * AggregatedNumericalMathHessianImplementation::clone() const
{
  return new AggregatedNumericalMathHessianImplementation(*this);
}


/* Hessian method */
SymmetricTensor AggregatedNumericalMathHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size(evaluation_.functionsCollection_.getSize());
  SymmetricTensor result(evaluation_.getInputDimension(), evaluation_.getOutputDimension());
  UnsignedInteger sheetIndex(0);
  // Loop over the contributors
  for (UnsignedInteger contributorIndex = 0; contributorIndex < size; ++contributorIndex)
  {
    const SymmetricTensor contributorHessian(evaluation_.functionsCollection_[contributorIndex].hessian(inP));
    // Copy the contributor hessian into the global tensor hessian
    for (UnsignedInteger k = 0; k < contributorHessian.getNbSheets(); ++k)
    {
      for (UnsignedInteger j = 0; j < contributorHessian.getNbColumns(); ++j)
        for (UnsignedInteger i = j; i < contributorHessian.getNbRows(); ++i)
          result(i, j, sheetIndex) = contributorHessian(i, j, k);
      ++sheetIndex;
    }
  } // contributorIndex
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger AggregatedNumericalMathHessianImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger AggregatedNumericalMathHessianImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* String converter */
String AggregatedNumericalMathHessianImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " evaluation=" << evaluation_;
}

String AggregatedNumericalMathHessianImplementation::__str__(const String & offset) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void AggregatedNumericalMathHessianImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void AggregatedNumericalMathHessianImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}



END_NAMESPACE_OPENTURNS
