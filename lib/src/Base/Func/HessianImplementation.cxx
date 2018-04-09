//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all hessian implementations
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/HessianImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicEvaluation.hxx"
#else
#include "openturns/LinearEvaluation.hxx"
#endif
#include "openturns/ConstantGradient.hxx"
#include "openturns/ConstantHessian.hxx"
#include "openturns/ComposedHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HessianImplementation)

static const Factory<HessianImplementation> Factory_HessianImplementation;

/* Default constructor */
HessianImplementation::HessianImplementation()
  : PersistentObject()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Virtual constructor */
HessianImplementation * HessianImplementation::clone() const
{
  return new HessianImplementation(*this);
}

/* Comparison operator */
Bool HessianImplementation::operator ==(const HessianImplementation & other) const
{
  return true;
}

/* String converter */
String HessianImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << HessianImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* String converter */
String HessianImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << "HessianImplementation";
}

/* Test for actual implementation */
Bool HessianImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor HessianImplementation::hessian(const Point & inP) const
{
  throw NotYetImplementedException(HERE) << "In HessianImplementation::hessian(const Point & inP) const";
}

SymmetricTensor HessianImplementation::hessian(const Point & inP,
    const Point & parameters)
{
  LOGWARN("HessianImplementation::gradient(inP,parameters) is deprecated, use setParameter(parameters) and hessian(inP)");
  setParameter(parameters);
  return hessian(inP);
}

/* Accessor for input point dimension */
UnsignedInteger HessianImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In HessianImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger HessianImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In HessianImplementation::getOutputDimension() const";
}

/* Get the number of calls to operator() */
UnsignedInteger HessianImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Parameters value and description accessor */
Point HessianImplementation::getParameter() const
{
  return parameter_;
}

void HessianImplementation::setParameter(const Point & parameter)
{
  parameter_ = parameter;
}


/* Get the i-th marginal function */
Hessian HessianImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Hessian HessianImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  // Here we use the linear algebra representation of the marginal extraction operation in order to extract the marginal hessian.
  // The chain rule gives:
  // D2(Af) = AD2(f) in our case, instead of D2(gof) = D(Dg(f)Df) = (D2g(f)Df)Df + Dg(f)D2f, so we don't need f as in our case Dg(f) = A is a constant, and we don't need D(f) as D2g(f) = 0.
  // As we don't have access to f and Df here but only to D2f, we build an arbitrary cheap evaluation with the proper dimension in order to reuse the
  // generic implementation of the chain rule for the hessians. We choose to build a null function using an analytical function.
  // Fake f
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  const SymbolicEvaluation right(Description::BuildDefault(inputDimension, "x"), Description::BuildDefault(outputDimension, "y"), Description(outputDimension, "0.0"));
#else
  Point center(inputDimension);
  Matrix linear(inputDimension, outputDimension);
  Point constant(outputDimension);
  const LinearEvaluation right(center, constant, linear);
#endif
  // Fake DF
  const ConstantGradient rightGradient(Matrix(inputDimension, outputDimension));
  // Dg = A
  const UnsignedInteger marginalOutputDimension = indices.getSize();
  Matrix gradientExtraction(outputDimension, marginalOutputDimension);
  for (UnsignedInteger i = 0; i < marginalOutputDimension; ++i)
    gradientExtraction(indices[i], i) = 1.0;
  const ConstantGradient leftGradient(gradientExtraction);
  // D2g = 0
  const ConstantHessian leftHessian(SymmetricTensor(outputDimension, marginalOutputDimension));
  return new ComposedHessian(leftGradient.clone(), leftHessian.clone(), right.clone(), rightGradient.clone(), clone());
}

/* Method save() stores the object through the StorageManager */
void HessianImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
  adv.saveAttribute( "parameter_", parameter_ );
}

/* Method load() reloads the object from the StorageManager */
void HessianImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
  adv.loadAttribute( "parameter_", parameter_ );
}


END_NAMESPACE_OPENTURNS
