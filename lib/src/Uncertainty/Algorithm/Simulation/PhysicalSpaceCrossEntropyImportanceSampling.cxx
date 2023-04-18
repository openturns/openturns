//                                               -*- C++ -*-
/**
 *  @brief PhysicalSpaceCrossEntropyImportanceSampling implement class for Cross Entropy Importance Sampling algorithms in Physical Space
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
#include "openturns/PhysicalSpaceCrossEntropyImportanceSampling.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/NLopt.hxx"
BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PhysicalSpaceCrossEntropyImportanceSampling
 */

CLASSNAMEINIT(PhysicalSpaceCrossEntropyImportanceSampling)


static const Factory<PhysicalSpaceCrossEntropyImportanceSampling> Factory_PhysicalSpaceCrossEntropyImportanceSampling;


// Default constructor
PhysicalSpaceCrossEntropyImportanceSampling::PhysicalSpaceCrossEntropyImportanceSampling()
  : CrossEntropyImportanceSampling()
{
  // Nothing TO DO
}



PhysicalSpaceCrossEntropyImportanceSampling::PhysicalSpaceCrossEntropyImportanceSampling(const RandomVector & event,
                                                                                         const Distribution & auxiliaryDistribution,
                                                                                         const Indices & activeParameters,
                                                                                         const Point & initialAuxiliaryDistributionParameters,
                                                                                         const Interval & bounds,
                                                                                         const Scalar quantileLevel)
  : CrossEntropyImportanceSampling(event, quantileLevel)
  , activeParameters_(activeParameters)
  , solver_(new NLopt("LD_LBFGS"))
  {
    auxiliaryDistribution_ = auxiliaryDistribution;
    quantileLevel_ = (event.getOperator()(0, 1) ? quantileLevel : 1.0 - quantileLevel);
    bounds_ = bounds;

    Point parameters(auxiliaryDistribution_.getParameter());
    
    if (activeParameters_.getSize() != bounds.getDimension())
    throw InvalidArgumentException(HERE) << "In PhysicalSpaceCrossEntropyImportanceSampling::PhysicalSpaceCrossEntropyImportanceSampling, active parameters size (" << activeParameters_.getSize() << ") does not match initial auxiliary distribution parameters bounds dimension (" << bounds.getDimension() << ").";
    
        if (activeParameters_.getSize() != initialAuxiliaryDistributionParameters.getDimension())
    throw InvalidArgumentException(HERE) << "In PhysicalSpaceCrossEntropyImportanceSampling::PhysicalSpaceCrossEntropyImportanceSampling, active parameters size (" << activeParameters_.getSize() << ") does not match initial auxiliary distribution parameters initial values (" << initialAuxiliaryDistributionParameters.getSize() << ").";
    
    if (activeParameters_.getSize() > auxiliaryDistribution_.getParameter().getSize())
    throw InvalidArgumentException(HERE) << "In PhysicalSpaceCrossEntropyImportanceSampling::PhysicalSpaceCrossEntropyImportanceSampling, active parameters size (" << activeParameters_.getSize() << ") is greater than auxiliary distribution size (" << initialAuxiliaryDistributionParameters.getSize() << ").";

    if (activeParameters_.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In PhysicalSpaceCrossEntropyImportanceSampling::PhysicalSpaceCrossEntropyImportanceSampling, active parameters size (" << activeParameters_.getSize() << " has to be greater than zero";    
    
    
    for (UnsignedInteger i = 0; i < activeParameters_.getSize(); ++i)
    {
      parameters[activeParameters_[i]] = initialAuxiliaryDistributionParameters[i];
    }      
    auxiliaryDistribution_.setParameter(parameters);
  }


/* Virtual constructor */
PhysicalSpaceCrossEntropyImportanceSampling * PhysicalSpaceCrossEntropyImportanceSampling::clone() const
{
  return new PhysicalSpaceCrossEntropyImportanceSampling(*this);
}

namespace {
class KullbackLeiblerDivergenceObjective : public EvaluationImplementation
{
public:
  KullbackLeiblerDivergenceObjective(const Sample auxiliaryCriticInputSample,
                                     const Point & initialCriticInputSamplePDFValue,
                                     const Distribution & auxiliaryDistribution,
                                     const Indices & activeParameters,
                                     const UnsignedInteger numberSamples)
  : EvaluationImplementation()
  , auxiliaryCriticInputSample_(auxiliaryCriticInputSample)
  , activeParameters_(activeParameters)
  , auxiliaryDistribution_(auxiliaryDistribution)
  , initialCriticInputSamplePDFValue_(initialCriticInputSamplePDFValue)
  , numberSamples_(numberSamples)
  {
    // Nothing to do
  }
  
  KullbackLeiblerDivergenceObjective * clone () const override
  {
    return new KullbackLeiblerDivergenceObjective(*this);
  }
  
  Point operator()(const Point & x) const override
  {
    Distribution distrib = auxiliaryDistribution_;
  
    // update auxiliary distribution
    Point parameters(auxiliaryDistribution_.getParameter());
    for (UnsignedInteger i = 0; i < activeParameters_.getSize(); ++i)
    {
      parameters[activeParameters_[i]] = x[i];
    }
    distrib.setParameter(parameters);
  
    const Point criticSamplesAuxiliaryPDFValue = distrib.computePDF(auxiliaryCriticInputSample_).asPoint();
    const Point criticSamplesAuxiliaryLogPDFValue = distrib.computeLogPDF(auxiliaryCriticInputSample_).asPoint();
  
    Scalar objectiveFunction = 0.0;
    for(UnsignedInteger j = 0; j < auxiliaryCriticInputSample_.getSize(); ++j)
    {  
      objectiveFunction += initialCriticInputSamplePDFValue_[j] / criticSamplesAuxiliaryPDFValue[j] * criticSamplesAuxiliaryLogPDFValue[j];
    }
    objectiveFunction = 1/numberSamples_*objectiveFunction;
  
    return Point(1,objectiveFunction);
  }
  
  UnsignedInteger getInputDimension() const override
  {
    return activeParameters_.getSize();
  }
  
  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }
  
protected :
  Sample auxiliaryCriticInputSample_;
  Indices activeParameters_;
  Distribution auxiliaryDistribution_;
  Point initialCriticInputSamplePDFValue_;
  Scalar numberSamples_;
};
}

/** Set solver */
void PhysicalSpaceCrossEntropyImportanceSampling::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
   solver_= solver;
}

/** Get solver */
OptimizationAlgorithm PhysicalSpaceCrossEntropyImportanceSampling::getOptimizationAlgorithm() const
{
  return solver_;
}


// Compute Output Samples
Sample PhysicalSpaceCrossEntropyImportanceSampling::computeOutputSamples(const Sample & inputSamples) const
{
  Sample outputSamples = getEvent().getFunction()(inputSamples);

  return outputSamples;
}


// Update auxiliary distribution
void PhysicalSpaceCrossEntropyImportanceSampling::updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters)
{
  Point parameters(auxiliaryDistribution_.getParameter());
  for (UnsignedInteger i = 0; i < activeParameters_.getSize(); ++i)
  {
    parameters[activeParameters_[i]] = auxiliaryDistributionParameters[i];
  }
  auxiliaryDistribution_.setParameter(parameters);
} 

// Optimize auxiliary distribution parameters
Point PhysicalSpaceCrossEntropyImportanceSampling::optimizeAuxiliaryDistributionParameters(const Sample & auxiliaryCriticInputSamples) const
{

  Point initialCriticInputSamplePDFValue = initialDistribution_.computePDF(auxiliaryCriticInputSamples).asPoint();

  const UnsignedInteger numberOfSample = getMaximumOuterSampling() * getBlockSize();

  Function objective(new KullbackLeiblerDivergenceObjective(auxiliaryCriticInputSamples,
                                                            initialCriticInputSamplePDFValue,
                                                            auxiliaryDistribution_,
                                                            activeParameters_,
                                                            numberOfSample));


  OptimizationAlgorithm solver = solver_;
  OptimizationProblem problem(objective);
  problem.setBounds(bounds_);
  problem.setMinimization(false);
  solver.setProblem(problem);


  Point param(activeParameters_.getSize());

  for (UnsignedInteger i = 0; i < activeParameters_.getSize(); ++i)
  {
    param[i] = auxiliaryDistribution_.getParameter()[activeParameters_[i]];
  }  
  solver.setStartingPoint(param);

  solver.run();

  Point auxiliaryParameters = solver.getResult().getOptimalPoint();

  return auxiliaryParameters;
}


END_NAMESPACE_OPENTURNS	
