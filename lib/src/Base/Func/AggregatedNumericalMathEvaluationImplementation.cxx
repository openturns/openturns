//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of an aggregation of functions from R^n to R^p_1,...,R^n to R^p_k
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/AggregatedNumericalMathEvaluationImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedNumericalMathEvaluationImplementation);

static const Factory<AggregatedNumericalMathEvaluationImplementation> Factory_AggregatedNumericalMathEvaluationImplementation;


/* Default constructor */
AggregatedNumericalMathEvaluationImplementation::AggregatedNumericalMathEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , functionsCollection_(0)
  , outputDimension_(0)
{
  // Nothing to do
}


/* Parameters constructor */
AggregatedNumericalMathEvaluationImplementation::AggregatedNumericalMathEvaluationImplementation(const NumericalMathFunctionCollection & functionsCollection)
  : NumericalMathEvaluationImplementation()
  , functionsCollection_(0)
  , outputDimension_(0)
{
  setFunctionsCollection(functionsCollection);
}


/* Virtual constructor */
AggregatedNumericalMathEvaluationImplementation * AggregatedNumericalMathEvaluationImplementation::clone() const
{
  return new AggregatedNumericalMathEvaluationImplementation(*this);
}


/* String converter */
String AggregatedNumericalMathEvaluationImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " functions=" << functionsCollection_;
}

String AggregatedNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  return OSS(false) << functionsCollection_.__str__(offset);
}


/* Evaluation operator */
NumericalPoint AggregatedNumericalMathEvaluationImplementation::operator () (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  const UnsignedInteger size = functionsCollection_.getSize();
  NumericalPoint result(outputDimension_);
  UnsignedInteger outputIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint atomValue(functionsCollection_[i](inP));
    const UnsignedInteger atomDimension = atomValue.getDimension();
    for (UnsignedInteger j = 0; j < atomDimension; ++j)
    {
      result[outputIndex] = atomValue[j];
      ++outputIndex;
    }

  }
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}


NumericalSample AggregatedNumericalMathEvaluationImplementation::operator () (const NumericalSample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  ++ callsNumber_;
  const UnsignedInteger collectionSize = functionsCollection_.getSize();
  const UnsignedInteger size = inS.getSize();
  NumericalSample result(size, outputDimension_);
  UnsignedInteger outputIndex = 0;
  for ( UnsignedInteger k = 0; k < collectionSize; ++ k )
  {
    const NumericalSample atomValue(functionsCollection_[k](inS));
    const UnsignedInteger atomDimension = atomValue.getDimension();
    for ( UnsignedInteger i = 0; i < size; ++ i )
      for ( UnsignedInteger j = 0; j < atomDimension; ++ j )
        result[i][outputIndex + j] = atomValue[i][j];
    outputIndex += atomDimension;
  }
  result.setDescription(getOutputDescription());
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(result);
  }
  return result;
}


/* Functions accessor */
AggregatedNumericalMathEvaluationImplementation::NumericalMathFunctionCollection AggregatedNumericalMathEvaluationImplementation::getFunctionsCollection() const
{
  return functionsCollection_;
}

void AggregatedNumericalMathEvaluationImplementation::setFunctionsCollection(const NumericalMathFunctionCollection & functionsCollection)
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
AggregatedNumericalMathEvaluationImplementation::Implementation AggregatedNumericalMathEvaluationImplementation::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
AggregatedNumericalMathEvaluationImplementation::Implementation AggregatedNumericalMathEvaluationImplementation::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getOutputDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal aggregated function must be in the range [0, dim-1] and must be different";
  NumericalMathFunctionCollection marginalFunctions;
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
    const NumericalMathFunction function(functionsCollection_[i]);
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
    if (currentIndex < lowerIndex) return NumericalMathEvaluationImplementation::getMarginal(indices);
  }
  return new AggregatedNumericalMathEvaluationImplementation(marginalFunctions);
}

/* Input dimension accessor */
UnsignedInteger AggregatedNumericalMathEvaluationImplementation::getInputDimension() const
{
  return functionsCollection_[0].getInputDimension();
}

/* Output dimension accessor */
UnsignedInteger AggregatedNumericalMathEvaluationImplementation::getOutputDimension() const
{
  return outputDimension_;
}


/* Gradient according to the marginal parameters */
Matrix AggregatedNumericalMathEvaluationImplementation::parameterGradient(const NumericalPoint & inP) const
{
  Matrix result(getParameter().getDimension(), getOutputDimension());
  const UnsignedInteger size = functionsCollection_.getSize();
  UnsignedInteger rowShift = 0;
  UnsignedInteger columnShift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Matrix currentGradient(functionsCollection_[i].parameterGradient(functionsCollection_[i].getParameter()));
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
NumericalPoint AggregatedNumericalMathEvaluationImplementation::getParameter() const
{
  NumericalPoint parameter;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    parameter.add(functionsCollection_[i].getParameter());
  }
  return parameter;
}

void AggregatedNumericalMathEvaluationImplementation::setParameter(const NumericalPoint & parameter)
{
  const UnsignedInteger size = functionsCollection_.getSize();
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    NumericalPoint marginalParameter(functionsCollection_[i].getParameter());
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
Description AggregatedNumericalMathEvaluationImplementation::getParameterDescription() const
{
  Description description;
  const UnsignedInteger size = functionsCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    description.add(functionsCollection_[i].getParameterDescription());
  }
  return description;
}

/* Method save() stores the object through the StorageManager */
void AggregatedNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "functionsCollection_", functionsCollection_ );
  adv.saveAttribute( "outputDimension_", outputDimension_ );
}


/* Method load() reloads the object from the StorageManager */
void AggregatedNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "functionsCollection_", functionsCollection_ );
  adv.loadAttribute( "outputDimension_", outputDimension_ );
}


END_NAMESPACE_OPENTURNS
