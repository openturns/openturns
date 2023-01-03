//                                               -*- C++ -*-
/**
 *  @brief An implementation from samples
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DatabaseEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DatabaseEvaluation)

static const Factory<DatabaseEvaluation> Factory_DatabaseEvaluation;


/* Default constructor */
DatabaseEvaluation::DatabaseEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Default constructor */
DatabaseEvaluation::DatabaseEvaluation(const Sample & inputSample,
                                       const Sample & outputSample)
  : EvaluationImplementation()
{
  setSample(inputSample, outputSample);
}


/* Virtual constructor */
DatabaseEvaluation * DatabaseEvaluation::clone() const
{
  return new DatabaseEvaluation(*this);
}


/* Comparison operator */
Bool DatabaseEvaluation::operator ==(const DatabaseEvaluation & other) const
{
  if (this == &other) return true;
  return (inputSample_ == other.inputSample_) && (outputSample_ == other.outputSample_);
}


/* String converter */
String DatabaseEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DatabaseEvaluation::GetClassName()
      << " name=" << getName()
      << " inputSample=" << inputSample_
      << " outputSample" << outputSample_;
  return oss;
}

String DatabaseEvaluation::__str__( const String & offset ) const
{
  OSS oss(false);
  oss << DatabaseEvaluation::GetClassName() << Os::GetEndOfLine() << offset
      << "  input sample :" << Os::GetEndOfLine() << offset
      << inputSample_.__str__(offset) << Os::GetEndOfLine() << offset
      << "  output sample :" << Os::GetEndOfLine() << offset
      << outputSample_.__str__(offset);
  return oss;
}

/* Input sample accessor */
void DatabaseEvaluation::setInputSample(const Sample &inputSample)
{
  setSample( inputSample, outputSample_ );
}

Sample DatabaseEvaluation::getInputSample() const
{
  return inputSample_;
}


/* Output sample accessor */
void DatabaseEvaluation::setOutputSample(const Sample &outputSample)
{
  setSample( inputSample_, outputSample );
}

Sample DatabaseEvaluation::getOutputSample() const
{
  return outputSample_;
}


void DatabaseEvaluation::setSample(const Sample & inputSample,
                                   const Sample & outputSample)
{
  const UnsignedInteger size = inputSample.getSize();

  if(!(size > 0))
    throw InvalidDimensionException(HERE) << "Empty input sample";

  if (!(outputSample.getSize() > 0))
    throw InvalidDimensionException(HERE) << "Empty output sample";

  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidDimensionException(HERE) << "Input and output samples have different sizes (in=" << inputSample.getSize() << " out=" << outputSample.getSize() << ")";

  if (inputSample_ != inputSample)
  {
    inputSample_ = inputSample;
    setInputDescription(inputSample.getDescription());
  }
  if (outputSample_ != outputSample)
  {
    outputSample_ = outputSample;
    setOutputDescription(outputSample.getDescription());
  }

  // Build nearest neighbor algorithm
  if (nearestNeighbour_.getSample() != inputSample_)
    nearestNeighbour_.setSample(inputSample_);
}

NearestNeighbourAlgorithm DatabaseEvaluation::getNearestNeighbourAlgorithm() const
{
  return nearestNeighbour_;
}

void DatabaseEvaluation::setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & tree)
{
  NearestNeighbourAlgorithm emptyClone(tree.getImplementation()->emptyClone());
  nearestNeighbour_.swap(emptyClone);
  nearestNeighbour_.setSample(inputSample_);
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point DatabaseEvaluation::operator()( const Point & inP ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  Point result(outputSample_[nearestNeighbour_.query(inP)]);
  callsNumber_.increment();
  return result;
}

Sample DatabaseEvaluation::operator()( const Sample & inS ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  Sample result;
  if (inS == inputSample_) result = outputSample_;
  else result = EvaluationImplementation::operator()(inS);
  callsNumber_.fetchAndAdd(inS.getSize());
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger DatabaseEvaluation::getInputDimension() const
{
  return inputSample_.getDimension();
}


/* Accessor for output point dimension */
UnsignedInteger DatabaseEvaluation::getOutputDimension() const
{
  return outputSample_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void DatabaseEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("nearestNeighbour_", nearestNeighbour_);
}

/* Method load() reloads the object from the StorageManager */
void DatabaseEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("nearestNeighbour_", nearestNeighbour_);
  setSample(inputSample_, outputSample_);
}

END_NAMESPACE_OPENTURNS
