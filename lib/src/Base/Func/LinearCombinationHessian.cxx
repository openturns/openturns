//                                               -*- C++ -*-
/**
 *  @brief The hessian part of linear combination of polynomials
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LinearCombinationHessian.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearCombinationHessian)

static const Factory<LinearCombinationHessian> Factory_LinearCombinationHessian;


/* Default constructor */
LinearCombinationHessian::LinearCombinationHessian()
  : HessianImplementation(),
    p_evaluation_(new LinearCombinationEvaluation)
{
  // Nothing to do
}


/* Parameters constructor */
LinearCombinationHessian::LinearCombinationHessian(const LinearCombinationEvaluation & evaluation)
  : HessianImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
LinearCombinationHessian::LinearCombinationHessian(const Pointer<LinearCombinationEvaluation> & p_evaluation)
  : HessianImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
LinearCombinationHessian * LinearCombinationHessian::clone() const
{
  return new LinearCombinationHessian(*this);
}


/* Hessian method */
SymmetricTensor LinearCombinationHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const UnsignedInteger size = p_evaluation_->functionsCollection_.getSize();
  const UnsignedInteger sheetSize = p_evaluation_->getOutputDimension();
  SymmetricTensor result(p_evaluation_->getInputDimension(), sheetSize);
  // We work on a sheet basis because there is no tensor arithmetic
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const SymmetricTensor currentTensor(p_evaluation_->functionsCollection_[i].hessian(inP));
    const Scalar factor = p_evaluation_->coefficients_[i];
    for (UnsignedInteger k = 0; k < sheetSize; ++k)
    {
      SymmetricMatrix sheet(result.getSheet(k) + factor * currentTensor.getSheet(k));
      result.setSheet(k, sheet);
    }
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger LinearCombinationHessian::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger LinearCombinationHessian::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String LinearCombinationHessian::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " evaluation=" << *p_evaluation_;
}

String LinearCombinationHessian::__str__(const String & ) const
{
  OSS oss(false);
  const UnsignedInteger size = p_evaluation_->functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) oss << (i > 0 ? "+" : "") << "(" << p_evaluation_->coefficients_[i] << ")*" << p_evaluation_->functionsCollection_[i].getGradient().__str__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LinearCombinationHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearCombinationHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "evaluation_", *p_evaluation_ );
}



END_NAMESPACE_OPENTURNS
