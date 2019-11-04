//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of an aggregation of functions from R^n to R^p_1,...,R^n to R^p_k
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
#include "openturns/AggregatedEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedEvaluation)

static const Factory<AggregatedEvaluation> Factory_AggregatedEvaluation;


/* Default constructor */
AggregatedEvaluation::AggregatedEvaluation()
  : EvaluationImplementation()
  , functionsCollection_(0)
  , outputDimension_(0)
{
  // Nothing to do
}


/* Parameters constructor */
AggregatedEvaluation::AggregatedEvaluation(const FunctionCollection & functionsCollection)
  : EvaluationImplementation()
  , functionsCollection_(0)
  , outputDimension_(0)
{
  setFunctionsCollection(functionsCollection);
}


/* Virtual constructor */
AggregatedEvaluation * AggregatedEvaluation::clone() const
{
  return new AggregatedEvaluation(*this);
}


/* String converter */
String AggregatedEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " functions=" << functionsCollection_;
}

String AggregatedEvaluation::__str__(const String & offset) const
{
  return OSS(false) << functionsCollection_.__str__(offset);
}


/* Evaluation operator */
Point AggregatedEvaluation::operator () (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  const UnsignedInteger size = functionsCollection_.getSize();
  Point result(outputDimension_);
  UnsignedInteger outputIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point atomValue(functionsCollection_[i](inP));
    const UnsignedInteger atomDimension = atomValue.getDimension();
    for (UnsignedInteger j = 0; j < atomDimension; ++j)
    {
      result[outputIndex] = atomValue[j];
      ++outputIndex;
    }

  }
  return result;
}


Sample AggregatedEvaluation::operator () (const Sample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  callsNumber_.fetchAndAdd(inS.getSize());
  const UnsignedInteger collectionSize = functionsCollection_.getSize();
  const UnsignedInteger size = inS.getSize();
  Sample result(size, outputDimension_);
  UnsignedInteger outputIndex = 0;
  for ( UnsignedInteger k = 0; k < collectionSize; ++ k )
  {
    const Sample atomValue(functionsCollection_[k](inS));
    const UnsignedInteger atomDimension = atomValue.getDimension();
    for ( UnsignedInteger i = 0; i < size; ++ i )
      for ( UnsignedInteger j = 0; j < atomDimension; ++ j )
        result(i, outputIndex + j) = atomValue(i, j);
    outputIndex += atomDimension;
  }
  result.setDescription(getOutputDescription());
  return result;
}


/* Functions accessor */
AggregatedEvaluation::FunctionCollection AggregatedEvaluation::getFunctionsCollection() const
{
  return functionsCollection_;
}

void AggregatedEvaluation::setFunctionsCollection(const FunctionCollection & functionsCollection)
{
  const UnsignedInteger size = functionsCollection.getSize();
  // Check for empty functions collection
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an aggregated function from an empty collection of functions.";
  // Check for coherent input and output dimensions of the functions
  UnsignedInteger inputDimension = functionsCollection[0].getInputDimension();
  outputDimension_ = functionsCollection[0].getOutputDimension();
  Description description(functionsCollection[0].getDescription());
  if (outputDimension_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an aggregated function with atoms of null output dimension.";
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (functionsCollection[i].getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given functions have incompatible input dimension.";
    const UnsignedInteger atomOutputDimension = functionsCollection[i].getOutputDimension();
    if (atomOutputDimension == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an aggregated function with atoms of null output dimension.";
    outputDimension_ += atomOutputDimension;
    const Description outputDescription(functionsCollection[i].getOutputDescription());
    for (UnsignedInteger j = 0; j < atomOutputDimension; ++j)
      description.add(outputDescription[j]);
  }
  functionsCollection_ = functionsCollection;
  setDescription(description);
}

/* Get the i-th marginal function */
Evaluation AggregatedEvaluation::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Evaluation AggregatedEvaluation::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getOutputDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal aggregated function must be in the range [0, dim-1] and must be different";
  FunctionCollection marginalFunctions;
  const UnsignedInteger indicesSize = indices.getSize();
  const UnsignedInteger size = functionsCollection_.getSize();
  // For each copula, see if there is something to extract
  UnsignedInteger currentPosition = 0;
  UnsignedInteger currentIndex = indices[currentPosition];
  // Lower bound of indices related to the current copula
  UnsignedInteger lowerIndex = 0;
  // Upper bound of indices related to the current copula plus 1
  UnsignedInteger upperIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Function function(functionsCollection_[i]);
    // Update index range for the current function
    lowerIndex = upperIndex;
    upperIndex += function.getOutputDimension();
    Indices functionIndices(0);
    // Find the indices related to the current function
    while ((currentPosition < indicesSize) && (currentIndex >= lowerIndex) && (currentIndex < upperIndex))
    {
      functionIndices.add(currentIndex - lowerIndex);
      // Go to next index
      ++currentPosition;
      if (currentPosition == indicesSize) break;
      currentIndex = indices[currentPosition];
    }
    // If there is something to extract
    if (functionIndices.getSize() > 0) marginalFunctions.add(functionsCollection_[i].getMarginal(functionIndices));
    // All the indices have been taken into account
    if (currentPosition == indicesSize) break;
    // Check if a bad case occurs: one index related to function i is found after indices related to function j, with j > i. In this case we use the generic marginal extraction
    if (currentIndex < lowerIndex) return EvaluationImplementation::getMarginal(indices);
  }
  return new AggregatedEvaluation(marginalFunctions);
}

/* Input dimension accessor */
UnsignedInteger AggregatedEvaluation::getInputDimension() const
{
  return functionsCollection_[0].getInputDimension();
}

/* Output dimension accessor */
UnsignedInteger AggregatedEvaluation::getOutputDimension() const
{
  return outputDimension_;
}


/* Gradient according to the marginal parameters */
Matrix AggregatedEvaluation::parameterGradient(const Point & inP) const
{
  Matrix result(getParameter().getDimension(), getOutputDimension());
  const UnsignedInteger size = functionsCollection_.getSize();
  UnsignedInteger rowShift = 0;
  UnsignedInteger columnShift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Matrix currentGradient(functionsCollection_[i].parameterGradient(inP));
    const UnsignedInteger currentRowDim = currentGradient.getNbRows();
    const UnsignedInteger currentColumnDim = currentGradient.getNbColumns();
    for (UnsignedInteger j = 0; j < currentRowDim; ++j)
      for (UnsignedInteger k = 0; k < currentColumnDim; ++k)
        result(rowShift + j, columnShift + k) = currentGradient(j, k);
    rowShift += currentRowDim;
    columnShift += currentColumnDim;
  }
  return result;
}

/* Parameters value accessor */
Point AggregatedEvaluation::getParameter() const
{
  Point parameter;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    parameter.add(functionsCollection_[i].getParameter());
  }
  return parameter;
}

void AggregatedEvaluation::setParameter(const Point & parameter)
{
  const UnsignedInteger size = functionsCollection_.getSize();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    Point marginalParameter(functionsCollection_[i].getParameter());
    const UnsignedInteger marginalDimension = marginalParameter.getDimension();
    for (UnsignedInteger j = 0; j < marginalDimension; ++ j)
    {
      marginalParameter[j] = parameter[index];
      ++ index;
    }
    functionsCollection_[i].setParameter(marginalParameter);
  }
}

/* Parameters description accessor */
Description AggregatedEvaluation::getParameterDescription() const
{
  Description description;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    description.add(functionsCollection_[i].getParameterDescription());
  }
  return description;
}

/* Linearity accessors */
Bool AggregatedEvaluation::isLinear() const
{
  for (UnsignedInteger i = 0; i < functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].isLinear())
      return false;

  return true;
}

Bool AggregatedEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  for (UnsignedInteger i = 0; i < functionsCollection_.getSize(); ++i)
    if (!functionsCollection_[i].isLinearlyDependent(index))
      return false;

  return true;
}

/* Method save() stores the object through the StorageManager */
void AggregatedEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "functionsCollection_", functionsCollection_ );
  adv.saveAttribute( "outputDimension_", outputDimension_ );
}


/* Method load() reloads the object from the StorageManager */
void AggregatedEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "functionsCollection_", functionsCollection_ );
  adv.loadAttribute( "outputDimension_", outputDimension_ );
}


END_NAMESPACE_OPENTURNS
