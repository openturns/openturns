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

static Factory<AnalyticalNumericalMathEvaluationImplementation> RegisteredFactory("AnalyticalNumericalMathEvaluationImplementation");


/* Default constructor */
AnalyticalNumericalMathEvaluationImplementation::AnalyticalNumericalMathEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , isInitialized_(false)
  , inputVariables_(0, 0)
  , inputVariablesNames_()
  , outputVariablesNames_()
  , formulas_()
  , parsers_(0)
{
  // Nothing to do
} // AnalyticalNumericalMathEvaluationImplementation

/* Default constructor */
AnalyticalNumericalMathEvaluationImplementation::AnalyticalNumericalMathEvaluationImplementation(const Description & inputVariablesNames,
    const Description & outputVariablesNames,
    const Description & formulas)
  : NumericalMathEvaluationImplementation()
  , isInitialized_(false)
  , inputVariables_(inputVariablesNames.getSize(), 0)
  , inputVariablesNames_(inputVariablesNames)
  , outputVariablesNames_(outputVariablesNames)
  , formulas_(formulas)
  , parsers_(ParserCollection(outputVariablesNames_.getSize()))
{
  if (outputVariablesNames.getSize() != formulas.getSize())
    throw InvalidDimensionException(HERE) << "The number of outputVariablesNames (" << outputVariablesNames.getSize()
                                          << ") does not match the number of formulas (" << formulas.getSize() << ")";

  // Call the initialize() method in order to check the parameters w.r.t. muParser
  initialize(1);
  setInputDescription(inputVariablesNames_);
  setOutputDescription(outputVariablesNames_);
} // AnalyticalNumericalMathEvaluationImplementation


/* Virtual constructor */
AnalyticalNumericalMathEvaluationImplementation * AnalyticalNumericalMathEvaluationImplementation::clone() const
{
  AnalyticalNumericalMathEvaluationImplementation * result = new AnalyticalNumericalMathEvaluationImplementation(*this);
  result->isInitialized_ = false;
  return result;
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

/* Must initialize the parser at the first call to operator() as the
   reference associated with the variables may have change after the construction.
   Initializing is necessary when the arguments size is different between two calls to operator().
   inputSize is the number of values of each input variables.
   So, if operator() method has a NumericalPoint as argument, inputSize is 1*/
void AnalyticalNumericalMathEvaluationImplementation::initialize(UnsignedInteger inputSize) const
{
  if (inputSize > inputVariables_.getDimension()) isInitialized_ = false;
  if (isInitialized_) return;
  const UnsignedInteger inputDimension(inputVariablesNames_.getSize());
  const UnsignedInteger outputSize(outputVariablesNames_.getSize());
  try
  {
    // inputVariables_ is the transposed sample (resp. NumericalPoint) of the operator() method sample
    // (resp. NumericalPoint) argument.
    inputVariables_ = NumericalSample(inputDimension, inputSize);
    // For each parser of a formula, do
    for (UnsignedInteger outputVariableIndex = 0; outputVariableIndex < outputSize; ++outputVariableIndex)
    {
      // First, define all the variable names and associate them
      // to the corresponding component of the input vector
      for(UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputDimension; ++inputVariableIndex)
      {
        // DefineVar defines all the values given to variables
        parsers_[outputVariableIndex].DefineVar(inputVariablesNames_[inputVariableIndex].c_str(), &inputVariables_[inputVariableIndex][0]);
      }
    }

    // For each parser of a formula, do
    for (UnsignedInteger outputVariableIndex = 0; outputVariableIndex < outputSize; ++outputVariableIndex)
    {
      // Second, define the several formulas
      parsers_[outputVariableIndex].SetExpr(formulas_[outputVariableIndex].c_str());
    }
  }
  catch(mu::Parser::exception_type & ex)
  {
    throw InvalidArgumentException(HERE) << "Error constructing an analytical function, message=" << ex.GetMsg() << " formula=" << ex.GetExpr() << " token=" << ex.GetToken() << " position=" << ex.GetPos();
  }
  isInitialized_ = true;
}

/* Operator () */
NumericalPoint AnalyticalNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a NumericalMathFunction with an argument of invalid dimension";
  initialize(1);
  NumericalPoint result(getOutputDimension());
  ++ callsNumber_;
  for (UnsignedInteger i = 0; i < inP.getDimension(); ++i) inputVariables_[i][0] = inP[i];

  try
  {
    for (UnsignedInteger index = 0; index < result.getDimension(); ++index) result[index] = parsers_[index].Eval();
  }
  catch(mu::Parser::exception_type & ex)
  {
    throw InternalException(HERE) << ex.GetMsg();
  }
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
  const UnsignedInteger inputDimension(getInputDimension());
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  const UnsignedInteger size(inS.getSize());
  NumericalSample outSample(size, getOutputDimension());
#ifdef OPENTURNS_HAVE_MUPARSER2
  initialize(size);
  callsNumber_ += size;
  for(UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputDimension; ++inputVariableIndex)
  {
    for (UnsignedInteger i = 0; i < size; ++i) inputVariables_[inputVariableIndex][i] = inS[i][inputVariableIndex];
  }
  NumericalPoint evalResult(size);
  for (UnsignedInteger outputVariableIndex = 0; outputVariableIndex < getOutputDimension(); ++outputVariableIndex)
  {
    parsers_[outputVariableIndex].Eval(&evalResult[0], size);
    for (UnsignedInteger i = 0; i < size; ++i) outSample[i][outputVariableIndex] = evalResult[i];
  }
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(outSample);
  }
#else
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i] = operator()(inS[i]);
#endif
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
