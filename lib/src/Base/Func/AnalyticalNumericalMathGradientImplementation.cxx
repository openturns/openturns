//                                               -*- C++ -*-
/**
 * @brief The class that implements the gradient of an analytical function.
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

#include "openturns/AnalyticalNumericalMathGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "Ev3/expression.h"
#include "Ev3/parser.h"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(AnalyticalNumericalMathGradientImplementation);

static const Factory<AnalyticalNumericalMathGradientImplementation> RegisteredFactory;

/* Default constructor */
AnalyticalNumericalMathGradientImplementation::AnalyticalNumericalMathGradientImplementation()
  : NumericalMathGradientImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , evaluation_()
{
  // Nothing to do
} // AnalyticalNumericalMathGradientImplementation

/* Default constructor */
AnalyticalNumericalMathGradientImplementation::AnalyticalNumericalMathGradientImplementation(const AnalyticalNumericalMathEvaluationImplementation & evaluation)
  : NumericalMathGradientImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , evaluation_(evaluation)
{
  // Nothing to do
} // AnalyticalNumericalMathGradientImplementation


/* Virtual constructor */
AnalyticalNumericalMathGradientImplementation * AnalyticalNumericalMathGradientImplementation::clone() const
{
  AnalyticalNumericalMathGradientImplementation * result = new AnalyticalNumericalMathGradientImplementation(*this);
  result->isInitialized_ = false;
  return result;
}


/* Comparison operator */
Bool AnalyticalNumericalMathGradientImplementation::operator ==(const AnalyticalNumericalMathGradientImplementation & other) const
{
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String AnalyticalNumericalMathGradientImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << AnalyticalNumericalMathGradientImplementation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter */
String AnalyticalNumericalMathGradientImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset;
  // Check that the symbolic differentiation has been done
  try
  {
    initialize();
    if (isAnalytical_)
    {
      oss << "\n";
      // First, find the maximum length of the output variable names
      const Description inputVariablesNames(evaluation_.getInputVariablesNames());
      const Description outputVariablesNames(evaluation_.getOutputVariablesNames());
      UnsignedInteger length(0);
      const UnsignedInteger iMax(getInputDimension());
      const UnsignedInteger jMax(getOutputDimension());
      for (UnsignedInteger i = 0; i < iMax; ++i)
      {
        const UnsignedInteger lengthI(inputVariablesNames[i].length());
        for (UnsignedInteger j = 0; j < jMax; ++j)
          length = std::max(length, lengthI + static_cast<UnsignedInteger>(outputVariablesNames[j].length()) + 8);
      }
      for (UnsignedInteger j = 0; j < jMax; ++j)
      {
        for (UnsignedInteger i = 0; i < iMax; ++i)
        {
          oss << offset << "| " << std::setw(length) << ("d(" + outputVariablesNames[j] + ") / d(" + inputVariablesNames[i] + ")") << " = " << getFormula(i, j) << Os::GetEndOfLine();
        }
      }
    } // isAnalytical
  }
  catch(...)
  {
    // Nothing to do
  }
  if (!isAnalytical_) oss << offset << "No analytical gradient available. Try using finite difference instead.";
  return oss;
}

/* Must initialize the parser at the first call to operator() as the
   reference associated with the variables may have change after the construction */
void AnalyticalNumericalMathGradientImplementation::initialize() const
{
  if (isInitialized_) return;
  isAnalytical_ = false;
  const UnsignedInteger inputSize(evaluation_.inputVariablesNames_.getSize());
  const UnsignedInteger outputSize(evaluation_.outputVariablesNames_.getSize());
  const UnsignedInteger gradientSize(inputSize * outputSize);
  Description gradientFormulas(gradientSize);
  // For each element of the gradient, do
  UnsignedInteger gradientIndex(0);
  for (UnsignedInteger columnIndex = 0; columnIndex < outputSize; ++columnIndex)
  {
    // Parse the current formula with Ev3
    int nerr(0);
    Ev3::ExpressionParser ev3Parser;
    // Initialize the variable indices in order to match the order of OpenTURNS in Ev3
    for (UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputSize; ++inputVariableIndex) ev3Parser.SetVariableID(evaluation_.inputVariablesNames_[inputVariableIndex], inputVariableIndex);
    Ev3::Expression ev3Expression;
    try
    {
      ev3Expression = ev3Parser.Parse(evaluation_.formulas_[columnIndex].c_str(), nerr);
    }
    catch (Ev3::ErrBase & exc)
    {
      throw InternalException(HERE) << exc.description_;
    }
    if (nerr != 0) throw InvalidArgumentException(HERE) << "Error: cannot parse " << evaluation_.formulas_[columnIndex] << " with Ev3. No analytical gradient.";
    //                Ev3::Simplify(&ev3Expression);
    for (UnsignedInteger rowIndex = 0; rowIndex < inputSize; ++rowIndex)
    {
      try
      {
        Ev3::Expression derivative(Ev3::Diff(ev3Expression, rowIndex));
        //                    Ev3::Simplify(&derivative);
        LOGINFO(OSS() << "d(" << ev3Expression->ToString() << ")/d(" << evaluation_.inputVariablesNames_[rowIndex] << ")=" << derivative->ToString());
        gradientFormulas[gradientIndex] = derivative->ToString();
        ++ gradientIndex;
      }
      catch(...)
      {
        throw InternalException(HERE) << "Error: cannot compute the derivative of " << ev3Expression->ToString() << " with respect to " << evaluation_.inputVariablesNames_[rowIndex];
      }
    }
  }
  parser_.setVariablesFormulas(evaluation_.inputVariablesNames_, gradientFormulas);
  // Everything is ok (no exception)
  isAnalytical_ = true;
  isInitialized_ = true;
}

/* Gradient */
Matrix AnalyticalNumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a NumericalMathFunction with an argument of invalid dimension";
  if (!isInitialized_) initialize();
  if (!isAnalytical_) throw InternalException(HERE) << "The gradient does not have an analytical expression.";
  const UnsignedInteger outputDimension(getOutputDimension());
  Matrix out(inputDimension, outputDimension);
  NumericalPoint outP(parser_(inP));
  ++ callsNumber_;
  UnsignedInteger parserIndex = 0;
  for (UnsignedInteger columnIndex = 0; columnIndex < outputDimension; ++ columnIndex)
  {
    for (UnsignedInteger rowIndex = 0; rowIndex < inputDimension; ++ rowIndex)
    {
      out(rowIndex, columnIndex) = outP[parserIndex];
      ++ parserIndex;
    }
  }
  return out;
}

/* Accessor for input point dimension */
UnsignedInteger AnalyticalNumericalMathGradientImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger AnalyticalNumericalMathGradientImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor to a specific formula */
String AnalyticalNumericalMathGradientImplementation::getFormula(const UnsignedInteger i,
    const UnsignedInteger j) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if ((i >= inputDimension) || (j >= getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: cannot access to a formula outside of the gradient dimensions.";
  if (!isInitialized_) initialize();
  return parser_.getFormulas()[i + j * inputDimension];
}

/* Get the i-th marginal function */
AnalyticalNumericalMathGradientImplementation::Implementation AnalyticalNumericalMathGradientImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal gradient must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
AnalyticalNumericalMathGradientImplementation::Implementation AnalyticalNumericalMathGradientImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension() - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal gradient must be in the range [0, dim-1] and  must be different";
  const UnsignedInteger marginalDimension(indices.getSize());
  Description marginalFormulas(marginalDimension);
  Description marginalOutputNames(marginalDimension);
  Description outputNames(evaluation_.getOutputVariablesNames());
  Description formulas(evaluation_.getFormulas());
  for (UnsignedInteger i = 0; i < marginalDimension; ++i)
  {
    marginalFormulas[i] = formulas[indices[i]];
    marginalOutputNames[i] = outputNames[indices[i]];
  }
  return new AnalyticalNumericalMathGradientImplementation(AnalyticalNumericalMathEvaluationImplementation(evaluation_.getInputVariablesNames(), marginalOutputNames, marginalFormulas));
}

/* Method save() stores the object through the StorageManager */
void AnalyticalNumericalMathGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void AnalyticalNumericalMathGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
  *this = AnalyticalNumericalMathGradientImplementation(evaluation_);
}

END_NAMESPACE_OPENTURNS
