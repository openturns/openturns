//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/UniVariateFunctionEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniVariateFunctionEvaluation);

static const Factory<UniVariateFunctionEvaluation> Factory_UniVariateFunctionEvaluation;

/* Default constructor */
UniVariateFunctionEvaluation::UniVariateFunctionEvaluation()
  : NumericalMathEvaluationImplementation()
  , function_()
{
  // Nothing to do
}


/* Constructor */
UniVariateFunctionEvaluation::UniVariateFunctionEvaluation(const UniVariateFunction & function)
  : NumericalMathEvaluationImplementation()
  , function_(function)
{
  // Set the descriptions
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}


/* Virtual constructor */
UniVariateFunctionEvaluation * UniVariateFunctionEvaluation::clone() const
{
  return new UniVariateFunctionEvaluation(*this);
}


/* String converter */
String UniVariateFunctionEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
                   << " function=" << function_;
}

String UniVariateFunctionEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << __repr__();
  return oss;
}


/* Operator (): Evaluate a product of 1D polynomials for one sample */
NumericalPoint UniVariateFunctionEvaluation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inDimension(inP.getDimension());
  if (inDimension != 1) throw InvalidArgumentException(HERE) << "Error: trying to evaluate an UniVariateFunction with an argument of invalid dimension";
  const NumericalPoint result(1, function_(inP[0]));
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger UniVariateFunctionEvaluation::getInputDimension() const
{
  return 1;
}

/* Accessor for output point dimension */
UnsignedInteger UniVariateFunctionEvaluation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void UniVariateFunctionEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("function_", function_);
}

/* Method load() reloads the object from the StorageManager */
void UniVariateFunctionEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("function_", function_);
}


END_NAMESPACE_OPENTURNS
