//                                               -*- C++ -*-
/**
 * @brief The class that implements the evaluation of an analytical function.
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */
#include <iomanip>

#include "AnalyticalNumericalMathEvaluationImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Os.hxx"
#include "OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AnalyticalNumericalMathEvaluationImplementation);

static const Factory<AnalyticalNumericalMathEvaluationImplementation> RegisteredFactory;


/* Default constructor */
AnalyticalNumericalMathEvaluationImplementation::AnalyticalNumericalMathEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , inputVariablesNames_()
  , outputVariablesNames_()
  , formulas_()
{
  // Nothing to do
} // AnalyticalNumericalMathEvaluationImplementation

/* Default constructor */
AnalyticalNumericalMathEvaluationImplementation::AnalyticalNumericalMathEvaluationImplementation(const Description & inputVariablesNames,
    const Description & outputVariablesNames,
    const Description & formulas)
  : NumericalMathEvaluationImplementation()
  , inputVariablesNames_(inputVariablesNames)
  , outputVariablesNames_(outputVariablesNames)
  , formulas_(formulas)
{
  if (outputVariablesNames.getSize() != formulas.getSize())
    throw InvalidDimensionException(HERE) << "The number of outputVariablesNames (" << outputVariablesNames.getSize()
                                          << ") does not match the number of formulas (" << formulas.getSize() << ")";

  parser_.setVariablesFormulas(inputVariablesNames, formulas);
  setInputDescription(inputVariablesNames_);
  setOutputDescription(outputVariablesNames_);
} // AnalyticalNumericalMathEvaluationImplementation


/* Virtual constructor */
AnalyticalNumericalMathEvaluationImplementation * AnalyticalNumericalMathEvaluationImplementation::clone() const
{
  return new AnalyticalNumericalMathEvaluationImplementation(*this);
}


/* Comparison operator */
Bool AnalyticalNumericalMathEvaluationImplementation::operator ==(const AnalyticalNumericalMathEvaluationImplementation & other) const
{
  return true;
}

/* String converter */
String AnalyticalNumericalMathEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << AnalyticalNumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " inputVariablesNames=" << inputVariablesNames_
      << " outputVariablesNames=" << outputVariablesNames_
      << " formulas=" << formulas_;
  return oss;
}

/* String converter */
String AnalyticalNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << "->" << formulas_;
  return oss;
}



/* Operator () */
NumericalPoint AnalyticalNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  NumericalPoint result(parser_(inP));
  ++ callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Operator () */
NumericalSample AnalyticalNumericalMathEvaluationImplementation::operator() (const NumericalSample & inS) const
{
  UnsignedInteger size = inS.getSize();
  NumericalSample outSample(size, getOutputDimension());
  for (UnsignedInteger i = 0; i < size; ++ i) outSample[i] = operator()(inS[i]);
  outSample.setDescription(getOutputDescription());
  return outSample;
}

/* Accessor for input point dimension */
UnsignedInteger AnalyticalNumericalMathEvaluationImplementation::getInputDimension() const
{
  return inputVariablesNames_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger AnalyticalNumericalMathEvaluationImplementation::getOutputDimension() const
{
  return outputVariablesNames_.getSize();
}

/* Get the i-th marginal function */
AnalyticalNumericalMathEvaluationImplementation::Implementation AnalyticalNumericalMathEvaluationImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new AnalyticalNumericalMathEvaluationImplementation(inputVariablesNames_, Description(1, outputVariablesNames_[i]), Description(1, formulas_[i]));
}

/* Get the function corresponding to indices components */
AnalyticalNumericalMathEvaluationImplementation::Implementation AnalyticalNumericalMathEvaluationImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension() - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and  must be different";
  const UnsignedInteger size(indices.getSize());
  Description marginalOutputVariablesNames(size);
  Description marginalFormulas(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger j(indices[i]);
    marginalOutputVariablesNames[i] = outputVariablesNames_[j];
    marginalFormulas[i] = formulas_[j];
  }
  return new AnalyticalNumericalMathEvaluationImplementation(inputVariablesNames_, marginalOutputVariablesNames, marginalFormulas);
}

/* Accessor to the input variables names */
Description AnalyticalNumericalMathEvaluationImplementation::getInputVariablesNames() const
{
  return inputVariablesNames_;
}

/* Accessor to the output variables names */
Description AnalyticalNumericalMathEvaluationImplementation::getOutputVariablesNames() const
{
  return outputVariablesNames_;
}

/* Accessor to the formulas */
Description AnalyticalNumericalMathEvaluationImplementation::getFormulas() const
{
  return formulas_;
}

/* Method save() stores the object through the StorageManager */
void AnalyticalNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.saveAttribute( "outputVariablesNames_", outputVariablesNames_ );
  adv.saveAttribute( "formulas_", formulas_ );
}

/* Method load() reloads the object from the StorageManager */
void AnalyticalNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.loadAttribute( "outputVariablesNames_", outputVariablesNames_ );
  adv.loadAttribute( "formulas_", formulas_ );
  *this = AnalyticalNumericalMathEvaluationImplementation(inputVariablesNames_, outputVariablesNames_, formulas_);
}

END_NAMESPACE_OPENTURNS
