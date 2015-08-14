//                                               -*- C++ -*-
/**
 *  @brief The gradient part of functional linear combination of vectors
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "DualLinearCombinationGradientImplementation.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(DualLinearCombinationGradientImplementation);

static Factory<DualLinearCombinationGradientImplementation> RegisteredFactory("DualLinearCombinationGradientImplementation");


/* Default constructor */
DualLinearCombinationGradientImplementation::DualLinearCombinationGradientImplementation()
  : NumericalMathGradientImplementation()
  , evaluation_()
{
  // Nothing to do
}


/* Parameters constructor */
DualLinearCombinationGradientImplementation::DualLinearCombinationGradientImplementation(const DualLinearCombinationEvaluationImplementation & evaluation)
  : NumericalMathGradientImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
DualLinearCombinationGradientImplementation * DualLinearCombinationGradientImplementation::clone() const
{
  return new DualLinearCombinationGradientImplementation(*this);
}


/* Gradient method */
Matrix DualLinearCombinationGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size(evaluation_.functionsCollection_.getSize());
  const UnsignedInteger outputDimension(getOutputDimension());
  Matrix result(inputDimension, outputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Matrix gradientI(evaluation_.functionsCollection_[i].gradient(inP));
    const NumericalPoint coefficientI(evaluation_.coefficients_[i]);
    for (UnsignedInteger j = 0; j < inputDimension; ++j)
      for (UnsignedInteger k = 0; k < outputDimension; ++k)
        result(j, k) += gradientI(j, 0) * coefficientI[k];
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger DualLinearCombinationGradientImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger DualLinearCombinationGradientImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* String converter */
String DualLinearCombinationGradientImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " evaluation=" << evaluation_;
}


/* Method save() stores the object through the StorageManager */
void DualLinearCombinationGradientImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void DualLinearCombinationGradientImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
