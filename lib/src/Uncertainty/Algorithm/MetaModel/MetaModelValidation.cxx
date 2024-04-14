//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel validation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SpecFunc.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/BernsteinCopulaFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetaModelValidation)

static const Factory<MetaModelValidation> Factory_MetaModelValidation;

/* Default constructor */
MetaModelValidation::MetaModelValidation()
  : PersistentObject()
{
  // Nothing to do
}

/* @deprecated Constructor with parameters */
MetaModelValidation::MetaModelValidation(const Sample & inputSample,
    const Sample & outputSample,
    const Function & metaModel)
  : PersistentObject()
  , outputSample_(outputSample)
  , metaModelPredictions_(metaModel(inputSample))
{
  if (inputSample.getSize() != outputSample_.getSize())
    throw InvalidArgumentException(HERE) << "Input sample size (" << inputSample.getSize() << ")"
                                         << " should match output sample size (" << outputSample_.getSize() << ")";
  if (inputSample.getSize() < 2)
    throw NotDefinedException(HERE) << "R2 computation needs at least 2 values";
  if (inputSample.getDimension() != metaModel.getInputDimension())
    throw InvalidArgumentException(HERE) << "Metamodel input dimension (" << metaModel.getInputDimension() << ")"
                                         <<  " should match input sample dimension (" << inputSample.getDimension() << ")";
  if (outputSample_.getDimension() != metaModel.getOutputDimension())
    throw InvalidArgumentException(HERE) << "Metamodel output dimension (" << metaModel.getOutputDimension() << ")"
                                         <<  " should match output sample dimension (" << outputSample_.getDimension() << ")";
  LOGWARN(OSS() << "MetaModelValidation(inputSample, outputSample, metaModel) is deprecated in favor of MetaModelValidation(outputSample, metamodelPredictions)");
  initialize();
}

/* Constructor with parameters */
MetaModelValidation::MetaModelValidation(
    const Sample & outputSample,
    const Sample & metamodelPredictions)
  : PersistentObject()
  , outputSample_(outputSample)
  , metaModelPredictions_(metamodelPredictions)
{
  if (outputSample_.getSize() != metaModelPredictions_.getSize())
    throw InvalidArgumentException(HERE) << "Output sample size (" << outputSample_.getSize() << ")"
                                         << " should match metamodel predictions sample size (" << metaModelPredictions_.getSize() << ")";
  if (outputSample_.getSize() < 2)
    throw NotDefinedException(HERE) << "R2 computation needs at least 2 values";
  if (outputSample_.getDimension() != metaModelPredictions_.getDimension())
    throw InvalidArgumentException(HERE) << "Metamodel output dimension (" << metaModelPredictions_.getDimension() << ")"
                                         <<  " should match output sample dimension (" << outputSample_.getDimension() << ")";
  initialize();
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
      << " output validation sample = " << outputSample_
      << " metaModel predictions = " << metaModelPredictions_
      << " r2Score_ = " << r2Score_
      << " meanSquaredError_ = " << meanSquaredError_;
  return oss;
}

void MetaModelValidation::initialize() const
{
  // Initialize
  // We compute first the residual sample
  // This last one is stored and returned by getResidualSample method
  // From this, it derives also the predictive factor i.e. 1 - RSS/SS,
  // RSS = Residual Sum of Squares, SS = Sum of Squares
  residual_ = outputSample_ - metaModelPredictions_;
  meanSquaredError_ = residual_.computeRawMoment(2);
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const Point sampleVariance(outputSample_.computeCentralMoment(2));
  Point r2Score(outputDimension);
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    if (std::abs(sampleVariance[j]) == 0.0)
      throw NotDefinedException(HERE) << "Q2 cannot be computed on constant output component index " << j;
    r2Score[j] = 1.0 - meanSquaredError_[j] / sampleVariance[j];
  }
  r2Score_ = r2Score;
}

Sample MetaModelValidation::getOutputSample() const
{
  return outputSample_;
}

Sample MetaModelValidation::getMetamodelPredictions() const
{
  return metaModelPredictions_;
}

Point MetaModelValidation::computePredictivityFactor() const
{
  LOGWARN(OSS() << "computePredictivityFactor is deprecated");
  return computeR2Score();
}

Point MetaModelValidation::computeR2Score() const
{
  return r2Score_;
}

Point MetaModelValidation::computeMeanSquaredError() const
{
  return meanSquaredError_;
}

/* Get residual sample */
Sample MetaModelValidation::getResidualSample() const
{
  return residual_;
}

/* Get residual distribution */
Distribution MetaModelValidation::getResidualDistribution(const Bool smooth) const
{
  if (!smooth)
  {
    const UnsignedInteger dimension = residual_.getDimension();
    JointDistribution::DistributionCollection coll(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      coll[j] = HistogramFactory().build(residual_.getMarginal(j));
    // Estimate a copula only if dimension>1
    if (dimension > 1)
      return JointDistribution(coll, BernsteinCopulaFactory().build(residual_));
    return coll[0];
  }
  return KernelSmoothing().build(residual_);
}

/* Draw model vs metamodel validation graph */
GridLayout MetaModelValidation::drawValidation() const
{
  // Build the first drawable
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  Point minS(outputSample_.getMin());
  Point maxS(outputSample_.getMax());
  GridLayout grid(1, outputDimension);
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph("", OSS() << "model " << j, j == 0 ? "metamodel" : "", true);

    // diagonal
    Sample diagonalPoints(2, 2);
    diagonalPoints[0] = Point(2, minS[j]);
    diagonalPoints[1] = Point(2, maxS[j]);
    Curve diagonal(diagonalPoints);
    graph.add(diagonal);

    // points
    Cloud cloud(outputSample_.getMarginal(j), metaModelPredictions_.getMarginal(j));
    graph.add(cloud);

    grid.setGraph(0, j, graph);
  }
  grid.setTitle(OSS() << "Metamodel validation - n = " << outputSample_.getSize());
  return grid;
}

/* Method save() stores the object through the StorageManager */
void MetaModelValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "metaModelPredictions_", metaModelPredictions_ );
  adv.saveAttribute( "residual_", residual_ );
  adv.saveAttribute( "r2Score_", r2Score_ );
  adv.saveAttribute( "meanSquaredError_", meanSquaredError_ );
}

/* Method load() reloads the object from the StorageManager */
void MetaModelValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "outputSample_", outputSample_ );
  if (adv.hasAttribute("metaModelPredictions_"))
    adv.loadAttribute( "metaModelPredictions_", metaModelPredictions_ );
  else
    adv.loadAttribute( "outputSample_", metaModelPredictions_ );
  adv.loadAttribute( "residual_", residual_ );
  if (adv.hasAttribute("r2Score_"))
    adv.loadAttribute( "r2Score_", r2Score_ );
  else
    adv.loadAttribute( "q2_", r2Score_ );
  if (adv.hasAttribute("meanSquaredError_"))
    adv.loadAttribute( "meanSquaredError_", meanSquaredError_ );
  else
  {
    const UnsignedInteger outputDimension = outputSample_.getDimension();
    if (r2Score_.getDimension() != outputDimension)
      throw NotDefinedException(HERE) << "The dimension of the R2 score is " << r2Score_.getDimension() 
                                      << " but the dimension of the output sample is " << outputDimension;
    const Point sampleVariance(outputSample_.computeCentralMoment(2));
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      meanSquaredError_[j] = sampleVariance[j] * (1.0 - r2Score_[j]);
  }
}

END_NAMESPACE_OPENTURNS
