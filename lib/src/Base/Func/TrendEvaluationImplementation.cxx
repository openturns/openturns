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

#include "openturns/TrendEvaluationImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrendEvaluationImplementation);

static const Factory<TrendEvaluationImplementation> Factory_TrendEvaluationImplementation;

/* Default constructor */
TrendEvaluationImplementation::TrendEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
TrendEvaluationImplementation::TrendEvaluationImplementation(const NumericalMathFunction & function)
  : NumericalMathEvaluationImplementation()
  , function_(function)
{
  Description inputDescription(function.getInputDescription());
  const Description outputDescription(function.getOutputDescription());
  const UnsignedInteger outputDimension = outputDescription.getSize();
  const Description otherInputDescription(Description::BuildDefault(outputDimension, "x"));
  for (UnsignedInteger i = 0; i < outputDimension; ++i) inputDescription.add(otherInputDescription[i]);
  setInputDescription(inputDescription);
  setOutputDescription(outputDescription);
}

/* Clone constructor */
TrendEvaluationImplementation * TrendEvaluationImplementation::clone() const
{
  return new TrendEvaluationImplementation(*this);
}

/* Comparison operator */
Bool TrendEvaluationImplementation::operator ==(const TrendEvaluationImplementation & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_);
}

/* String converter */
String TrendEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TrendEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " function=" << function_.__repr__();
  return oss;
}

/* String converter __str__ */
String TrendEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << function_.__str__(offset);
  return oss;
}

/* Accessor for the function */
NumericalMathFunction TrendEvaluationImplementation::getFunction() const
{
  return function_;
}

/* Operator () */
NumericalPoint TrendEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  UnsignedInteger outputDimension = getOutputDimension();
  NumericalPoint result(outputDimension);
  const UnsignedInteger reducedInputDimension = function_.getInputDimension();
  NumericalPoint t(reducedInputDimension);
  for (UnsignedInteger i = 0; i < reducedInputDimension; ++i) t[i] = inP[i];
  const NumericalPoint z(function_(t));
  for (UnsignedInteger i = 0; i < outputDimension; ++i) result[i] = inP[i + reducedInputDimension] + z[i];
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger TrendEvaluationImplementation::getInputDimension() const
{
  return function_.getInputDimension() + function_.getOutputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger TrendEvaluationImplementation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void TrendEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Method load() reloads the object from the StorageManager */
void TrendEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
}

END_NAMESPACE_OPENTURNS
