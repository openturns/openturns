//                                               -*- C++ -*-
/**
 *  @brief The class that implements the evaluation of an analytical function.
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iomanip>

#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/OTconfig.hxx"
#include "Ev3/expression.h"
#include "Ev3/parser.h"

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
  setOutputDescription(outputVariablesNames);
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
  setOutputDescription(outputVariablesNames);
  initialize();
}

void SymbolicEvaluation::initialize()
{
  parser_.setVariables(inputVariablesNames_);
  parser_.setFormulas(formulas_);
  setInputDescription(inputVariablesNames_);
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
String SymbolicEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << getInputDescription() << "->" << formulas_;
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

/* Linearity accessors */
Bool SymbolicEvaluation::isLinear() const
{
  const UnsignedInteger inputSize = inputVariablesNames_.getSize();
  const UnsignedInteger outputSize = outputVariablesNames_.getSize();

  // SymbolicEvaluation is linear if all its marginals are
  for (UnsignedInteger columnIndex = 0; columnIndex < outputSize; ++columnIndex)
  {
    // Parse the current formula with Ev3
    int nerr(0);
    Ev3::ExpressionParser ev3Parser;

    // Initialize the variable indices in order to match the order of OpenTURNS in Ev3
    for (UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputSize; ++inputVariableIndex)
      ev3Parser.SetVariableID(inputVariablesNames_[inputVariableIndex],
                              inputVariableIndex);

    Ev3::Expression ev3Expression;

    try
    {
      ev3Expression = ev3Parser.Parse(formulas_[columnIndex].c_str(), nerr);
    }
    catch (Ev3::ErrBase & exc)
    {
      throw InternalException(HERE) << exc.description_;
    }

    if (nerr != 0)
      return false;

    if (!ev3Expression->IsLinear())
      return false;
  }

  return true;
}

Bool SymbolicEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  const UnsignedInteger inputSize = inputVariablesNames_.getSize();
  const UnsignedInteger outputSize = outputVariablesNames_.getSize();

  // Check index consistency
  if (index >= inputSize)
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function input dimension (" << inputSize << ")";

  // Function depends linearly on variable i if all its marginals do
  for (UnsignedInteger columnIndex = 0; columnIndex < outputSize; ++columnIndex)
  {
    // Parse the current formula with Ev3
    int nerr(0);
    Ev3::ExpressionParser ev3Parser;

    // Initialize the variable indices in order to match the order of OpenTURNS in Ev3
    for (UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputSize; ++inputVariableIndex) ev3Parser.SetVariableID(inputVariablesNames_[inputVariableIndex],
          inputVariableIndex);
    Ev3::Expression ev3Expression;

    try
    {
      ev3Expression = ev3Parser.Parse(formulas_[columnIndex].c_str(), nerr);
    }
    catch (Ev3::ErrBase & exc)
    {
      throw InternalException(HERE) << exc.description_;
    }

    if (nerr != 0)
      return false;

    if (ev3Expression->DependsLinearlyOnVariable(index) == 0)
      return false;
  }

  return true;
}

/* Invalid values check accessor */
void SymbolicEvaluation::setCheckOutput(const Bool checkOutput)
{
  EvaluationImplementation::setCheckOutput(checkOutput);
  parser_.setCheckOutput(checkOutput);
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
