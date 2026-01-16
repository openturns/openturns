//                                               -*- C++ -*-
/**
 *  @brief Cross-validation of a Gaussian Process Regression surrogate model
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
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/GaussianProcessRegressionCrossValidation.hxx"
#include "openturns/KFoldSplitter.hxx"
#include "openturns/MetaModelValidation.hxx"
#include "openturns/GaussianProcessFitterResult.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessRegressionCrossValidation)

static const Factory<GaussianProcessRegressionCrossValidation> Factory_GaussianProcessRegressionCrossValidation;

/* Default constructor */
GaussianProcessRegressionCrossValidation::GaussianProcessRegressionCrossValidation()
  : MetaModelValidation()
{
  // Nothing to do
}

/* Parameter constructor */
GaussianProcessRegressionCrossValidation::GaussianProcessRegressionCrossValidation(const GaussianProcessRegressionResult & gaussianProcessRegressionResult)
  : GaussianProcessRegressionCrossValidation(gaussianProcessRegressionResult, LeaveOneOutSplitter(gaussianProcessRegressionResult.getOutputSample().getSize()))
{
  // Nothing to do
}

/* LOO constructor */
GaussianProcessRegressionCrossValidation::GaussianProcessRegressionCrossValidation(const GaussianProcessRegressionResult & gaussianProcessRegressionResult,
    const LeaveOneOutSplitter & splitter)
  : MetaModelValidation(gaussianProcessRegressionResult.getOutputSample()
                        , ComputeMetamodelLeaveOneOutPredictions(gaussianProcessRegressionResult))
  , gaussianProcessRegressionResult_(gaussianProcessRegressionResult)
  , splitter_ (splitter)
{
  const UnsignedInteger sampleDimension = gaussianProcessRegressionResult.getOutputSample().getDimension();
  if (sampleDimension != 1)
    throw InvalidArgumentException(HERE) << "Gaussian Process Regression cross-validation is only implemented for scalar output (current output dimension is "
                                         << sampleDimension << ").";
  const UnsignedInteger sampleSize = gaussianProcessRegressionResult.getOutputSample().getSize();
  if ((splitter_.getN() != sampleSize))
    throw InvalidArgumentException(HERE) << "The parameter N in the splitter is " << splitter_.getN()
                                         << " but the sample size is " << sampleSize;
}

/* K-Fold constructor */
GaussianProcessRegressionCrossValidation::GaussianProcessRegressionCrossValidation(const GaussianProcessRegressionResult & gaussianProcessRegressionResult,
    const KFoldSplitter & splitter)
  : MetaModelValidation(gaussianProcessRegressionResult.getOutputSample()
                        , ComputeMetamodelKFoldPredictions(gaussianProcessRegressionResult, splitter))
  , gaussianProcessRegressionResult_(gaussianProcessRegressionResult)
  , splitter_ (splitter)
{
  throw NotYetImplementedException(HERE) << "Error: cannot perform KFold cross-validation of Gaussian Process Regression.";
}

/* Virtual constructor */
GaussianProcessRegressionCrossValidation * GaussianProcessRegressionCrossValidation::clone() const
{
  return new GaussianProcessRegressionCrossValidation(*this);
}

/* String converter */
String GaussianProcessRegressionCrossValidation::__repr__() const
{
  OSS oss;
  oss << "class=" << GaussianProcessRegressionCrossValidation::GetClassName()
      << " Gaussian process regression result=" << gaussianProcessRegressionResult_
      << " splitter_=" << splitter_;
  return oss;
}

/* Get result*/
GaussianProcessRegressionResult GaussianProcessRegressionCrossValidation::getGaussianProcessRegressionResult() const
{
  return gaussianProcessRegressionResult_;
}

/* Get the splitter */
SplitterImplementation GaussianProcessRegressionCrossValidation::getSplitter() const
{
  return splitter_;
}

/* Compute cross-validation Leave-One-Out metamodel predictions */
Sample GaussianProcessRegressionCrossValidation::ComputeMetamodelLeaveOneOutPredictions(
  const GaussianProcessRegressionResult & gaussianProcessRegressionResult)
  //const LeaveOneOutSplitter & splitter)
{
  const Sample outputSample(gaussianProcessRegressionResult.getOutputSample());

  // Implement second formula from Equation (32) in (Ginsbourger, 2023 preprint) which is equivalent to Dubrule (1983)

  // Compute unscaled residuals
  const GaussianProcessFitterResult gpfResult(gaussianProcessRegressionResult.getGaussianProcessFitterResult());
  const Point rho(gpfResult.getRho());
  const TriangularMatrix covarianceCholeskyFactor(gpfResult.getCholeskyFactor()); // L
  const TriangularMatrix covarianceCholeskyFactorTranspose(covarianceCholeskyFactor.transpose()); // L^T
  Point residuals(covarianceCholeskyFactorTranspose.solveLinearSystem(rho)); // L^-T rho = (Sigma^-1  - Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1 ) y

  // Compute scales of residuals
  const TriangularMatrix covarianceCholeskyFactorInverse(covarianceCholeskyFactor.inverse().getImplementation()); //L^-1
  TriangularMatrix covarianceCholeskyFactorInverseSquared(covarianceCholeskyFactorInverse);
  covarianceCholeskyFactorInverseSquared.squareElements();
  Point scales(covarianceCholeskyFactorInverseSquared.getImplementation()->genVectProd(Point(outputSample.getSize(), 1.0), true)); // diagonal elements of L^-T L^-1

  // If the trend is estimated, the scales (which are homogeneous to precisions) must be diminished
  const UnsignedInteger  basisSize = gaussianProcessRegressionResult.getBasis().getSize();
  if (basisSize > 0)
  {
    const Matrix regressionMatrix(gaussianProcessRegressionResult.getRegressionMatrix()); // F
    const Matrix Phi(covarianceCholeskyFactor.solveLinearSystem(regressionMatrix)); // Phi = L^-1 F
    const Matrix Phitranspose(Phi.transpose());
    CovarianceMatrix PhitransposePhi((Phitranspose * Phi).getImplementation()); // Phi^T Phi (not to be used, later modified in-place)
    TriangularMatrix phi(PhitransposePhi.computeCholesky()); // Phi^T Phi =: phi phi^T (phi not to be used, later modified in-place)
    Matrix auxiliary(phi.solveLinearSystemInPlace(Phitranspose * covarianceCholeskyFactorInverse)); // auxiliary := phi^-1 Phi^T L^-1 has basisSize rows
    // Subtract the diagonal elements of auxiliary^T auxiliary from the scales
    auxiliary.squareElements();
    scales -= auxiliary.getImplementation()->genVectProd(Point(basisSize, 1.0), true);
  }

  // Compute scaled residuals
  for (UnsignedInteger i = 0; i < outputSample.getSize(); ++i) residuals[i] /= scales[i];

  // LOO predictions computed from the LOO residuals
  return outputSample - Sample::BuildFromPoint(residuals);
}

/* Compute cross-validation K-Fold metamodel predictions */
Sample GaussianProcessRegressionCrossValidation::ComputeMetamodelKFoldPredictions(
  const GaussianProcessRegressionResult &,
  const KFoldSplitter &)
{
  throw NotYetImplementedException(HERE) << "Error: cannot perform KFold cross-validation of Gaussian Process Regression.";
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessRegressionCrossValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "gaussianProcessRegressionResult_", gaussianProcessRegressionResult_ );
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcessRegressionCrossValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "gaussianProcessRegressionResult_", gaussianProcessRegressionResult_ );
}

END_NAMESPACE_OPENTURNS
