//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear combination of functions.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/LinearCombinationEvaluation.hxx"
#include "openturns/LinearCombinationGradient.hxx"
#include "openturns/LinearCombinationHessian.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearCombinationFunction)

/* Default constructor */
LinearCombinationFunction::LinearCombinationFunction ()
  : Function()
{
}


/* Parameter constructor */
LinearCombinationFunction::LinearCombinationFunction (const FunctionCollection & functionCollection,
    const Point & coefficients)
  : Function()
{
  // The atoms are optionally merged (flattened and deduplicated) when the
  // "LinearCombinationFunction-MergeDuplicates" ResourceMap key is set to true
  FunctionCollection mergedFunctions(0);
  Point mergedCoefficients(0);
  if (ResourceMap::GetAsBool("LinearCombinationFunction-MergeDuplicates"))
  {
    const UnsignedInteger size = functionCollection.getSize();
    if (size != coefficients.getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build a linear combination with a different number of functions and coefficients.";
    for (UnsignedInteger i = 0; i < size; ++i)
      appendMerged(mergedFunctions, mergedCoefficients, functionCollection[i], coefficients[i]);
  }
  else
  {
    mergedFunctions = functionCollection;
    mergedCoefficients = coefficients;
  }
  const Pointer<LinearCombinationEvaluation> p_evaluation = new LinearCombinationEvaluation(mergedFunctions, mergedCoefficients);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new LinearCombinationGradient(p_evaluation));
  setHessian(new LinearCombinationHessian(p_evaluation));
}


/* Merge the given function, multiplied by the given coefficient, into this linear combination */
void LinearCombinationFunction::mergeLinearCombination(const Function & function,
    const Scalar coefficient)
{
  const LinearCombinationEvaluation * p_lce = dynamic_cast<const LinearCombinationEvaluation *>(getEvaluation().getImplementation().get());
  FunctionCollection functionCollection(0);
  Point mergedCoefficients(0);
  if (p_lce != nullptr)
  {
    const FunctionCollection atoms = p_lce->getFunctionsCollection();
    const Point atomCoefficients = p_lce->getCoefficients();
    for (UnsignedInteger i = 0; i < atoms.getSize(); ++i)
      appendMerged(functionCollection, mergedCoefficients, atoms[i], atomCoefficients[i]);
  }
  appendMerged(functionCollection, mergedCoefficients, function, coefficient);
  const Pointer<LinearCombinationEvaluation> p_evaluation = new LinearCombinationEvaluation(functionCollection, mergedCoefficients);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new LinearCombinationGradient(p_evaluation));
  setHessian(new LinearCombinationHessian(p_evaluation));
}


/* Append the given function, multiplied by the given coefficient, to the atom/coefficient lists of a linear combination */
void LinearCombinationFunction::appendMerged(FunctionCollection & functionCollection,
    Point & coefficients,
    const Function & function,
    const Scalar coefficient)
{
  // If the function is a linear combination in disguise, flatten it
  const LinearCombinationEvaluation * p_lce = dynamic_cast<const LinearCombinationEvaluation *>(function.getEvaluation().getImplementation().get());
  if (p_lce != nullptr)
  {
    const FunctionCollection atoms = p_lce->getFunctionsCollection();
    const Point atomCoefficients = p_lce->getCoefficients();
    for (UnsignedInteger j = 0; j < atoms.getSize(); ++j)
      appendMerged(functionCollection, coefficients, atoms[j], coefficient * atomCoefficients[j]);
    return;
  }
  // Otherwise merge the atom, summing the weights of identical atoms
  for (UnsignedInteger i = 0; i < functionCollection.getSize(); ++i)
  {
    // Two atoms can only be identical if their evaluation implementations
    // share the same dynamic type: several equals() implementations rely on
    // a dynamic_cast to a reference which would raise std::bad_cast on a
    // type mismatch, so we reject different types before comparing.
    const EvaluationImplementation & existingEval = *functionCollection[i].getEvaluation().getImplementation();
    const EvaluationImplementation & newEval = *function.getEvaluation().getImplementation();
    if (existingEval.getClassName() == newEval.getClassName())
    {
      try
      {
        if (existingEval == newEval)
        {
          coefficients[i] += coefficient;
          return;
        }
      }
      catch (NotYetImplementedException &)
      {
        // The equals() method is not implemented for this evaluation type;
        // skip the merge and treat the atoms as distinct.
      }
    }
  }
  functionCollection.add(function);
  coefficients.add(coefficient);
}


/* Build the linear combination equal to the sum of the two given functions */
LinearCombinationFunction LinearCombinationFunction::add(const Function & left,
    const Function & right,
    const Scalar rightCoefficient)
{
  FunctionCollection functionCollection(0);
  Point coefficients(0);
  appendMerged(functionCollection, coefficients, left, 1.0);
  appendMerged(functionCollection, coefficients, right, rightCoefficient);
  return LinearCombinationFunction(functionCollection, coefficients);
}


/* Build the linear combination equal to the negation of the given function */
LinearCombinationFunction LinearCombinationFunction::negate(const Function & function)
{
  FunctionCollection functionCollection(0);
  Point coefficients(0);
  appendMerged(functionCollection, coefficients, function, -1.0);
  return LinearCombinationFunction(functionCollection, coefficients);
}


/* Comparison operator */
Bool LinearCombinationFunction::operator ==(const LinearCombinationFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
}

/* String converter */
String LinearCombinationFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String LinearCombinationFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
