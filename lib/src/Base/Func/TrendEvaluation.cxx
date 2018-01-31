//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/TrendEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TrendEvaluation)

static const Factory<TrendEvaluation> Factory_TrendEvaluation;

/* Default constructor */
TrendEvaluation::TrendEvaluation()
  : EvaluationImplementation()
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
TrendEvaluation::TrendEvaluation(const Function & function)
  : EvaluationImplementation()
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
TrendEvaluation * TrendEvaluation::clone() const
{
  return new TrendEvaluation(*this);
}

/* Comparison operator */
Bool TrendEvaluation::operator ==(const TrendEvaluation & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_);
}

/* String converter */
String TrendEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TrendEvaluation::GetClassName()
      << " name=" << getName()
      << " function=" << function_.__repr__();
  return oss;
}

/* String converter __str__ */
String TrendEvaluation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << function_.__str__(offset);
  return oss;
}

/* Accessor for the function */
Function TrendEvaluation::getFunction() const
{
  return function_;
}

/* Operator () */
Point TrendEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const UnsignedInteger outputDimension = getOutputDimension();
  Point result(outputDimension);
  const UnsignedInteger reducedInputDimension = function_.getInputDimension();
  Point t(reducedInputDimension);
  for (UnsignedInteger i = 0; i < reducedInputDimension; ++i) t[i] = inP[i];
  const Point z(function_(t));
  for (UnsignedInteger i = 0; i < outputDimension; ++i) result[i] = inP[i + reducedInputDimension] + z[i];
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

Sample TrendEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const UnsignedInteger outputDimension = getOutputDimension();
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger reducedInputDimension = function_.getInputDimension();
  Indices inputIndices(reducedInputDimension);
  inputIndices.fill();
  Indices remainingIndices(inputDimension - reducedInputDimension);
  remainingIndices.fill(reducedInputDimension);
  Sample result(inSample.getMarginal(remainingIndices));
  result += function_(inSample.getMarginal(inputIndices));
  result.setDescription(getOutputDescription());
  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inSample);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger TrendEvaluation::getInputDimension() const
{
  return function_.getInputDimension() + function_.getOutputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger TrendEvaluation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void TrendEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Method load() reloads the object from the StorageManager */
void TrendEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
}

END_NAMESPACE_OPENTURNS
