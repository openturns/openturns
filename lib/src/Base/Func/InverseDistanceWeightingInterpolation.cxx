//                                               -*- C++ -*-
/**
 *  @brief An implementation from samples
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/InverseDistanceWeightingInterpolation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseDistanceWeightingInterpolation)

static const Factory<InverseDistanceWeightingInterpolation> Factory_InverseDistanceWeightingInterpolation;


/* Default constructor */
InverseDistanceWeightingInterpolation::InverseDistanceWeightingInterpolation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Default constructor */
InverseDistanceWeightingInterpolation::InverseDistanceWeightingInterpolation(const Sample & inputSample,
                                       const Sample & outputSample, const Scalar & p)
  : EvaluationImplementation()
{
  setSample(inputSample, outputSample);
  setP(p);
  norm_ = "norm";
}


/* Virtual constructor */
InverseDistanceWeightingInterpolation * InverseDistanceWeightingInterpolation::clone() const
{
  return new InverseDistanceWeightingInterpolation(*this);
}


/* Comparison operator */
Bool InverseDistanceWeightingInterpolation::operator ==(const InverseDistanceWeightingInterpolation & other) const
{
  if (this == &other) return true;
  return (inputSample_ == other.inputSample_) && (outputSample_ == other.outputSample_) && (p_== other.p_);
}


/* String converter */
String InverseDistanceWeightingInterpolation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseDistanceWeightingInterpolation::GetClassName()
      << " name=" << getName()
      << " inputSample=" << inputSample_
      << " outputSample" << outputSample_
      << " smoothing parameter p :" << p_;
  return oss;
}

String InverseDistanceWeightingInterpolation::__str__( const String & offset ) const
{
  OSS oss(false);
  oss << InverseDistanceWeightingInterpolation::GetClassName() << Os::GetEndOfLine() << offset
      << "  input sample :" << Os::GetEndOfLine() << offset
      << inputSample_.__str__(offset) << Os::GetEndOfLine() << offset
      << "  output sample :" << Os::GetEndOfLine() << offset
      << outputSample_.__str__(offset)
      << "smoothing parameter p :" << p_;
  return oss;
}

/* Input sample accessor */
void InverseDistanceWeightingInterpolation::setInputSample(const Sample &inputSample)
{
  setSample( inputSample, outputSample_ );
}

Sample InverseDistanceWeightingInterpolation::getInputSample() const
{
  return inputSample_;
}


/* Output sample accessor */
void InverseDistanceWeightingInterpolation::setOutputSample(const Sample &outputSample)
{
  setSample( inputSample_, outputSample );
}

Sample InverseDistanceWeightingInterpolation::getOutputSample() const
{
  return outputSample_;
}

void InverseDistanceWeightingInterpolation::setP(const Scalar &p)
{
  p_ = p;
}

Scalar InverseDistanceWeightingInterpolation::getP() const
{
  return p_;
}

void InverseDistanceWeightingInterpolation::setSample(const Sample & inputSample,
                                   const Sample & outputSample)
{
  const UnsignedInteger size = inputSample.getSize();

  if(size == 0)
    throw InvalidDimensionException(HERE) << "Empty input sample";

  if (outputSample.getSize() == 0)
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

NearestNeighbourAlgorithm InverseDistanceWeightingInterpolation::getNearestNeighbourAlgorithm() const
{
  return nearestNeighbour_;
}

void InverseDistanceWeightingInterpolation::setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & tree)
{
  NearestNeighbourAlgorithm emptyClone(tree.getImplementation()->emptyClone());
  nearestNeighbour_.swap(emptyClone);
  nearestNeighbour_.setSample(inputSample_);
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point InverseDistanceWeightingInterpolation::operator()( const Point & inP ) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  Point result(inputDimension);
  UnsignedInteger nbInputSample_ = inputSample_.getSize();
  UnsignedInteger posinP = inputSample_.find(inP);
  if(posinP!=nbInputSample_)
  {
    //point is in the inout Sample return outputSample point
    return outputSample_[posinP];
  }
  //interpolate output with the inverse distance weigthing algorithm
  Scalar sumWeight = 0.;
  if(norm_=="norm")
  {
    for(UnsignedInteger i=0;i<nbInputSample_;i++)
    {
        Scalar distance = (inputSample_[i] - inP).norm();
        Scalar weight = std::pow((1./ distance),p_);
        sumWeight += weight;
        result += outputSample_[i] * weight;
    }
  }
  else if(norm_=="norm1")
  {
    for(UnsignedInteger i=0;i<nbInputSample_;i++)
    {
        Scalar distance = (inputSample_[i] - inP).norm1();
        Scalar weight = std::pow((1./ distance),p_);
        sumWeight += weight;
        result += outputSample_[i] * weight;
    }
  }
  else if(norm_=="normInf")
  {
    for(UnsignedInteger i=0;i<nbInputSample_;i++)
    {
        Scalar distance = (inputSample_[i] - inP).normInf();
        Scalar weight = std::pow((1./ distance),p_);
        sumWeight += weight;
        result += outputSample_[i] * weight;
    }
  }
  result /= sumWeight;
  return result;
}

Sample InverseDistanceWeightingInterpolation::operator()( const Sample & inS ) const
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
UnsignedInteger InverseDistanceWeightingInterpolation::getInputDimension() const
{
  return inputSample_.getDimension();
}


/* Accessor for output point dimension */
UnsignedInteger InverseDistanceWeightingInterpolation::getOutputDimension() const
{
  return outputSample_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void InverseDistanceWeightingInterpolation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("p_", p_);
}

/* Method load() reloads the object from the StorageManager */
void InverseDistanceWeightingInterpolation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("p_", p_);
  setSample(inputSample_, outputSample_);
}

END_NAMESPACE_OPENTURNS
