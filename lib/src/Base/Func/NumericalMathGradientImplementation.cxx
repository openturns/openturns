//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/AnalyticalNumericalMathEvaluationImplementation.hxx"
#else
#include "openturns/LinearNumericalMathEvaluationImplementation.hxx"
#endif
#include "openturns/ConstantNumericalMathGradientImplementation.hxx"
#include "openturns/ComposedNumericalMathGradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NumericalMathGradientImplementation);

static const Factory<NumericalMathGradientImplementation> RegisteredFactory;

/* Default constructor */
NumericalMathGradientImplementation::NumericalMathGradientImplementation()
  : PersistentObject()
  , callsNumber_(0)
  , parameters_(0)
{
  // Nothing to do
}

/* Virtual constructor */
NumericalMathGradientImplementation * NumericalMathGradientImplementation::clone() const
{
  return new NumericalMathGradientImplementation(*this);
}

/* Comparison operator */
Bool NumericalMathGradientImplementation::operator ==(const NumericalMathGradientImplementation & other) const
{
  return true;
}

/* String converter */
String NumericalMathGradientImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NumericalMathGradientImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* String converter */
String NumericalMathGradientImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << "NumericalMathGradientImplementation";
}


/* Test for actual implementation */
Bool NumericalMathGradientImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

/* Gradient method */
Matrix NumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  throw NotYetImplementedException(HERE) << "In NumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const";
}

/* Gradient method */
Matrix NumericalMathGradientImplementation::gradient(const NumericalPoint & inP,
    const NumericalPoint & parameters)
{
  setParameter(parameters);
  return gradient(inP);
}

/* Accessor for input point dimension */
UnsignedInteger NumericalMathGradientImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In NumericalMathGradientImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger NumericalMathGradientImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In NumericalMathGradientImplementation::getOutputDimension() const";
}

/* Get the number of calls to operator() */
UnsignedInteger NumericalMathGradientImplementation::getCallsNumber() const
{
  return callsNumber_;
}

/* Parameters value and description accessor */
NumericalPointWithDescription NumericalMathGradientImplementation::getParameter() const
{
  return parameters_;
}

void NumericalMathGradientImplementation::setParameter(const NumericalPointWithDescription & parameters)
{
  parameters_ = parameters;
}

/* Get the i-th marginal function */
NumericalMathGradientImplementation::Implementation NumericalMathGradientImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
NumericalMathGradientImplementation::Implementation NumericalMathGradientImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension() - 1)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and  must be different";
  // Here we use the linear algebra representation of the marginal extraction operation in order to extract the marginal gradient.
  // The chain rule gives:
  // D(Af) = AD(f) in our case, instead of D(gof) = Dg(f)Df, so we don't need f as in our case Dg(f) = A is a constant. As we don't
  // have access to f here but only to Df, we build an arbitrary cheap evaluation with the proper dimension in order to reuse the
  // generic implementation of the chain rule for the gradients. We choose to build a null function using an analytical function.
  // Fake f
  const UnsignedInteger inputDimension = getInputDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
#ifdef OPENTURNS_HAVE_MUPARSER
  Description input(inputDimension);
  for (UnsignedInteger index = 0; index < inputDimension; ++index)
    input[index] = OSS() << "x" << index;
  Description output(outputDimension);
  for (UnsignedInteger index = 0; index < outputDimension; ++index)
    output[index] = OSS() << "y" << index;
  const Description formulas(outputDimension, "0.0");
  const AnalyticalNumericalMathEvaluationImplementation right(input, output, formulas);
#else
  NumericalPoint center(inputDimension);
  Matrix linear(inputDimension, outputDimension);
  NumericalPoint constant(outputDimension);
  const LinearNumericalMathEvaluationImplementation right(center, constant, linear);
#endif
  // A
  const UnsignedInteger marginalOutputDimension = indices.getSize();
  Matrix gradientExtraction(outputDimension, marginalOutputDimension);
  for (UnsignedInteger i = 0; i < marginalOutputDimension; ++i)
    gradientExtraction(indices[i], i) = 1.0;
  const ConstantNumericalMathGradientImplementation leftGradient(gradientExtraction);
  return new ComposedNumericalMathGradientImplementation(leftGradient.clone(), right.clone(), clone());
}

/* Method save() stores the object through the StorageManager */
void NumericalMathGradientImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", callsNumber_ );
}

/* Method load() reloads the object from the StorageManager */
void NumericalMathGradientImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "callsNumber_", callsNumber_ );
}

END_NAMESPACE_OPENTURNS
