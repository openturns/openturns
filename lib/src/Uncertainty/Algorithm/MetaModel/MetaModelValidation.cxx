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
      throw NotDefinedException(HERE) << "R2 cannot be computed on constant output component index " << j;
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

/* Compute cross-validation predictions */
Sample MetaModelValidation::ComputeMetamodelLeaveOneOutPredictions(
    const Sample & outputSample,
    const Sample & residual,
    const Point & hMatrixDiag,
    const LeaveOneOutSplitter & splitter)
{
  // The residual is ri = g(xi) - tilde{g}(xi) where g is the model
  // and tilde(g) is the metamodel.
  // Hence the metamodel prediction is tilde{g}(xi) = yi - ri.
  const UnsignedInteger sampleSize = outputSample.getSize();
  const UnsignedInteger outputDimension = outputSample.getDimension();
  if (residual.getSize() != sampleSize)
    throw InvalidArgumentException(HERE)
      << "Error: the residual sample size is: "
      << residual.getSize() <<
      " but the output sample size is: " << sampleSize;
  if (residual.getDimension() != outputSample.getDimension())
    throw InvalidArgumentException(HERE)
      << "Error: the output sample dimension is: "
      << outputSample.getDimension() 
      << " which is different from the residual output dimension: "
      << residual.getDimension();
  if (splitter.getN() != sampleSize)
    throw InvalidArgumentException(HERE)
      << "Error: the splitter size is: " << splitter.getSize() << 
      " but the output sample size is " << sampleSize;
  if (hMatrixDiag.getDimension() != sampleSize)
    throw InvalidArgumentException(HERE)
      << "Error: the H matrix diagional dimension is: " << hMatrixDiag.getDimension() << 
      " but the output sample size is " << sampleSize;
  Sample cvPredictions(sampleSize, outputDimension);
  for (UnsignedInteger j = 0; j < outputDimension; ++j)
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      if (hMatrixDiag[i] == 1.0)
        throw InvalidArgumentException(HERE) 
          << "The leverage of observation #" << i
          << " is equal to 1. Cannot divide by zero.";
      cvPredictions(i, j) = outputSample(i, j) - residual(i, j) / (1.0 - hMatrixDiag[i]);
    } // For observations indices
  return cvPredictions;
}

/* Compute cross-validation predictions */
Sample MetaModelValidation::ComputeMetamodelKFoldPredictions(
    const Sample & outputSample,
    const Sample & residual,
    const SymmetricMatrix & projectionMatrix, 
    const KFoldSplitter & splitter)
{
  // The residual is ri = g(xi) - tilde{g}(xi) where g is the model
  // and tilde(g) is the metamodel.
  // Hence the metamodel prediction is tilde{g}(xi) = yi - ri.
  const UnsignedInteger sampleSize = outputSample.getSize();
  const UnsignedInteger outputDimension = outputSample.getDimension();
  if (residual.getSize() != sampleSize)
    throw InvalidArgumentException(HERE)
      << "Error: the residual sample size is: "
      << residual.getDimension() << 
      " but the output sample size is: " << sampleSize;
  if (residual.getDimension() != outputSample.getDimension())
    throw InvalidArgumentException(HERE)
      << "Error: the output sample dimension is: "
      << outputSample.getDimension() 
      << " which is different from the residual output dimension: "
      << residual.getDimension();
  if (splitter.getN() != sampleSize)
    throw InvalidArgumentException(HERE)
      << "Error: the splitter size is: " << splitter.getSize() << 
      " but the output sample size is " << sampleSize;
  if (projectionMatrix.getDimension() != sampleSize)
    throw InvalidArgumentException(HERE)
      << "Error: the H matrix diagional dimension is: " << projectionMatrix.getDimension() << 
      " but the output sample size is " << sampleSize;
  Sample cvPredictions(sampleSize, outputDimension);
  UnsignedInteger kParameter = splitter.getSize();
  Indices indicesTest;
  for (UnsignedInteger foldIndex = 0; foldIndex < kParameter; ++foldIndex)
  {
    splitter.generate(indicesTest);
    const UnsignedInteger foldSize = indicesTest.getSize();
    SymmetricMatrix projectionKFoldMatrix(foldSize);
    for (UnsignedInteger i1 = 0; i1 < foldSize; ++i1)
      for (UnsignedInteger i2 = 0; i2 < 1 + i1; ++i2)
        projectionKFoldMatrix(i1, i2) = projectionMatrix(indicesTest[i1], indicesTest[i2]);
    const IdentityMatrix identityMatrix(foldSize);
    const SymmetricMatrix reducedMatrix(identityMatrix - projectionKFoldMatrix);
    const Sample residualsSampleKFoldTest(residual.select(indicesTest));
    Matrix multipleRightHandSide(foldSize, outputDimension);
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger i = 0; i < foldSize; ++i)
        multipleRightHandSide(i, j) = residualsSampleKFoldTest(i, j);
    const Matrix residualsKFoldMatrix(reducedMatrix.solveLinearSystem(multipleRightHandSide));
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger i = 0; i < foldSize; ++i)
        cvPredictions(indicesTest[i], j) = outputSample(indicesTest[i], j) - residualsKFoldMatrix(i, j);
  } // For fold indices
  return cvPredictions;
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
  adv.loadAttribute( "residual_", residual_ );
  if (adv.hasAttribute("metaModelPredictions_") &&
      adv.hasAttribute("meanSquaredError_") &&
      adv.hasAttribute("r2Score_"))
  {
    adv.loadAttribute( "metaModelPredictions_", metaModelPredictions_ );
    adv.loadAttribute( "meanSquaredError_", meanSquaredError_ );
    adv.loadAttribute( "r2Score_", r2Score_ );
  }
  else  // Old version of the object
  {
    // Recompute everything as meanSquaredError_ has to be recomputed in any case
    Sample inputSample;
    Function metaModel;
    adv.loadAttribute( "inputSample_", inputSample );
    adv.loadAttribute( "metaModel_", metaModel );
    *this = MetaModelValidation(outputSample_, metaModel(inputSample));
  }
}

END_NAMESPACE_OPENTURNS
