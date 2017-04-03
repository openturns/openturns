//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/InverseTrendEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseTrendEvaluation);

static const Factory<InverseTrendEvaluation> Factory_InverseTrendEvaluation;

/* Default constructor */
InverseTrendEvaluation::InverseTrendEvaluation()
  : EvaluationImplementation()
  , function_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseTrendEvaluation::InverseTrendEvaluation(const Function & function)
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
InverseTrendEvaluation * InverseTrendEvaluation::clone() const
{
  return new InverseTrendEvaluation(*this);
}

/* Comparison operator */
Bool InverseTrendEvaluation::operator ==(const InverseTrendEvaluation & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_);
}

/* String converter */
String InverseTrendEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseTrendEvaluation::GetClassName()
      << " name=" << getName()
      << " function=" << function_.__repr__();
  return oss;
}

/* String converter __str__ */
String InverseTrendEvaluation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << function_.__str__(offset);
  return oss;
}

/* Accessor for the function */
Function InverseTrendEvaluation::getFunction() const
{
  return function_;
}

/* Operator () */
Point InverseTrendEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  UnsignedInteger outputDimension = getOutputDimension();
  Point result(outputDimension);
  const UnsignedInteger reducedInputDimension = function_.getInputDimension();
  Point t(reducedInputDimension);
  for (UnsignedInteger i = 0; i < reducedInputDimension; ++i) t[i] = inP[i];
  const Point z(function_(t));
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
UnsignedInteger InverseTrendEvaluation::getInputDimension() const
{
  return function_.getInputDimension() + function_.getOutputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseTrendEvaluation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void InverseTrendEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "function_", function_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseTrendEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "function_", function_ );
}

END_NAMESPACE_OPENTURNS
