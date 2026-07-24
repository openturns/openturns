//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningGMMFunction implements parent class for 
 *  active learning "Generalized Maximin" criterion used in reliability analysis
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

#include "openturns/ActiveLearningGMMFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningGMMFunction
 */
 
CLASSNAMEINIT(ActiveLearningGMMFunction)

static const Factory<ActiveLearningGMMFunction> Factory_ActiveLearningGMMFunction;

 /** Default constructor */
ActiveLearningGMMFunction::ActiveLearningGMMFunction()
  :ActiveLearningReliabilityFunction()
  {
    // Nothing to do
  } 

/* Constructor with parameters */
ActiveLearningGMMFunction::ActiveLearningGMMFunction(const Scalar reliabilityThreshold,
                                                     const Scalar learningThreshold,
                                                     const Distribution & inputDistribution,
                                                     const Scalar outputQuantileLevel)
    : ActiveLearningReliabilityFunction(reliabilityThreshold,
                                        learningThreshold)
    , inputDistribution_(inputDistribution)
    , outputQuantileLevel_(outputQuantileLevel)
  {
  }

/* Virtual constructor */
ActiveLearningGMMFunction * ActiveLearningGMMFunction::clone() const
{
  return new ActiveLearningGMMFunction(*this);
}


/* String converter */
String ActiveLearningGMMFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << ActiveLearningReliabilityFunction::__repr__();
  return oss;
}


/* Compute U criterion for one sample */
Scalar ActiveLearningGMMFunction::computeAsScalar(const Point & x) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningGMMFunction::computeAsScalar(const Point & x)";
}


Sample ActiveLearningGMMFunction::operator()(const Sample & inputSample, const Sample & inputDoE) const 
{

  const UnsignedInteger inputSampleSize = inputSample.getSize();
  const UnsignedInteger inputDoESize = inputDoE.getSize();
  const UnsignedInteger dimension = inputSample.getDimension();
  // computation of quantile of abs (\hat(g) - t)
  
  Sample scaledOutputSample(inputSampleSize, 1);
  const Sample mx = gprCov_.getConditionalMean(inputSample);
  
  for (UnsignedInteger i = 0; i < inputSampleSize; ++i)
    {
      Scalar meanGPValue = mx[i][0];
      scaledOutputSample[i][0] = std::abs(meanGPValue - reliabilityThreshold_);
    }
    
  Scalar outputQuantile = scaledOutputSample.computeQuantile(outputQuantileLevel_)[0];

  Sample outS(inputSampleSize, 1);
  
  // compute min distance between inputSample and inputDoE weighted by PDF
  for (UnsignedInteger i = 0; i < inputSampleSize; ++i)
  {
    Point inputSample_i = inputSample[i];
    
    Scalar meanGPValue = mx[i][0];
    Scalar scaledGPValue = std::abs(meanGPValue - reliabilityThreshold_);
    if (scaledGPValue > outputQuantile) // check if current sample is near the limit state
    {
      outS(i, 0) = 0.;
    }
    else
    { 
      Scalar currentMin;
  
      for (UnsignedInteger j = 0; j < inputDoESize; ++ j)
        {
          Scalar distSquared = 0;
          
          for (UnsignedInteger k = 0; k < dimension; ++ k)
          {
            distSquared += std::pow(inputSample_i[k] - inputDoE[j][k], 2);
          }
          
          Scalar dist = std::sqrt(distSquared);
      
          if (j == 0)
          {
            currentMin = dist * std::pow(inputDistribution_.computePDF(inputSample_i), 1./dimension) ;
          }
          else
          {
            Scalar proposalMin = dist* std::pow(inputDistribution_.computePDF(inputSample_i), 1./dimension);
          
          if ( proposalMin < currentMin)
            {
              currentMin = proposalMin;
            }
          }
        outS(i, 0) = currentMin;
        }
      }
  }
  return outS;
}

/* Compute U criterion for one sample */
Bool ActiveLearningGMMFunction::checkConvergenceLearning(const Sample & criterionValues) const 
{
  Scalar max_GMM = criterionValues.getMax()[0];
  
  if (max_GMM <=learningThreshold_)
  {
    return true;
  }
  else
  {
    return false;
  } 
}

/* Return sample corresponding to GMM criterion*/
Sample ActiveLearningGMMFunction::getInfillSample(const Sample & inputSample,
                                                  const Sample & criterionValues) const 
{
  Indices indextoReturn(1);
  indextoReturn[0] = criterionValues.argsort(false)[0];
 
  Sample sampleToReturn = Sample(inputSample.select(indextoReturn));

  return sampleToReturn;
}


  
  
END_NAMESPACE_OPENTURNS
