//                                               -*- C++ -*-
/**
 *  @brief StandardSpaceCrossEntropyImportanceSampling implement class for Cross Entropy Importance Sampling algorithms in Standard Space
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
#include "openturns/StandardSpaceCrossEntropyImportanceSampling.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/EventSimulation.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StandardSpaceCrossEntropyImportanceSampling
 */

CLASSNAMEINIT(StandardSpaceCrossEntropyImportanceSampling)

static const Factory<StandardSpaceCrossEntropyImportanceSampling> Factory_StandardSpaceCrossEntropyImportanceSampling;

// Default constructor
StandardSpaceCrossEntropyImportanceSampling::StandardSpaceCrossEntropyImportanceSampling()
  : CrossEntropyImportanceSampling()
{
  // Nothing TO DO
}


/* Virtual constructor */
StandardSpaceCrossEntropyImportanceSampling * StandardSpaceCrossEntropyImportanceSampling::clone() const
{
  return new StandardSpaceCrossEntropyImportanceSampling(*this);
}

// Default constructor
StandardSpaceCrossEntropyImportanceSampling::StandardSpaceCrossEntropyImportanceSampling(const RandomVector & event,
                                                                                         const Scalar rhoQuantile)
  : CrossEntropyImportanceSampling(event,rhoQuantile)
  {

  initialDistribution_ = Normal(initialDistribution_.getDimension());
  auxiliaryDistribution_ = Normal(initialDistribution_.getDimension());
  rhoQuantile_ = (event.getOperator()(0, 1) ? rhoQuantile : 1.0 - rhoQuantile);
}

// Compute Output Samples
Sample StandardSpaceCrossEntropyImportanceSampling::computeOutputSamples(const Sample & inputSamples) const
{
  Sample outputSamples = getEvent().getFunction()(getEvent().getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation()(inputSamples));
  return outputSamples;
}

// Update auxiliary distribution
void StandardSpaceCrossEntropyImportanceSampling::updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters)
{
  Point temporaryParameters = auxiliaryDistribution_.getParameter();
  
  for (UnsignedInteger i = 0; i < auxiliaryDistributionParameters.getDimension(); ++i)
  {
    temporaryParameters[i] = auxiliaryDistributionParameters[i];
  }
  auxiliaryDistribution_.setParameter(temporaryParameters);
} 
  
// Optimize auxiliary distribution parameters
Point StandardSpaceCrossEntropyImportanceSampling::optimizeAuxiliaryDistributionParameters(const Sample & auxiliaryCriticInputSamples) const
{
  //evaluate initial PDF on failure auxiliaryInputSamples
  Point criticSamplesInitialPDFValue   = initialDistribution_.computePDF(auxiliaryCriticInputSamples).asPoint(); 
  //evaluate auxiliary PDF on failure auxiliaryInputSamples
  Point criticSamplesAuxiliaryPDFValue = auxiliaryDistribution_.computePDF(auxiliaryCriticInputSamples).asPoint(); 

  //evaluate initial PDF on failure auxiliaryInputSamples
  Point criticSamplesInitialLogPDFValue   = initialDistribution_.computeLogPDF(auxiliaryCriticInputSamples).asPoint();
  //evaluate auxiliary PDF on failure auxiliaryInputSamples 
  Point criticSamplesAuxiliaryLogPDFValue = auxiliaryDistribution_.computeLogPDF(auxiliaryCriticInputSamples).asPoint();
  
  // calculation of denominator of estimators
  Scalar sumPdfCritic = 0.0;
  Point PDFRatio(auxiliaryCriticInputSamples.getSize()) ;
  
  for(UnsignedInteger i = 0; i < auxiliaryCriticInputSamples.getSize(); ++i)
  {
    PDFRatio[i] = std::exp(criticSamplesInitialLogPDFValue[i] - criticSamplesAuxiliaryLogPDFValue[i]);
    sumPdfCritic += PDFRatio[i] ;
  }
  
  // Calculation of updated mean of auxiliary distribution
  Point mean(auxiliaryCriticInputSamples.getDimension());
  
  for(UnsignedInteger i = 0; i < auxiliaryCriticInputSamples.getDimension(); ++i)
  {
  
    Scalar numeratorMeanCalculation = 0.0;
  
    for(UnsignedInteger j = 0; j < auxiliaryCriticInputSamples.getSize(); ++j)
    {
      numeratorMeanCalculation += std::exp(criticSamplesInitialLogPDFValue[j] - criticSamplesAuxiliaryLogPDFValue[j])* auxiliaryCriticInputSamples(j,i);
    }
    
    mean[i]= numeratorMeanCalculation / sumPdfCritic; 
  }
 
  // Calculation of updated standard deviation of auxiliary distribution
  Point standardDeviation(auxiliaryCriticInputSamples.getDimension());
  
  for(UnsignedInteger i = 0; i < auxiliaryCriticInputSamples.getDimension(); ++i)
  {
    Point diff(auxiliaryCriticInputSamples.getSize());
  
    for(UnsignedInteger k=0; k < auxiliaryCriticInputSamples.getSize();++k)
    {
      diff[k] = std::pow(auxiliaryCriticInputSamples.getMarginal(i)(k,0) - mean[i],2);
    }

    Scalar numeratorStdCalculation = 0.0;
    for(UnsignedInteger j = 0; j < auxiliaryCriticInputSamples.getSize(); ++j)
    {
      numeratorStdCalculation += criticSamplesInitialPDFValue[j]*diff[j] / criticSamplesAuxiliaryPDFValue[j];
    }
    standardDeviation[i] = std::sqrt(numeratorStdCalculation/sumPdfCritic);
  }

 Point auxiliaryParameters = Point(2*auxiliaryCriticInputSamples.getDimension());
 
 for(UnsignedInteger i = 0; i < auxiliaryCriticInputSamples.getDimension(); ++i)
 {
   auxiliaryParameters[2*i] = mean[i];
   auxiliaryParameters[2 * i + 1] = standardDeviation[i]; 
 } 

 
return auxiliaryParameters;
}

END_NAMESPACE_OPENTURNS
