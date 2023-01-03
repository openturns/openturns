//                                               -*- C++ -*-
/**
 *  @brief ParametricHessian
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ParametricHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricHessian)

static const Factory<ParametricHessian> Factory_ParametricHessian;


/* Default constructor */
ParametricHessian::ParametricHessian()
  : HessianImplementation()
  , p_evaluation_(new ParametricEvaluation)
{
  // Nothing to do
}

/* Parameter constructor */
ParametricHessian::ParametricHessian(const ParametricEvaluation & evaluation)
  : HessianImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
ParametricHessian::ParametricHessian(const Pointer<ParametricEvaluation> & p_evaluation)
  : HessianImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor method */
ParametricHessian * ParametricHessian::clone() const
{
  return new ParametricHessian(*this);
}

/* Hessian operator */
SymmetricTensor ParametricHessian::hessian(const OT::Point & point) const
{
  const UnsignedInteger parametersDimension = p_evaluation_->getParameterDimension();
  const UnsignedInteger inputDimension = p_evaluation_->function_.getInputDimension();
  const UnsignedInteger pointDimension = point.getDimension();
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  Point x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[p_evaluation_->parametersPositions_[i]] = p_evaluation_->parameter_[i];
  for (UnsignedInteger i = 0; i < pointDimension; ++i) x[p_evaluation_->inputPositions_[i]] = point[i];
  const UnsignedInteger outputDimension = getOutputDimension();
  const SymmetricTensor fullHessian(p_evaluation_->function_.hessian(x));
  // The gradient wrt x corresponds to the inputPositions rows of the full gradient
  SymmetricTensor result(pointDimension, outputDimension);
  for (UnsignedInteger i = 0; i < pointDimension; ++i)
  {
    const UnsignedInteger i0 = p_evaluation_->inputPositions_[i];
    for (UnsignedInteger j = 0; j < pointDimension; ++j)
    {
      const UnsignedInteger j0 = p_evaluation_->inputPositions_[j];
      {
        for (UnsignedInteger k = 0; k < outputDimension; ++k)
          result(i, j, k) = fullHessian(i0, j0, k);
      }
    }
  }
  return result;
}

/* Evaluation accessors */
Evaluation ParametricHessian::getEvaluation() const
{
  return p_evaluation_->clone();
}

/* Dimension accessors */
UnsignedInteger ParametricHessian::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

UnsignedInteger ParametricHessian::getParameterDimension() const
{
  return p_evaluation_->getParameterDimension();
}

UnsignedInteger ParametricHessian::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* String converter */
String ParametricHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << ParametricHessian::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

String ParametricHessian::__str__(const String & ) const
{
  OSS oss;
  oss << ParametricHessian::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ParametricHessian::save(Advocate & adv) const
{
  HessianImplementation::save( adv );
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void ParametricHessian::load(Advocate & adv)
{
  HessianImplementation::load( adv );
  TypedInterfaceObject<ParametricEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}


END_NAMESPACE_OPENTURNS
