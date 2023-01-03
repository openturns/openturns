//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for product functions
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
#include "openturns/ProductFunction.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"
#include "openturns/ProductEvaluation.hxx"
#include "openturns/ProductGradient.hxx"
#include "openturns/ProductHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductFunction)

static const Factory<ProductFunction> Factory_ProductFunction;

/* Composition constructor */
ProductFunction::ProductFunction(const Implementation & p_left,
                                 const Implementation & p_right)
  : FunctionImplementation(new ProductEvaluation(p_left->getEvaluation(), p_right->getEvaluation()),
                           new NoGradient(),
                           new NoHessian()),
    p_leftFunction_(p_left),
    p_rightFunction_(p_right)
{
  //  try{
  setGradient(new ProductGradient(p_leftFunction_->getEvaluation(), p_leftFunction_->getGradient(), p_rightFunction_->getEvaluation(), p_rightFunction_->getGradient()));
  //  }
  //  catch (const InvalidArgumentException &) {
  // Nothing to do
  //  }
  //  try{
  setHessian(new ProductHessian(p_left->getEvaluation(), p_left->getGradient(), p_left->getHessian(), p_right->getEvaluation(), p_right->getGradient(), p_right->getHessian()));
  //  }
  //  catch (const InvalidArgumentException & ex) {
  // Nothing to do
  //  }
}

/* Virtual constructor */
ProductFunction * ProductFunction::clone() const
{
  return new ProductFunction(*this);
}

/* Comparison operator */
Bool ProductFunction::operator ==(const ProductFunction & ) const
{
  return true;
}

/* String converter */
String ProductFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ProductFunction::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " left function=" << p_leftFunction_->__repr__()
      << " right function=" << p_rightFunction_->__repr__();
  return oss;
}

String ProductFunction::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset
      << "class=" << ProductFunction::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " left function=" << p_leftFunction_->__str__()
      << " right function=" << p_rightFunction_->__str__();
  return oss;
}

/*
 * Gradient according to the marginal parameters
 * H(x, p) = F(x, pf) . G(x, pg)
 * dH/dp = dF/dp(x, pf) . G(x, pg) + F(x, pf) . dG/dp(x, pg)
 * with
 * p = [pf, pg], dF/dp = [dF/dpf, 0], dG/dp = [0, dG/dpg]
 * thus
 * dH/dp = [dF/dpf(x, pf) . G(x, pg), dG/dpg(x, pg) . F(x, pf)]
 * and the needed gradient is (dH/dp)^t
 */
Matrix ProductFunction::parameterGradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  // Values of the functions
  Scalar leftValue = p_leftFunction_->operator()(inP)[0];
  Scalar rightValue = p_rightFunction_->operator()(inP)[0];
  // Parameters gradient of the functions scaled by the value of there product term
  Matrix upper(p_leftFunction_->parameterGradient(inP) * leftValue);
  Matrix lower(p_rightFunction_->parameterGradient(inP) * rightValue);
  // Fill-in the result
  UnsignedInteger leftParametersDimension = upper.getNbRows();
  UnsignedInteger rightParametersDimension = lower.getNbRows();
  Matrix grad(rightParametersDimension + leftParametersDimension, 1);
  UnsignedInteger rowIndex = 0;
  // Gradient according to left parameters
  for (UnsignedInteger i = 0; i < leftParametersDimension; ++i)
  {
    grad(rowIndex, 0) = upper(i, 0);
    ++ rowIndex;
  }
  // Gradient according to right parameters
  for (UnsignedInteger i = 0; i < leftParametersDimension; ++i)
  {
    grad(rowIndex, 0) = lower(i, 0);
    ++ rowIndex;
  }
  return grad;
}

/* Method save() stores the object through the StorageManager */
void ProductFunction::save(Advocate & adv) const
{
  FunctionImplementation::save(adv);
  adv.saveAttribute( "leftFunction_", *p_leftFunction_ );
  adv.saveAttribute( "rightFunction_", *p_rightFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductFunction::load(Advocate & adv)
{
  TypedInterfaceObject<FunctionImplementation> functionValue;
  FunctionImplementation::load(adv);
  adv.loadAttribute( "leftFunction_", functionValue );
  p_leftFunction_ = functionValue.getImplementation();
  adv.loadAttribute( "rightFunction_", functionValue );
  p_rightFunction_ = functionValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
