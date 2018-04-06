//                                               -*- C++ -*-
/**
 * @brief The class that implements the evaluation of an analytical function.
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
#include <iomanip>

#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SymbolicEvaluation)

static const Factory<SymbolicEvaluation> Factory_SymbolicEvaluation;


/* Default constructor */
SymbolicEvaluation::SymbolicEvaluation()
  : EvaluationImplementation()
  , inputVariablesNames_()
  , outputVariablesNames_()
  , formulas_()
  , parser_()
{
  // Nothing to do
} // SymbolicEvaluation

/* Default constructor */
SymbolicEvaluation::SymbolicEvaluation(const Description & inputVariablesNames,
                                       const Description & outputVariablesNames,
                                       const Description & formulas)
  : EvaluationImplementation()
  , inputVariablesNames_(inputVariablesNames)
  , outputVariablesNames_(outputVariablesNames)
  , formulas_(formulas)
  , parser_()
{
  if (outputVariablesNames.getSize() != formulas.getSize())
    throw InvalidDimensionException(HERE) << "The number of outputVariablesNames (" << outputVariablesNames.getSize()
                                          << ") does not match the number of formulas (" << formulas.getSize() << ")";

  initialize();
} // SymbolicEvaluation

/* Constructor with a single formula which computes output variables */
SymbolicEvaluation::SymbolicEvaluation(const Description & inputVariablesNames,
                                       const Description & outputVariablesNames,
                                       const String & formula)
  : EvaluationImplementation()
  , inputVariablesNames_(inputVariablesNames)
  , outputVariablesNames_(outputVariablesNames)
  , formulas_(Description(1, formula))
  , parser_(outputVariablesNames)
{
  initialize();
}

void SymbolicEvaluation::initialize()
{
  parser_.setVariables(inputVariablesNames_);
  parser_.setFormulas(formulas_);
  setInputDescription(inputVariablesNames_);
  setOutputDescription(outputVariablesNames_);
}

/* Virtual constructor */
SymbolicEvaluation * SymbolicEvaluation::clone() const
{
  return new SymbolicEvaluation(*this);
}


/* Comparison operator */
Bool SymbolicEvaluation::operator ==(const SymbolicEvaluation & other) const
{
  if (this == &other) return true;
  return inputVariablesNames_ == other.inputVariablesNames_ && outputVariablesNames_ == other.outputVariablesNames_ && formulas_ == other.formulas_;
}

/* String converter */
String SymbolicEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SymbolicEvaluation::GetClassName()
      << " name=" << getName()
      << " inputVariablesNames=" << inputVariablesNames_
      << " outputVariablesNames=" << outputVariablesNames_
      << " formulas=" << formulas_;
  return oss;
}

/* String converter */
String SymbolicEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << "->" << formulas_;
  return oss;
}



/* Operator () */
Point SymbolicEvaluation::operator() (const Point & inP) const
{
  Point result(parser_(inP));
  callsNumber_.increment();
  return result;
}

/* Operator () */
Sample SymbolicEvaluation::operator() (const Sample & inS) const
{
  Sample result(parser_(inS));
  callsNumber_.fetchAndAdd(inS.getSize());
  result.setDescription(getOutputDescription());
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger SymbolicEvaluation::getInputDimension() const
{
  return inputVariablesNames_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger SymbolicEvaluation::getOutputDimension() const
{
  return outputVariablesNames_.getSize();
}

/* Get the i-th marginal function */
Evaluation SymbolicEvaluation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return new SymbolicEvaluation(inputVariablesNames_, Description(1, outputVariablesNames_[i]), Description(1, formulas_[i]));
}

/* Get the function corresponding to indices components */
Evaluation SymbolicEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  const UnsignedInteger size = indices.getSize();
  Description marginalOutputVariablesNames(size);
  Description marginalFormulas(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger j = indices[i];
    marginalOutputVariablesNames[i] = outputVariablesNames_[j];
    marginalFormulas[i] = formulas_[j];
  }
  return new SymbolicEvaluation(inputVariablesNames_, marginalOutputVariablesNames, marginalFormulas);
}

/* Accessor to the input variables names */
Description SymbolicEvaluation::getInputVariablesNames() const
{
  return inputVariablesNames_;
}

/* Accessor to the output variables names */
Description SymbolicEvaluation::getOutputVariablesNames() const
{
  return outputVariablesNames_;
}

/* Accessor to the formulas */
Description SymbolicEvaluation::getFormulas() const
{
  return formulas_;
}

/* Method save() stores the object through the StorageManager */
void SymbolicEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.saveAttribute( "outputVariablesNames_", outputVariablesNames_ );
  adv.saveAttribute( "formulas_", formulas_ );
}

/* Method load() reloads the object from the StorageManager */
void SymbolicEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.loadAttribute( "outputVariablesNames_", outputVariablesNames_ );
  adv.loadAttribute( "formulas_", formulas_ );
  initialize();
}

END_NAMESPACE_OPENTURNS
