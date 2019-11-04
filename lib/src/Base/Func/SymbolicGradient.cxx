//                                               -*- C++ -*-
/**
 *  @brief The class that implements the gradient of an analytical function.
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

#include "openturns/SymbolicGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "Ev3/expression.h"
#include "Ev3/parser.h"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SymbolicGradient)

static const Factory<SymbolicGradient> Factory_SymbolicGradient;

/* Default constructor */
SymbolicGradient::SymbolicGradient()
  : GradientImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , p_evaluation_(new SymbolicEvaluation)
{
  // Nothing to do
} // SymbolicGradient

/* Default constructor */
SymbolicGradient::SymbolicGradient(const SymbolicEvaluation & evaluation)
  : GradientImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
} // SymbolicGradient


/* Parameters constructor */
SymbolicGradient::SymbolicGradient(Pointer<SymbolicEvaluation> & p_evaluation)
  : GradientImplementation()
  , isInitialized_(false)
  , isAnalytical_(true)
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
} // SymbolicGradient


/* Virtual constructor */
SymbolicGradient * SymbolicGradient::clone() const
{
  SymbolicGradient * result = new SymbolicGradient(*this);
  result->isInitialized_ = false;
  return result;
}


/* Comparison operator */
Bool SymbolicGradient::operator ==(const SymbolicGradient & other) const
{
  return (p_evaluation_ == other.p_evaluation_);
}

/* String converter */
String SymbolicGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SymbolicGradient::GetClassName()
      << " name=" << getName()
      << " evaluation=" << *p_evaluation_;
  return oss;
}

/* String converter */
String SymbolicGradient::__str__(const String & offset) const
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
      const UnsignedInteger jMax = getOutputDimension();
      for (UnsignedInteger i = 0; i < iMax; ++i)
      {
        const UnsignedInteger lengthI = inputVariablesNames[i].length();
        for (UnsignedInteger j = 0; j < jMax; ++j)
          length = std::max(length, lengthI + static_cast<UnsignedInteger>(outputVariablesNames[j].length()) + 8);
      }
      for (UnsignedInteger j = 0; j < jMax; ++j)
      {
        for (UnsignedInteger i = 0; i < iMax; ++i)
        {
          oss << "| " << std::setw(length) << ("d(" + outputVariablesNames[j] + ") / d(" + inputVariablesNames[i] + ")") << " = " << getFormula(i, j) << Os::GetEndOfLine() << offset;
        }
      }
    } // isAnalytical
  }
  catch(...)
  {
    // Nothing to do
  }
  if (!isAnalytical_) oss << "No analytical gradient available. Try using finite difference instead.";
  return oss;
}

/* Must initialize the parser at the first call to operator() as the
   reference associated with the variables may have change after the construction */
void SymbolicGradient::initialize() const
{
  if (isInitialized_) return;
  isAnalytical_ = false;
  const UnsignedInteger inputSize = p_evaluation_->inputVariablesNames_.getSize();
  const UnsignedInteger outputSize = p_evaluation_->outputVariablesNames_.getSize();
  const UnsignedInteger gradientSize = inputSize * outputSize;
  Description gradientFormulas(gradientSize);
  // For each element of the gradient, do
  UnsignedInteger gradientIndex = 0;
  for (UnsignedInteger columnIndex = 0; columnIndex < outputSize; ++columnIndex)
  {
    // Parse the current formula with Ev3
    int nerr(0);
    Ev3::ExpressionParser ev3Parser;
    // Initialize the variable indices in order to match the order of OpenTURNS in Ev3
    for (UnsignedInteger inputVariableIndex = 0; inputVariableIndex < inputSize; ++inputVariableIndex) ev3Parser.SetVariableID(p_evaluation_->inputVariablesNames_[inputVariableIndex], inputVariableIndex);
    Ev3::Expression ev3Expression;
    try
    {
      ev3Expression = ev3Parser.Parse(p_evaluation_->formulas_[columnIndex].c_str(), nerr);
    }
    catch (Ev3::ErrBase & exc)
    {
      throw InternalException(HERE) << exc.description_;
    }
    if (nerr != 0) throw InvalidArgumentException(HERE) << "Error: cannot parse " << p_evaluation_->formulas_[columnIndex] << " with Ev3. No analytical gradient.";
    //                Ev3::Simplify(&ev3Expression);
    for (UnsignedInteger rowIndex = 0; rowIndex < inputSize; ++rowIndex)
    {
      try
      {
        Ev3::Expression derivative(Ev3::Diff(ev3Expression, rowIndex));
        //                    Ev3::Simplify(&derivative);
        LOGDEBUG(OSS() << "d(" << ev3Expression->ToString() << ")/d(" << p_evaluation_->inputVariablesNames_[rowIndex] << ")=" << derivative->ToString());
        gradientFormulas[gradientIndex] = derivative->ToString();
        ++ gradientIndex;
      }
      catch(...)
      {
        throw InternalException(HERE) << "Error: cannot compute the derivative of " << ev3Expression->ToString() << " with respect to " << p_evaluation_->inputVariablesNames_[rowIndex];
      }
    }
  }
  parser_.setVariables(p_evaluation_->inputVariablesNames_);
  parser_.setFormulas(gradientFormulas);
  // Everything is ok (no exception)
  isAnalytical_ = true;
  isInitialized_ = true;
}

/* Gradient */
Matrix SymbolicGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a Function with an argument of invalid dimension";
  if (!isInitialized_) initialize();
  if (!isAnalytical_) throw InternalException(HERE) << "The gradient does not have an analytical expression.";
  const UnsignedInteger outputDimension = getOutputDimension();
  Matrix out(inputDimension, outputDimension);
  Point outP(parser_(inP));
  callsNumber_.increment();
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
UnsignedInteger SymbolicGradient::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger SymbolicGradient::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* Accessor to a specific formula */
String SymbolicGradient::getFormula(const UnsignedInteger i,
                                    const UnsignedInteger j) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if ((i >= inputDimension) || (j >= getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: cannot access to a formula outside of the gradient dimensions.";
  if (!isInitialized_) initialize();
  return parser_.getFormulas()[i + j * inputDimension];
}

/* Get the i-th marginal function */
Gradient SymbolicGradient::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal gradient must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Gradient SymbolicGradient::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal gradient must be in the range [0, dim-1] and must be different";
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
  return new SymbolicGradient(SymbolicEvaluation(p_evaluation_->getInputVariablesNames(), marginalOutputNames, marginalFormulas));
}

/* Method save() stores the object through the StorageManager */
void SymbolicGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void SymbolicGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  TypedInterfaceObject<SymbolicEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}

END_NAMESPACE_OPENTURNS
