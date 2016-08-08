//                                               -*- C++ -*-
/**
 *  @brief ParametricGradientImplementation
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
#include "openturns/ParametricGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricGradientImplementation);

static const Factory<ParametricGradientImplementation> Factory_ParametricGradientImplementation;


/* Default constructor */
ParametricGradientImplementation::ParametricGradientImplementation()
  : NumericalMathGradientImplementation()
  , p_evaluation_()
{
  // Nothing to do
}

/* Parameter constructor */
ParametricGradientImplementation::ParametricGradientImplementation(const EvaluationImplementation & p_evaluation)
  : NumericalMathGradientImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor method */
ParametricGradientImplementation * ParametricGradientImplementation::clone() const
{
  return new ParametricGradientImplementation(*this);
}

/* Gradient operator */
Matrix ParametricGradientImplementation::gradient(const NumericalPoint & point,
    const NumericalPoint & parameters) const
{
  const UnsignedInteger parametersDimension = parameters.getDimension();
  if (parametersDimension != p_evaluation_->getParametersPositions().getSize()) throw InvalidArgumentException(HERE) << "Error: expected a parameters of dimension=" << p_evaluation_->getParametersPositions().getSize() << ", got dimension=" << parametersDimension;
  const UnsignedInteger inputDimension = p_evaluation_->getFunction().getInputDimension();
  const UnsignedInteger pointDimension = point.getDimension();
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  NumericalPoint x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[p_evaluation_->parametersPositions_[i]] = parameters[i];
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

/* Gradient operator */
Matrix ParametricGradientImplementation::gradient(const OT::NumericalPoint & point) const
{
  // Use the current parameters value
  return gradient(point, p_evaluation_->getParameter());
}

/* Evaluation accessor */
ParametricEvaluationImplementation ParametricGradientImplementation::getEvaluation() const
{
  return *p_evaluation_;
}

/* Dimension accessors */
UnsignedInteger ParametricGradientImplementation::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

UnsignedInteger ParametricGradientImplementation::getParameterDimension() const
{
  return p_evaluation_->getParameterDimension();
}

UnsignedInteger ParametricGradientImplementation::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String ParametricGradientImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << ParametricGradientImplementation::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

String ParametricGradientImplementation::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << ParametricGradientImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ParametricGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save( adv );
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ParametricGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load( adv );
  TypedInterfaceObject<ParametricEvaluationImplementation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}


END_NAMESPACE_OPENTURNS
