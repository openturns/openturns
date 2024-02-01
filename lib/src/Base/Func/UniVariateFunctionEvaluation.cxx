//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/UniVariateFunctionEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniVariateFunctionEvaluation)

static const Factory<UniVariateFunctionEvaluation> Factory_UniVariateFunctionEvaluation;

/* Default constructor */
UniVariateFunctionEvaluation::UniVariateFunctionEvaluation()
  : EvaluationImplementation()
  , function_()
{
  // Nothing to do
}


/* Constructor */
UniVariateFunctionEvaluation::UniVariateFunctionEvaluation(const UniVariateFunction & function)
  : EvaluationImplementation()
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

String UniVariateFunctionEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << __repr__();
  return oss;
}


/* Operator (): Evaluate a product of 1D polynomials for one sample */
Point UniVariateFunctionEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inDimension(inP.getDimension());
  if (inDimension != 1) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a UniVariateFunction with an argument of dimension " << inDimension << " when this dimension should be 1.";
  const Point result(1, function_(inP[0]));
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
  EvaluationImplementation::save(adv);
  adv.saveAttribute("function_", function_);
}

/* Method load() reloads the object from the StorageManager */
void UniVariateFunctionEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("function_", function_);
}


END_NAMESPACE_OPENTURNS
