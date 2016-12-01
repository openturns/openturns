//                                               -*- C++ -*-
/**
 * @brief The class that implements the Hessian of analytical functions.
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

#include "openturns/AnalyticalNumericalMathHessianImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "Ev3/expression.h"
#include "Ev3/parser.h"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(AnalyticalNumericalMathHessianImplementation);

static const Factory<AnalyticalNumericalMathHessianImplementation> Factory_AnalyticalNumericalMathHessianImplementation;


/* Default constructor */
AnalyticalNumericalMathHessianImplementation::AnalyticalNumericalMathHessianImplementation()
  : NumericalMathHessianImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , evaluation_()
{
  // Nothing to do
} // AnalyticalNumericalMathHessianImplementation

/* Default constructor */
AnalyticalNumericalMathHessianImplementation::AnalyticalNumericalMathHessianImplementation(const AnalyticalNumericalMathEvaluationImplementation & evaluation)
  : NumericalMathHessianImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , evaluation_(evaluation)
{
  // Nothing to do
} // AnalyticalNumericalMathHessianImplementation


/* Virtual constructor */
AnalyticalNumericalMathHessianImplementation * AnalyticalNumericalMathHessianImplementation::clone() const
{
  AnalyticalNumericalMathHessianImplementation * result = new AnalyticalNumericalMathHessianImplementation(*this);
  result->isInitialized_ = false;
  return result;
}


/* Comparison operator */
Bool AnalyticalNumericalMathHessianImplementation::operator ==(const AnalyticalNumericalMathHessianImplementation & other) const
{
  return true;
}

/* String converter */
String AnalyticalNumericalMathHessianImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << AnalyticalNumericalMathHessianImplementation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter */
String AnalyticalNumericalMathHessianImplementation::__str__(const String & offset) const
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
      UnsignedInteger length = 0;
      const UnsignedInteger iMax = getInputDimension();
      const UnsignedInteger kMax = getOutputDimension();
      for (UnsignedInteger k = 0; k < kMax; ++k)
      {
        const UnsignedInteger lengthK = outputVariablesNames[k].length();
        for (UnsignedInteger i = 0; i < iMax; ++i)
        {
          const UnsignedInteger lengthI = inputVariablesNames[i].length();
          // The diagonal term is always shorter than one of the off-diagonal terms
          for (UnsignedInteger j = 0; j < i; ++j)
            length = std::max(length, lengthI + lengthK + static_cast<UnsignedInteger>(inputVariablesNames[j].length()) + 14);
        } // For i
      } // For k
      for (UnsignedInteger k = 0; k < kMax; ++k)
      {
        for (UnsignedInteger i = 0; i < iMax; ++i)
        {
          for (UnsignedInteger j = 0; j < i; ++j)
          {
            oss << offset << "| " << std::setw(length) << ("d^2(" + outputVariablesNames[k] + ") / d(" + inputVariablesNames[i] + ")d(" + inputVariablesNames[j] + ")") << " = " << getFormula(i, j, k) << Os::GetEndOfLine();
          }
          oss << offset << "| " << std::setw(length) << ("d^2(" + outputVariablesNames[k] + ") / d(" + inputVariablesNames[i] + ")^2") << " = " << getFormula(i, i, k) << Os::GetEndOfLine();
        } // For i
      } // For k
    } // isAnalytical
  }
  catch(...)
  {
    // Nothing to do
  }
  if (!isAnalytical_) oss << offset << "No analytical hessian available. Try using finite difference instead.";
  return oss;
}

/* Must initialize the parser at the first call to operator() as the
   reference associated with the variables may have change after the construction */
void AnalyticalNumericalMathHessianImplementation::initialize() const
{
  if (isInitialized_) return;

  isAnalytical_ = false;
  const UnsignedInteger inputSize = evaluation_.inputVariablesNames_.getSize();
  const UnsignedInteger outputSize = evaluation_.outputVariablesNames_.getSize();
  const UnsignedInteger hessianSize = inputSize * (inputSize + 1) * outputSize / 2;
  // For each element of the hessian, do
  UnsignedInteger hessianIndex = 0;
  Description hessianFormulas(hessianSize);
  for (UnsignedInteger sheetIndex = 0; sheetIndex < outputSize; ++sheetIndex)
  {
    // Parse the current formula with Ev3
    int nerr(0);
    Ev3::ExpressionParser ev3Parser;
    // Initialize the variable indices in order to match the order of OpenTURNS in Ev3
    for (UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputSize; ++inputVariableIndex) ev3Parser.SetVariableID(evaluation_.inputVariablesNames_[inputVariableIndex], inputVariableIndex);
    Ev3::Expression ev3Expression;
    try
    {
      ev3Expression = ev3Parser.Parse(evaluation_.formulas_[sheetIndex].c_str(), nerr);
    }
    catch (Ev3::ErrBase & exc)
    {
      throw InternalException(HERE) << exc.description_;
    }
    if (nerr != 0) throw InvalidArgumentException(HERE) << "Error: cannot parse " << evaluation_.formulas_[sheetIndex] << " with Ev3. No analytical hessian.";
    //                Ev3::Simplify(&ev3Expression);
    for (UnsignedInteger rowIndex = 0; rowIndex < inputSize; ++rowIndex)
    {
      Ev3::Expression firstDerivative;
      try
      {
        firstDerivative = Ev3::Diff(ev3Expression, rowIndex);
        //                    Ev3::Simplify(&firstDerivative);
        LOGINFO(OSS() << "First variable=" << evaluation_.inputVariablesNames_[rowIndex] << ", derivative=" << firstDerivative->ToString());
      }
      catch(...)
      {
        throw InternalException(HERE) << "Error: cannot compute the derivative of " << ev3Expression->ToString() << " with respect to " << evaluation_.inputVariablesNames_[rowIndex];
      }
      for (UnsignedInteger columnIndex = 0; columnIndex <= rowIndex; ++columnIndex)
      {
        try
        {
          Ev3::Expression secondDerivative(Ev3::Diff(firstDerivative, columnIndex));
          //                        Ev3::Simplify(&secondDerivative);
          LOGINFO(OSS() << "d2(" << ev3Expression->ToString() << ")/d(" << evaluation_.inputVariablesNames_[rowIndex] << ")d(" << evaluation_.inputVariablesNames_[columnIndex] << ")=" << secondDerivative->ToString());
          hessianFormulas[hessianIndex] = secondDerivative->ToString();
          ++ hessianIndex;
        }
        catch(...)
        {
          throw InternalException(HERE) << "Error: cannot compute the derivative of " << firstDerivative->ToString() << " with respect to " << evaluation_.inputVariablesNames_[columnIndex];
        }
      } // columnIndex
    } // rowIndex
  } // sheetIndex

  parser_.setVariablesFormulas(evaluation_.inputVariablesNames_, hessianFormulas);
  // Everything is ok (no exception)
  isAnalytical_ = true;
  isInitialized_ = true;
}

/* Hessian */
SymmetricTensor AnalyticalNumericalMathHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a NumericalMathFunction with an argument of invalid dimension";
  if (!isInitialized_) initialize();
  if (!isAnalytical_) throw InternalException(HERE) << "The hessian does not have an analytical expression.";
  const UnsignedInteger outputDimension = getOutputDimension();
  SymmetricTensor out(inputDimension, outputDimension);
  ++ callsNumber_;
  NumericalPoint outP(parser_(inP));
  UnsignedInteger parserIndex = 0;
  for (UnsignedInteger sheetIndex = 0; sheetIndex < outputDimension; ++ sheetIndex)
  {
    for (UnsignedInteger rowIndex = 0; rowIndex < inputDimension; ++ rowIndex)
    {
      for (UnsignedInteger columnIndex = 0; columnIndex <= rowIndex; ++ columnIndex)
      {
        out(rowIndex, columnIndex, sheetIndex) = outP[parserIndex];
        ++ parserIndex;
      } // columnIndex
    } // rowIndex
  } // sheetIndex
  return out;
}

/* Accessor to a specific formula */
String AnalyticalNumericalMathHessianImplementation::getFormula(const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if ((i >= inputDimension) || (j >= inputDimension) || (k >= getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: cannot access to a formula outside of the hessian dimensions.";
  if (!isInitialized_) initialize();
  // Convert the 3D index into a linear index
  UnsignedInteger rowIndex = i;
  UnsignedInteger columnIndex = j;
  UnsignedInteger sheetIndex = k;
  // First, take the symmetry into account
  if (i < j)
  {
    rowIndex = j;
    columnIndex = i;
  }
  // Now, columnIndex <= rowIndex
  UnsignedInteger linearIndex = 0;
  // Each sheet adds a triangle with the main diagonal
  linearIndex += ((inputDimension * (inputDimension + 1)) / 2) * sheetIndex;
  // Compute the linear sub-index into the triangle
  linearIndex += (rowIndex * (rowIndex + 1)) / 2 + columnIndex;
  return parser_.getFormulas()[linearIndex];
}

/* Accessor for input point dimension */
UnsignedInteger AnalyticalNumericalMathHessianImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger AnalyticalNumericalMathHessianImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Get the i-th marginal function */
AnalyticalNumericalMathHessianImplementation::Implementation AnalyticalNumericalMathHessianImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal hessian must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
AnalyticalNumericalMathHessianImplementation::Implementation AnalyticalNumericalMathHessianImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal hessian must be in the range [0, dim-1] and must be different";
  const UnsignedInteger marginalDimension = indices.getSize();
  Description marginalFormulas(marginalDimension);
  Description marginalOutputNames(marginalDimension);
  Description outputNames(evaluation_.getOutputVariablesNames());
  Description formulas(evaluation_.getFormulas());
  for (UnsignedInteger i = 0; i < marginalDimension; ++i)
  {
    marginalFormulas[i] = formulas[indices[i]];
    marginalOutputNames[i] = outputNames[indices[i]];
  }
  return new AnalyticalNumericalMathHessianImplementation(AnalyticalNumericalMathEvaluationImplementation(evaluation_.getInputVariablesNames(), marginalOutputNames, marginalFormulas));
}

/* Method save() stores the object through the StorageManager */
void AnalyticalNumericalMathHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void AnalyticalNumericalMathHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
  *this = AnalyticalNumericalMathHessianImplementation(evaluation_);
}

END_NAMESPACE_OPENTURNS
