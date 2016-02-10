//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "InverseTrendEvaluationImplementation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseTrendEvaluationImplementation);

static const Factory<InverseTrendEvaluationImplementation> RegisteredFactory;

/* Default constructor */
InverseTrendEvaluationImplementation::InverseTrendEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseTrendEvaluationImplementation::InverseTrendEvaluationImplementation(const NumericalMathFunction & function)
  : NumericalMathEvaluationImplementation()
  , function_(function)
{
  Description inputDescription(function.getInputDescription());
  const Description outputDescription(function.getOutputDescription());
  const UnsignedInteger outputDimension(outputDescription.getSize());
  const Description otherInputDescription(Description::BuildDefault(outputDimension, "x"));
  for (UnsignedInteger i = 0; i < outputDimension; ++i) inputDescription.add(otherInputDescription[i]);
  setInputDescription(inputDescription);
  setOutputDescription(outputDescription);
}

/* Clone constructor */
InverseTrendEvaluationImplementation * InverseTrendEvaluationImplementation::clone() const
{
  return new InverseTrendEvaluationImplementation(*this);
}

/* Comparison operator */
Bool InverseTrendEvaluationImplementation::operator ==(const InverseTrendEvaluationImplementation & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_);
}

/* String converter */
String InverseTrendEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseTrendEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " function=" << function_.__repr__();
  return oss;
}

/* String converter __str__ */
String InverseTrendEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << function_.__str__(offset);
  return oss;
}

/* Accessor for the function */
NumericalMathFunction InverseTrendEvaluationImplementation::getFunction() const
{
  return function_;
}

/* Operator () */
NumericalPoint InverseTrendEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  UnsignedInteger outputDimension(getOutputDimension());
  NumericalPoint result(outputDimension);
  const UnsignedInteger reducedInputDimension(function_.getInputDimension());
  NumericalPoint t(reducedInputDimension);
  for (UnsignedInteger i = 0; i < reducedInputDimension; ++i) t[i] = inP[i];
  const NumericalPoint z(function_(t));
  for (UnsignedInteger i = 0; i < outputDimension; ++i) result[i] = inP[i + reducedInputDimension] - z[i];
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseTrendEvaluationImplementation::getInputDimension() const
{
  return function_.getInputDimension() + function_.getOutputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseTrendEvaluationImplementation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void InverseTrendEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseTrendEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
}

END_NAMESPACE_OPENTURNS
