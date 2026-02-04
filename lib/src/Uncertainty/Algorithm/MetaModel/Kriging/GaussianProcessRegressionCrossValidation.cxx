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
  if (gaussianProcessRegressionResult.getLinearAlgebraMethod() != 0)
    throw NotYetImplementedException(HERE) << "Error: HMAT is not supported yet.";
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
  const UnsignedInteger sampleDimension = outputSample.getDimension();
  if (sampleDimension != 1)
    throw InvalidArgumentException(HERE) << "Gaussian Process Regression cross-validation is only implemented for scalar output (current output dimension is "
                                         << sampleDimension << ").";


  // Implement second formula from Equation (32) in (Ginsbourger, 2023 preprint) which is equivalent to Dubrule (1983)
  // Let y be the vector of the output samples, yi its i-th element and ŷi the cross-validation prediction,
  // i.e. the GPR regression based on the sample of the yj (j != i)
  // yi - ŷi = (Sigmatilde y)[i] / Sigmatilde[i,i]
  // where Sigmatilde = Sigma^-1 - Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1.
  // Note that equivalently
  // Sigmatilde = L^-T (L^-1 - L^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1)

  // Compute unscaled residuals using the already computed rho : Sigmatilde y = L^-T rho
  const GaussianProcessFitterResult gpfResult(gaussianProcessRegressionResult.getGaussianProcessFitterResult());
  const Point rho(gpfResult.getRho());
  const TriangularMatrix covarianceCholeskyFactor(gpfResult.getCholeskyFactor()); // L
  const TriangularMatrix covarianceCholeskyFactorTranspose(covarianceCholeskyFactor.transpose()); // L^T
  Point residuals(covarianceCholeskyFactorTranspose.solveLinearSystem(rho)); // L^-T rho = (Sigma^-1  - Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1 ) y

  // Compute scales of residuals : Sigmatilde[i,i] for all i

  // Start by computing Sigma^-1[i,i] = ||L^-1[:,i]||^2 for all i
  // ||L^-1[:,i]||^2 can be computed as 1^T S[:,i],
  // where 1 is the column vector filled with ones,
  // and S is the matrix whose elements are the squares of the elements of L^-1.
  // Therefore the vector of the Sigma^-1[i,i] is 1^T S.
  const TriangularMatrix covarianceCholeskyFactorInverse(covarianceCholeskyFactor.inverse().getImplementation()); //L^-1
  TriangularMatrix covarianceCholeskyFactorInverseSquared(covarianceCholeskyFactorInverse);
  covarianceCholeskyFactorInverseSquared.squareElements();
  Point scales(covarianceCholeskyFactorInverseSquared.getImplementation()->genVectProd(Point(outputSample.getSize(), 1.0), true)); // diagonal elements of L^-T L^-1

  // Then, if the trend is estimated, the scales (which are homogeneous to precisions) must be diminished:
  // we subtract (Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1)[i,i]
  const UnsignedInteger  basisSize = gaussianProcessRegressionResult.getBasis().getSize();
  if (basisSize > 0)
  {
    const Matrix regressionMatrix(gaussianProcessRegressionResult.getRegressionMatrix()); // F
    const Matrix Phi(covarianceCholeskyFactor.solveLinearSystem(regressionMatrix)); // Phi := L^-1 F
    // Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1 = L^-T Phi (Phi^T Phi)^-1 Phi^T L^-1
    const Matrix Phitranspose(Phi.transpose());
    CovarianceMatrix PhiGram(Phi.computeGram()); // Phi^T Phi (not to be used, later modified in-place)
    TriangularMatrix phi(PhiGram.computeCholesky()); // Phi^T Phi =: phi phi^T (phi not to be used, later modified in-place)
    // Thus Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1 = L^-T Phi phi^-T phi^-1 Phi^T L^-1 = (phi^-1 Phi^T L^-1)^T (phi^-1 Phi^T L^-1)
    Matrix auxiliary(phi.solveLinearSystemInPlace(Phitranspose * covarianceCholeskyFactorInverse)); // auxiliary := phi^-1 Phi^T L^-1 has basisSize rows
    // (Sigma^-1 F (F^T Sigma^-1 F)^-1 F^T Sigma^-1)[i,i] = ||auxiliary[:,i]||^2
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
  MetaModelValidation::save(adv);
  adv.saveAttribute( "gaussianProcessRegressionResult_", gaussianProcessRegressionResult_ );
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcessRegressionCrossValidation::load(Advocate & adv)
{
  MetaModelValidation::load(adv);
  adv.loadAttribute( "gaussianProcessRegressionResult_", gaussianProcessRegressionResult_ );
}

END_NAMESPACE_OPENTURNS
