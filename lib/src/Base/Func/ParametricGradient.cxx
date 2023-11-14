//                                               -*- C++ -*-
/**
 *  @brief ParametricGradient
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ParametricGradient.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/FiniteDifferenceStep.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricGradient)

static const Factory<ParametricGradient> Factory_ParametricGradient;


/* Default constructor */
ParametricGradient::ParametricGradient()
  : GradientImplementation()
  , p_evaluation_(new ParametricEvaluation)
{
  // Nothing to do
}

/* Parameter constructor */
ParametricGradient::ParametricGradient(const ParametricEvaluation & evaluation)
  : GradientImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
ParametricGradient::ParametricGradient(const Pointer<ParametricEvaluation> & p_evaluation)
  : GradientImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor method */
ParametricGradient * ParametricGradient::clone() const
{
  return new ParametricGradient(*this);
}

/* Gradient operator */
Matrix ParametricGradient::gradient(const OT::Point & point) const
{
  const UnsignedInteger parametersDimension = p_evaluation_->getParameterDimension();
  const UnsignedInteger inputDimension = p_evaluation_->getFunction().getInputDimension();
  const UnsignedInteger pointDimension = point.getDimension();
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  // Special case if the gradient of the underlying function is based on finite differences
  {
    const CenteredFiniteDifferenceGradient * p_gradient = dynamic_cast<const CenteredFiniteDifferenceGradient *>(p_evaluation_->function_.getGradient().getImplementation().get());
    if (p_gradient)
    {
      // Retrieve the full gradient parameters
      FiniteDifferenceStep step(p_gradient->getFiniteDifferenceStep());
      const Point fullEpsilon(step.getEpsilon());
      // Build the step restricted to the parameter set of the function
      Point reducedEpsilon(pointDimension);
      for (UnsignedInteger i = 0; i < pointDimension; ++i)
        reducedEpsilon[i] = fullEpsilon[p_evaluation_->inputPositions_[i]];
      // Update the step
      step.setEpsilon(reducedEpsilon);
      const CenteredFiniteDifferenceGradient reducedGradient(step, ParametricEvaluation(p_evaluation_->function_, p_evaluation_->parametersPositions_, p_evaluation_->parameter_));
      return reducedGradient.gradient(point);
    }
  }
  // Second try: NonCenteredFiniteDifferenceGradient
  {
    const NonCenteredFiniteDifferenceGradient * p_gradient = dynamic_cast<const NonCenteredFiniteDifferenceGradient *>(p_evaluation_->function_.getGradient().getImplementation().get());
    if (p_gradient)
    {
      // Retrieve the full gradient parameters
      FiniteDifferenceStep step(p_gradient->getFiniteDifferenceStep());
      const Point fullEpsilon(step.getEpsilon());
      // Build the step restricted to the parameter set of the function
      Point reducedEpsilon(pointDimension);
      for (UnsignedInteger i = 0; i < pointDimension; ++i)
        reducedEpsilon[i] = fullEpsilon[p_evaluation_->inputPositions_[i]];
      // Update the step
      step.setEpsilon(reducedEpsilon);
      const NonCenteredFiniteDifferenceGradient reducedGradient(step, ParametricEvaluation(p_evaluation_->function_, p_evaluation_->parametersPositions_, p_evaluation_->parameter_));
      return reducedGradient.gradient(point);
    }
  }
  Point x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[p_evaluation_->parametersPositions_[i]] = p_evaluation_->parameter_[i];
  for (UnsignedInteger i = 0; i < pointDimension; ++i) x[p_evaluation_->inputPositions_[i]] = point[i];
  const UnsignedInteger outputDimension = getOutputDimension();
  const Matrix fullGradient(p_evaluation_->getFunction().gradient(x));
  // The gradient wrt x corresponds to the inputPositions rows of the full gradient
  Matrix result(pointDimension, outputDimension);
  for (UnsignedInteger i = 0; i < pointDimension; ++i)
  {
    const UnsignedInteger i0 = p_evaluation_->inputPositions_[i];
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      result(i, j) = fullGradient(i0, j);
  }
  return result;
}

/* Dimension accessors */
UnsignedInteger ParametricGradient::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

UnsignedInteger ParametricGradient::getParameterDimension() const
{
  return p_evaluation_->getParameterDimension();
}

UnsignedInteger ParametricGradient::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String ParametricGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << ParametricGradient::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

String ParametricGradient::__str__(const String & ) const
{
  OSS oss;
  oss << ParametricGradient::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ParametricGradient::save(Advocate & adv) const
{
  GradientImplementation::save( adv );
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ParametricGradient::load(Advocate & adv)
{
  GradientImplementation::load( adv );
  TypedInterfaceObject<ParametricEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}


END_NAMESPACE_OPENTURNS
