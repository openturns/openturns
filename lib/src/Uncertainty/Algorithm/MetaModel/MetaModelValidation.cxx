//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel validation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/MetaModelValidation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Cloud.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetaModelValidation)

static const Factory<MetaModelValidation> Factory_MetaModelValidation;

/* Default constructor */
MetaModelValidation::MetaModelValidation()
  : PersistentObject()
  , inputSample_()
  , outputSample_()
  , metaModel_()
  , isInitialized_(false)
  , residual_()
  , q2_()
{
  // Nothing to do
}

/* Constructor with parameters */
MetaModelValidation::MetaModelValidation(const Sample & inputSample,
    const Sample & outputSample,
    const Function & metaModel)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , metaModel_(metaModel)
  , isInitialized_(false)
  , residual_()
  , q2_()
{
  if (inputSample_.getSize() != outputSample_.getSize())
    throw InvalidArgumentException(HERE) << "Input & output samples have different size."
                                         << " Input size = " << inputSample_.getSize()
                                         << ", output size = " << outputSample_.getSize();
  if (outputSample_.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Output sample should be of dimension 1";

  if (inputSample_.getDimension() != metaModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << "Input sample have different size from metamodel."
                                         << " Input sample dimension = " << inputSample_.getDimension()
                                         << ", metamodel input dimension = " << metaModel_.getInputDimension();

  if (metaModel_.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "Metamodel output dimension should be 1. Here, dim = " << metaModel_.getOutputDimension();

}

/* Virtual constructor */
MetaModelValidation * MetaModelValidation::clone() const
{
  return new MetaModelValidation(*this);
}

/* String converter */
String MetaModelValidation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " input validation sample = " << inputSample_
      << " output validation sample = " << outputSample_
      << " metaModel = " << metaModel_;
  return oss;
}

void MetaModelValidation::initialize() const
{
  // Initialize
  // We compute first the residual sample
  // This last one is stored and retured by getResidualSample method
  // From this, it derives also the predictive factor i.e. 1 - RSS/SS,
  // RSS = Residual Sum of Squares, SS = Sum of Squares
  residual_ = outputSample_ - metaModel_(inputSample_);
  q2_ = 1.0 - residual_.computeRawMoment(2)[0] / outputSample_.computeCenteredMoment(2)[0];
  isInitialized_ = true;
}

Sample MetaModelValidation::getInputSample() const
{
  return inputSample_;
}

Sample MetaModelValidation::getOutputSample() const
{
  return outputSample_;
}

Scalar MetaModelValidation::computePredictivityFactor() const
{
  if (!isInitialized_) initialize();
  return q2_;
}

/* Get residual sample */
Sample MetaModelValidation::getResidualSample() const
{
  if (!isInitialized_) initialize();
  return residual_;
}

/* Get residual distribution */
Distribution MetaModelValidation::getResidualDistribution(const Bool smooth) const
{
  if (!isInitialized_) initialize();
  if (!smooth)
  {
    return HistogramFactory().build(residual_);
  }
  return KernelSmoothing().build(residual_);
}

/* Draw model vs metamodel validation graph */
Graph MetaModelValidation::drawValidation() const
{
  // Build the first drawable
  const Sample yhat = metaModel_(inputSample_);
  Curve curve(outputSample_, outputSample_);
  curve.setColor("blue");
  Cloud cloud(outputSample_, yhat);
  // set color
  cloud.setColor("red");
  Graph graph(OSS() << "Metamodel validation - Q2 = " <<q2_ * 100 << " %" , "model", "metamodel", true);
  // Add drawables
  graph.add(curve);
  graph.add(cloud);
  return graph;
}

/* Method save() stores the object through the StorageManager */
void MetaModelValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "metaModel_", metaModel_ );
  adv.saveAttribute( "isInitialized_", isInitialized_ );
  adv.saveAttribute( "residual_", residual_ );
  adv.saveAttribute( "q2_", q2_ );
}

/* Method load() reloads the object from the StorageManager */
void MetaModelValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "metaModel_", metaModel_ );
  adv.loadAttribute( "isInitialized_", isInitialized_ );
  adv.loadAttribute( "residual_", residual_ );
  adv.loadAttribute( "q2_", q2_ );
}

END_NAMESPACE_OPENTURNS
