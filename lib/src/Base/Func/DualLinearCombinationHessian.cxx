//                                               -*- C++ -*-
/**
 *  @brief The hessian part of functional linear combination of vectors
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DualLinearCombinationHessian.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(DualLinearCombinationHessian)

static const Factory<DualLinearCombinationHessian> Factory_DualLinearCombinationHessian;


/* Default constructor */
DualLinearCombinationHessian::DualLinearCombinationHessian()
  : HessianImplementation()
  , p_evaluation_(new DualLinearCombinationEvaluation)
{
  // Nothing to do
}


/* Parameters constructor */
DualLinearCombinationHessian::DualLinearCombinationHessian(const DualLinearCombinationEvaluation & evaluation)
  : HessianImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
DualLinearCombinationHessian::DualLinearCombinationHessian(const Pointer<DualLinearCombinationEvaluation> & p_evaluation)
  : HessianImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
DualLinearCombinationHessian * DualLinearCombinationHessian::clone() const
{
  return new DualLinearCombinationHessian(*this);
}


/* Hessian method */
SymmetricTensor DualLinearCombinationHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size = p_evaluation_->functionsCollection_.getSize();
  const UnsignedInteger outputDimension = getOutputDimension();
  SymmetricTensor result(inputDimension, outputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const SymmetricTensor hessianI(p_evaluation_->functionsCollection_[i].hessian(inP));
    const Point coefficientI(p_evaluation_->coefficients_[i]);
    for (UnsignedInteger j = 0; j < inputDimension; ++j)
      for (UnsignedInteger k = 0; k <= j; ++k)
        for (UnsignedInteger n = 0; n < outputDimension; ++n)
          result(j, k, n) += hessianI(j, k, 0) * coefficientI[n];
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger DualLinearCombinationHessian::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger DualLinearCombinationHessian::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String DualLinearCombinationHessian::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " evaluation=" << *p_evaluation_;
}


/* Method save() stores the object through the StorageManager */
void DualLinearCombinationHessian::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void DualLinearCombinationHessian::load(Advocate & adv)
{
  PersistentObject::load(adv);
  TypedInterfaceObject<DualLinearCombinationEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}

END_NAMESPACE_OPENTURNS
