//                                               -*- C++ -*-
/**
 *  @brief PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm implement class for Cross Entropy Importance Sampling algorithms in Physical Space
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
#include "openturns/PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TNC.hxx"
#include "openturns/OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm
 */

CLASSNAMEINIT(PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm)


static const Factory<PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm> Factory_PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm;


// Default constructor
PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm::PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm()
  : CrossEntropyImportanceSamplingAlgorithm()
{
  // Nothing TO DO
}



PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm::PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm(const RandomVector & event,
                                                         const Point & activeParameters,
                                                         const Point & initialAuxiliaryDistributionParameters,
                                                         const Interval & bounds,
                                                         const Distribution & auxiliaryDistribution,
                                                         const Scalar rhoQuantile)
  : CrossEntropyImportanceSamplingAlgorithm(event, rhoQuantile)
  , solver_(new TNC)
  , useDefaultSolver_(true)
  {
    auxiliaryDistribution_ = auxiliaryDistribution;
    rhoQuantile_ = (event.getOperator()(0, 1) ? rhoQuantile : 1.0 - rhoQuantile);
    bounds_ = bounds;

    activeParameters_ = activeParameters;
  
    Point parameters_(auxiliaryDistribution_.getParameter());
    for (UnsignedInteger i = 0; i < activeParameters_.getDimension(); ++i)
    {
      parameters_[activeParameters_[i]] = initialAuxiliaryDistributionParameters[i];
    }      
    auxiliaryDistribution_.setParameter(parameters_);
  }


/* Virtual constructor */
PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm * PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm::clone() const
{
  return new PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm(*this);
}

namespace {
class KullbackLeiblerDivergenceObjective : public EvaluationImplementation
{
public:
  KullbackLeiblerDivergenceObjective(const Sample auxiliaryCriticInputSample,
                                    const Point & initialCriticInputSamplePDFValue,
                                    const Distribution & auxiliaryDistribution,
                                    const Point & activeParameters,
                                    const Scalar numberSamples)
  : EvaluationImplementation()
  , auxiliaryCriticInputSample_(auxiliaryCriticInputSample)
  , activeParameters_(activeParameters)
  , auxiliaryDistribution_(auxiliaryDistribution)
  , initialCriticInputSamplePDFValue_(initialCriticInputSamplePDFValue)
  , numberSamples_(numberSamples)
  {
    // Nothing to do
  }
  
  virtual KullbackLeiblerDivergenceObjective * clone () const
  {
    return new KullbackLeiblerDivergenceObjective(*this);
  }
  
  Point operator()(const Point & x) const
  {
  

  Distribution myDistrib = auxiliaryDistribution_;
  
  // update auxiliary distribution
  Point parameters_(auxiliaryDistribution_.getParameter());
  
  for (UnsignedInteger i = 0; i < activeParameters_.getDimension(); ++i)
  {
       parameters_[activeParameters_[i]] = x[i];
  }
  myDistrib.setParameter(parameters_);
  
  Point criticSamplesAuxiliaryPDFValue = myDistrib.computePDF(auxiliaryCriticInputSample_).asPoint();
  Point criticSamplesAuxiliaryLogPDFValue = myDistrib.computeLogPDF(auxiliaryCriticInputSample_).asPoint();
  
  Scalar objectiveFunction = 0.0;
  
  for(UnsignedInteger j = 0; j < auxiliaryCriticInputSample_.getSize(); ++j)
  {
     objectiveFunction += initialCriticInputSamplePDFValue_[j]/criticSamplesAuxiliaryPDFValue[j]*criticSamplesAuxiliaryLogPDFValue[j];
  }
  objectiveFunction = 1/numberSamples_*objectiveFunction;
  
  return Point(1,objectiveFunction);
  }
  
  UnsignedInteger getInputDimension() const
  {
    return activeParameters_.getDimension();
  }
  
    UnsignedInteger getOutputDimension() const
  {
    return 1;
  }
  
  
protected :
Sample auxiliaryCriticInputSample_;
Point activeParameters_;
Distribution auxiliaryDistribution_;
Point initialCriticInputSamplePDFValue_;
Scalar numberSamples_;
};
}


// Compute Output Samples
Sample PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm::computeOutputSamples(const Sample & inputSamples) const
{


  Sample outputSamples = getEvent().getFunction()(inputSamples);

  return outputSamples;
}


// Update auxiliary distribution
void PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm::updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters)
{

  Point parameters_(auxiliaryDistribution_.getParameter());
  for (UnsignedInteger i = 0; i < activeParameters_.getDimension(); ++i)
  {
       parameters_[activeParameters_[i]] = auxiliaryDistributionParameters[i];
         }
  auxiliaryDistribution_.setParameter(parameters_);
} 

// Optimize auxiliary distribution parameters
Point PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm::optimizeAuxiliaryDistributionParameters(const Sample & auxiliaryCriticInputSamples) const
{

Point initialCriticInputSamplePDFValue = initialDistribution_.computePDF(auxiliaryCriticInputSamples).asPoint();

const UnsignedInteger numberOfSample = getMaximumOuterSampling() * getBlockSize();


Function objective(new KullbackLeiblerDivergenceObjective(auxiliaryCriticInputSamples,
                                                          initialCriticInputSamplePDFValue,
                                                          auxiliaryDistribution_,
                                                          activeParameters_,
                                                          numberOfSample));


OptimizationProblem problem(objective);
problem.setBounds(bounds_);
problem.setMinimization(false);
solver_.setProblem(problem);


Point param(activeParameters_.getDimension());


  for (UnsignedInteger i = 0; i < activeParameters_.getDimension(); ++i)
  {
       param[i] = auxiliaryDistribution_.getParameter()[activeParameters_[i]];
  }

solver_.setStartingPoint(param);

solver_.run();

Point auxiliaryParameters = solver_.getResult().getOptimalPoint();

return auxiliaryParameters;
}


END_NAMESPACE_OPENTURNS	
