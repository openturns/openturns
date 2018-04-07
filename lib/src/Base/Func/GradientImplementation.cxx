//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all gradient implementations
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
#include "openturns/GradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicEvaluation.hxx"
#else
#include "openturns/LinearEvaluation.hxx"
#endif
#include "openturns/ConstantGradient.hxx"
#include "openturns/ComposedGradient.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GradientImplementation)

static const Factory<GradientImplementation> Factory_GradientImplementation;

/* Default constructor */
GradientImplementation::GradientImplementation()
  : PersistentObject()
  , callsNumber_(0)
  , parameter_(0)
{
  // Nothing to do
}

/* Virtual constructor */
GradientImplementation * GradientImplementation::clone() const
{
  return new GradientImplementation(*this);
}

/* Comparison operator */
Bool GradientImplementation::operator ==(const GradientImplementation & other) const
{
  return true;
}

/* String converter */
String GradientImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GradientImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* String converter */
String GradientImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << "GradientImplementation";
}


/* Test for actual implementation */
Bool GradientImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

/* Gradient method */
Matrix GradientImplementation::gradient(const Point & inP) const
{
  throw NotYetImplementedException(HERE) << "In GradientImplementation::gradient(const Point & inP) const";
}

/* Gradient method */
Matrix GradientImplementation::gradient (const Point & inP,
    const Point & parameter)
{
  LOGWARN("GradientImplementation::gradient(inP,parameter) is deprecated, use setParameter(parameter) and gradient(inP)");
  setParameter(parameter);
  return gradient(inP);
}

/* Accessor for input point dimension */
UnsignedInteger GradientImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In GradientImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger GradientImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In GradientImplementation::getOutputDimension() const";
}

/* Get the number of calls to operator() */
UnsignedInteger GradientImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Parameters value and description accessor */
Point GradientImplementation::getParameter() const
{
  return parameter_;
}

void GradientImplementation::setParameter(const Point & parameter)
{
  parameter_ = parameter;
}

/* Get the i-th marginal function */
Gradient GradientImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Gradient GradientImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  // Here we use the linear algebra representation of the marginal extraction operation in order to extract the marginal gradient.
  // The chain rule gives:
  // D(Af) = AD(f) in our case, instead of D(gof) = Dg(f)Df, so we don't need f as in our case Dg(f) = A is a constant. As we don't
  // have access to f here but only to Df, we build an arbitrary cheap evaluation with the proper dimension in order to reuse the
  // generic implementation of the chain rule for the gradients. We choose to build a null function using an analytical function.
  // Fake f
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  Description input(inputDimension);
  for (UnsignedInteger index = 0; index < inputDimension; ++index)
    input[index] = OSS() << "x" << index;
  Description output(outputDimension);
  for (UnsignedInteger index = 0; index < outputDimension; ++index)
    output[index] = OSS() << "y" << index;
  const Description formulas(outputDimension, "0.0");
  const SymbolicEvaluation right(input, output, formulas);
#else
  Point center(inputDimension);
  Matrix linear(inputDimension, outputDimension);
  Point constant(outputDimension);
  const LinearEvaluation right(center, constant, linear);
#endif
  // A
  const UnsignedInteger marginalOutputDimension = indices.getSize();
  Matrix gradientExtraction(outputDimension, marginalOutputDimension);
  for (UnsignedInteger i = 0; i < marginalOutputDimension; ++i)
    gradientExtraction(indices[i], i) = 1.0;
  const ConstantGradient leftGradient(gradientExtraction);
  return new ComposedGradient(leftGradient.clone(), right.clone(), clone());
}

/* Method save() stores the object through the StorageManager */
void GradientImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
  adv.saveAttribute( "parameter_", parameter_ );
}

/* Method load() reloads the object from the StorageManager */
void GradientImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
  adv.loadAttribute( "parameter_", parameter_ );
}

END_NAMESPACE_OPENTURNS
