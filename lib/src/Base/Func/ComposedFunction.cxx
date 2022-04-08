//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the composed function
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
#include "openturns/ComposedFunction.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"
#include "openturns/ComposedEvaluation.hxx"
#include "openturns/ComposedGradient.hxx"
#include "openturns/ComposedHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComposedFunction)

static const Factory<ComposedFunction> Factory_ComposedFunction;

/* Default constructor */
ComposedFunction::ComposedFunction()
  : FunctionImplementation()
  , p_leftFunction_(new FunctionImplementation)
  , p_rightFunction_(new FunctionImplementation)
{
  // Nothing to do
}

/* Composition constructor */
ComposedFunction::ComposedFunction(const Implementation & p_left,
                                   const Implementation & p_right)
  : FunctionImplementation(new ComposedEvaluation(p_left->getEvaluation(), p_right->getEvaluation()),
                           new NoGradient(),
                           new NoHessian())
  , p_leftFunction_(p_left)
  , p_rightFunction_(p_right)
{
  try
  {
    setGradient(new ComposedGradient(p_left->getGradient(), p_right->getEvaluation(), p_right->getGradient()));
    setUseDefaultGradientImplementation(p_left->getUseDefaultGradientImplementation() || p_right->getUseDefaultGradientImplementation());
  }
  catch(InvalidArgumentException &)
  {
    // Nothing to do
  }
  try
  {
    setHessian(new ComposedHessian(p_left->getGradient(), p_left->getHessian(), p_right->getEvaluation(), p_right->getGradient(), p_right->getHessian()));
    setUseDefaultHessianImplementation(p_left->getUseDefaultHessianImplementation() || p_right->getUseDefaultHessianImplementation());
  }
  catch(InvalidArgumentException &)
  {
    // Nothing to do
  }
}

/* Composition constructor */
ComposedFunction::ComposedFunction(const Function & left,
                                   const Function & right)
  : FunctionImplementation(new ComposedEvaluation(left.getEvaluation(), right.getEvaluation()),
                           new NoGradient(),
                           new NoHessian())
  , p_leftFunction_(left.getImplementation())
  , p_rightFunction_(right.getImplementation())
{
  try
  {
    setGradient(new ComposedGradient(left.getGradient(), right.getEvaluation(), right.getGradient()));
    setUseDefaultGradientImplementation(left.getUseDefaultGradientImplementation() || right.getUseDefaultGradientImplementation());
  }
  catch(InvalidArgumentException &)
  {
    // Nothing to do
  }
  try
  {
    setHessian(new ComposedHessian(left.getGradient(), left.getHessian(), right.getEvaluation(), right.getGradient(), right.getHessian()));
    setUseDefaultHessianImplementation(left.getUseDefaultHessianImplementation() || right.getUseDefaultHessianImplementation());
  }
  catch(InvalidArgumentException &)
  {
    // Nothing to do
  }
}

/* Virtual constructor */
ComposedFunction * ComposedFunction::clone() const
{
  return new ComposedFunction(*this);
}

/* Comparison operator */
Bool ComposedFunction::operator ==(const ComposedFunction & ) const
{
  return true;
}

/* String converter */
String ComposedFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedFunction::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " left function=" << p_leftFunction_->__repr__()
      << " right function=" << p_rightFunction_->__repr__();
  return oss;
}

/*
 * Gradient according to the marginal parameters
 *
 * F : RkxRs -> Rn
 *    (y, pf) -> F(y, pf)
 *
 * G : RmxRt -> Rk
 *    (x, pg) -> G(x, pg)
 *
 * Let p = [pg, pf] be the parameter vector of H, with:
 *
 * H : RmxRt+s -> Rn
 *     (x, p)  -> F(G(x, pg), pf)
 *
 * We have:
 *
 * (dH/dp)(x, p) = [(dF/dy)(G(x, pg), pf) . (dG/dpg)(x, pg), 0] + [0, (dF/dpf)(G(x, pg), pf)]
 *
 * the needed gradient is [(dH/dp)(x,p)]^t
 */
Matrix ComposedFunction::parameterGradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  // y = G(x, pg)
  const Point y(p_rightFunction_->operator()(inP));
  // (dG/dpg)(x, pg)
  const Matrix rightGradientP(p_rightFunction_->parameterGradient(inP));
  // (dF/dy)(y, pf)
  const Matrix leftGradientY(p_leftFunction_->gradient(y));
  // (dF/dpf)(G(x, pg), pf)
  const Matrix leftGradientP(p_leftFunction_->parameterGradient(y));
  // (dF/dy)(G(x, pg), pf) . (dG/dpg)(x, pg)
  const Matrix upper(rightGradientP * leftGradientY);
  // Build the full gradient
  const UnsignedInteger rightParametersDimension = upper.getNbRows();
  const UnsignedInteger leftParametersDimension = leftGradientP.getNbRows();
  Matrix grad(rightParametersDimension + leftParametersDimension, outputDimension);
  UnsignedInteger rowIndex = 0;
  // Gradient according to left parameters
  for (UnsignedInteger i = 0; i < rightParametersDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      grad(rowIndex, j) = upper(i, j);
    ++ rowIndex;
  }
  // Gradient according to right parameters
  for (UnsignedInteger i = 0; i < leftParametersDimension; ++i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      grad(rowIndex, j) = leftGradientP(i, j);
    ++ rowIndex;
  }
  return grad;
}

/* Method save() stores the object through the StorageManager */
void ComposedFunction::save(Advocate & adv) const
{
  FunctionImplementation::save(adv);
  adv.saveAttribute( "leftFunction_", *p_leftFunction_ );
  adv.saveAttribute( "rightFunction_", *p_rightFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedFunction::load(Advocate & adv)
{
  TypedInterfaceObject<FunctionImplementation> functionValue;
  FunctionImplementation::load(adv);
  adv.loadAttribute( "leftFunction_", functionValue );
  p_leftFunction_ = functionValue.getImplementation();
  adv.loadAttribute( "rightFunction_", functionValue );
  p_rightFunction_ = functionValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
