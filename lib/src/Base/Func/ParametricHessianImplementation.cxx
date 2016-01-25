//                                               -*- C++ -*-
/**
 *  @brief ParametricHessianImplementation
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
 */
#include "ParametricHessianImplementation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricHessianImplementation);

static const Factory<ParametricHessianImplementation> RegisteredFactory;


/* Default constructor */
ParametricHessianImplementation::ParametricHessianImplementation()
  : NumericalMathHessianImplementation()
  , p_evaluation_()
{
  // Nothing to do
}

/* Parameter constructor */
ParametricHessianImplementation::ParametricHessianImplementation(const EvaluationImplementation & p_evaluation)
  : NumericalMathHessianImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor method */
ParametricHessianImplementation * ParametricHessianImplementation::clone() const
{
  return new ParametricHessianImplementation(*this);
}

/* Hessian operator */
SymmetricTensor ParametricHessianImplementation::hessian(const NumericalPoint & point,
    const NumericalPoint & parameters) const
{
  const UnsignedInteger parametersDimension(parameters.getDimension());
  if (parametersDimension != p_evaluation_->getParametersPositions().getSize()) throw InvalidArgumentException(HERE) << "Error: expected a parameters of dimension=" << p_evaluation_->getParametersPositions().getSize() << ", got dimension=" << parametersDimension;
  const UnsignedInteger inputDimension(p_evaluation_->function_.getInputDimension());
  const UnsignedInteger pointDimension(point.getDimension());
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  NumericalPoint x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[p_evaluation_->parametersPositions_[i]] = parameters[i];
  for (UnsignedInteger i = 0; i < pointDimension; ++i) x[p_evaluation_->inputPositions_[i]] = point[i];
  const UnsignedInteger outputDimension(getOutputDimension());
  const SymmetricTensor fullHessian(p_evaluation_->function_.hessian(x));
  // The gradient wrt x corresponds to the inputPositions rows of the full gradient
  SymmetricTensor result(pointDimension, outputDimension);
  for (UnsignedInteger i = 0; i < pointDimension; ++i)
  {
    const UnsignedInteger i0(p_evaluation_->inputPositions_[i]);
    for (UnsignedInteger j = 0; j < pointDimension; ++j)
    {
      const UnsignedInteger j0(p_evaluation_->inputPositions_[j]);
      {
        for (UnsignedInteger k = 0; k < outputDimension; ++k)
          result(i, j, k) = fullHessian(i0, j0, k);
      }
    }
  }
  return result;
}

/* Hessian operator */
SymmetricTensor ParametricHessianImplementation::hessian(const OT::NumericalPoint & point) const
{
  // Use the current parameters value
  return hessian(point, p_evaluation_->getParameter());
}

/* Evaluation accessors */
ParametricEvaluationImplementation ParametricHessianImplementation::getEvaluation() const
{
  return *p_evaluation_;
}

/* Dimension accessors */
UnsignedInteger ParametricHessianImplementation::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

UnsignedInteger ParametricHessianImplementation::getParameterDimension() const
{
  return p_evaluation_->getParameterDimension();
}

UnsignedInteger ParametricHessianImplementation::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String ParametricHessianImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << ParametricHessianImplementation::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

String ParametricHessianImplementation::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << ParametricHessianImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ParametricHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save( adv );
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ParametricHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load( adv );
  TypedInterfaceObject<ParametricEvaluationImplementation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}


END_NAMESPACE_OPENTURNS
