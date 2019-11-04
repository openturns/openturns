//                                               -*- C++ -*-
/**
 *  @brief The class that implements the Hessian of analytical functions.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/SymbolicHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "Ev3/expression.h"
#include "Ev3/parser.h"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SymbolicHessian)

static const Factory<SymbolicHessian> Factory_SymbolicHessian;


/* Default constructor */
SymbolicHessian::SymbolicHessian()
  : HessianImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , p_evaluation_(new SymbolicEvaluation)
{
  // Nothing to do
} // SymbolicHessian

/* Default constructor */
SymbolicHessian::SymbolicHessian(const SymbolicEvaluation & evaluation)
  : HessianImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
} // SymbolicHessian


/* Constructor for implementation pointer */
SymbolicHessian::SymbolicHessian(const Pointer<SymbolicEvaluation> & p_evaluation)
  : HessianImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
} // SymbolicHessian


/* Virtual constructor */
SymbolicHessian * SymbolicHessian::clone() const
{
  SymbolicHessian * result = new SymbolicHessian(*this);
  result->isInitialized_ = false;
  return result;
}


/* Comparison operator */
Bool SymbolicHessian::operator ==(const SymbolicHessian & other) const
{
  if (this == &other) return true;
  return *p_evaluation_ == *other.p_evaluation_;
}

/* String converter */
String SymbolicHessian::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SymbolicHessian::GetClassName()
      << " name=" << getName()
      << " evaluation=" << *p_evaluation_;
  return oss;
}

/* String converter */
String SymbolicHessian::__str__(const String & offset) const
{
  OSS oss(false);
  // Check that the symbolic differentiation has been done
  try
  {
    initialize();
    if (isAnalytical_)
    {
      oss << Os::GetEndOfLine() << offset;
      // First, find the maximum length of the output variable names
      const Description inputVariablesNames(p_evaluation_->getInputVariablesNames());
      const Description outputVariablesNames(p_evaluation_->getOutputVariablesNames());
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
            oss << "| " << std::setw(length) << ("d^2(" + outputVariablesNames[k] + ") / d(" + inputVariablesNames[i] + ")d(" + inputVariablesNames[j] + ")") << " = " << getFormula(i, j, k) << Os::GetEndOfLine() << offset;
          }
          oss << "| " << std::setw(length) << ("d^2(" + outputVariablesNames[k] + ") / d(" + inputVariablesNames[i] + ")^2") << " = " << getFormula(i, i, k) << Os::GetEndOfLine() << offset;
        } // For i
      } // For k
    } // isAnalytical
  }
  catch(...)
  {
    // Nothing to do
  }
  if (!isAnalytical_) oss << "No analytical hessian available. Try using finite difference instead.";
  return oss;
}

/* Must initialize the parser at the first call to operator() as the
   reference associated with the variables may have change after the construction */
void SymbolicHessian::initialize() const
{
  if (isInitialized_) return;

  isAnalytical_ = false;
  const UnsignedInteger inputSize = p_evaluation_->inputVariablesNames_.getSize();
  const UnsignedInteger outputSize = p_evaluation_->outputVariablesNames_.getSize();
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
    for (UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputSize; ++inputVariableIndex) ev3Parser.SetVariableID(p_evaluation_->inputVariablesNames_[inputVariableIndex], inputVariableIndex);
    Ev3::Expression ev3Expression;
    try
    {
      ev3Expression = ev3Parser.Parse(p_evaluation_->formulas_[sheetIndex].c_str(), nerr);
    }
    catch (Ev3::ErrBase & exc)
    {
      throw InternalException(HERE) << exc.description_;
    }
    if (nerr != 0) throw InvalidArgumentException(HERE) << "Error: cannot parse " << p_evaluation_->formulas_[sheetIndex] << " with Ev3. No analytical hessian.";
    //                Ev3::Simplify(&ev3Expression);
    for (UnsignedInteger rowIndex = 0; rowIndex < inputSize; ++rowIndex)
    {
      Ev3::Expression firstDerivative;
      try
      {
        firstDerivative = Ev3::Diff(ev3Expression, rowIndex);
        //                    Ev3::Simplify(&firstDerivative);
        LOGDEBUG(OSS() << "First variable=" << p_evaluation_->inputVariablesNames_[rowIndex] << ", derivative=" << firstDerivative->ToString());
      }
      catch(...)
      {
        throw InternalException(HERE) << "Error: cannot compute the derivative of " << ev3Expression->ToString() << " with respect to " << p_evaluation_->inputVariablesNames_[rowIndex];
      }
      for (UnsignedInteger columnIndex = 0; columnIndex <= rowIndex; ++columnIndex)
      {
        try
        {
          Ev3::Expression secondDerivative(Ev3::Diff(firstDerivative, columnIndex));
          //                        Ev3::Simplify(&secondDerivative);
          LOGDEBUG(OSS() << "d2(" << ev3Expression->ToString() << ")/d(" << p_evaluation_->inputVariablesNames_[rowIndex] << ")d(" << p_evaluation_->inputVariablesNames_[columnIndex] << ")=" << secondDerivative->ToString());
          hessianFormulas[hessianIndex] = secondDerivative->ToString();
          ++ hessianIndex;
        }
        catch(...)
        {
          throw InternalException(HERE) << "Error: cannot compute the derivative of " << firstDerivative->ToString() << " with respect to " << p_evaluation_->inputVariablesNames_[columnIndex];
        }
      } // columnIndex
    } // rowIndex
  } // sheetIndex

  parser_.setVariables(p_evaluation_->inputVariablesNames_);
  parser_.setFormulas(hessianFormulas);
  // Everything is ok (no exception)
  isAnalytical_ = true;
  isInitialized_ = true;
}

/* Hessian */
SymmetricTensor SymbolicHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a Function with an argument of invalid dimension";
  if (!isInitialized_) initialize();
  if (!isAnalytical_) throw InternalException(HERE) << "The hessian does not have an analytical expression.";
  const UnsignedInteger outputDimension = getOutputDimension();
  SymmetricTensor out(inputDimension, outputDimension);
  callsNumber_.increment();
  Point outP(parser_(inP));
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
String SymbolicHessian::getFormula(const UnsignedInteger i,
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
UnsignedInteger SymbolicHessian::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger SymbolicHessian::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* Get the i-th marginal function */
Hessian SymbolicHessian::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal hessian must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Hessian SymbolicHessian::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal hessian must be in the range [0, dim-1] and must be different";
  const UnsignedInteger marginalDimension = indices.getSize();
  Description marginalFormulas(marginalDimension);
  Description marginalOutputNames(marginalDimension);
  Description outputNames(p_evaluation_->getOutputVariablesNames());
  Description formulas(p_evaluation_->getFormulas());
  for (UnsignedInteger i = 0; i < marginalDimension; ++i)
  {
    marginalFormulas[i] = formulas[indices[i]];
    marginalOutputNames[i] = outputNames[indices[i]];
  }
  return new SymbolicHessian(SymbolicEvaluation(p_evaluation_->getInputVariablesNames(), marginalOutputNames, marginalFormulas));
}

/* Method save() stores the object through the StorageManager */
void SymbolicHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void SymbolicHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  TypedInterfaceObject<SymbolicEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}

END_NAMESPACE_OPENTURNS
