//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel validation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ComposedDistribution.hxx"
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

/* Constructor with parameters */
MetaModelValidation::MetaModelValidation(const Sample & inputSample,
    const Sample & outputSample,
    const Function & metaModel)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , metaModel_(metaModel)
{
  if (inputSample_.getSize() != outputSample_.getSize())
    throw InvalidArgumentException(HERE) << "Input sample size (" << inputSample_.getSize() << ")"
                                         << " should match output sample size (" << outputSample_.getSize() << ")";
  if (inputSample_.getSize() < 2)
    throw NotDefinedException(HERE) << "Q2 computation needs at least 2 values";
  if (inputSample_.getDimension() != metaModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << "Metamodel input dimension (" << metaModel_.getInputDimension() << ")"
                                         <<  " should match input sample dimension (" << inputSample_.getDimension() << ")";
  if (outputSample_.getDimension() != metaModel_.getOutputDimension())
    throw InvalidArgumentException(HERE) << "Metamodel output dimension (" << metaModel_.getOutputDimension() << ")"
                                         <<  " should match output sample dimension (" << outputSample_.getDimension() << ")";
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

  const UnsignedInteger outputDimension = outputSample_.getDimension();
  q2_ = Point(outputDimension);
  const Point residualRawMoment2(residual_.computeRawMoment(2));
  const Point sampleVariance(outputSample_.computeCenteredMoment(2));
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    if (std::abs(sampleVariance[j]) == 0.0)
      throw NotDefinedException(HERE) << "Q2 cannot be computed on constant output component";
    q2_[j] = 1.0 - residualRawMoment2[j] / sampleVariance[j];
  }
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

Point MetaModelValidation::computePredictivityFactor() const
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
    const UnsignedInteger dimension = residual_.getDimension();
    ComposedDistribution::DistributionCollection coll(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      coll[j] = HistogramFactory().build(residual_.getMarginal(j));
    // Estimate a copula only if dimension>1
    if (dimension > 1)
      return ComposedDistribution(coll, BernsteinCopulaFactory().build(residual_));
    return coll[0];
  }
  return KernelSmoothing().build(residual_);
}

/* Draw model vs metamodel validation graph */
GridLayout MetaModelValidation::drawValidation() const
{
  // Build the first drawable
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const Sample yhat = metaModel_(inputSample_);
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
    diagonal.setColor("red");
    graph.add(diagonal);

    // points
    Cloud cloud(outputSample_.getMarginal(j), yhat.getMarginal(j));
    cloud.setColor("blue");
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
