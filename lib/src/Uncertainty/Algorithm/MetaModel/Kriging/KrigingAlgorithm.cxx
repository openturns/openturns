//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "openturns/KrigingAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/KrigingEvaluation.hxx"
#include "openturns/KrigingGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/GeneralLinearModelResult.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingAlgorithm)

static const Factory<KrigingAlgorithm> Factory_KrigingAlgorithm;


/* Default constructor */
KrigingAlgorithm::KrigingAlgorithm()
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , outputSample_(0, 0)
  , normalize_(false)
  , covarianceModel_()
  , glmAlgo_()
  , gamma_(0)
  , rho_(0)
  , result_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  // Force the GLM algo to use the exact same linear algebra as the Kriging algorithm
  if (ResourceMap::GetAsString("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
  else glmAlgo_.setMethod(0);
}


/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const Sample & inputSample,
                                   const Sample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const Basis & basis,
                                   const Bool normalize)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , normalize_(normalize)
  , covarianceModel_()
  , glmAlgo_(inputSample, outputSample, covarianceModel, basis, normalize, true)
  , gamma_(0)
  , rho_(0)
  , result_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  // Force the GLM algo to use the exact same linear algebra as the Kriging algorithm
  if (ResourceMap::GetAsString("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
  else glmAlgo_.setMethod(0);
}


/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const Sample & inputSample,
                                   const Sample & outputSample,
                                   const CovarianceModel & covarianceModel,
                                   const BasisCollection & basisCollection,
                                   const Bool normalize)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , normalize_(normalize)
  , covarianceModel_(covarianceModel)
  , glmAlgo_(inputSample, outputSample, covarianceModel, basisCollection, normalize, true)
  , gamma_(0)
  , rho_(0)
  , result_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  // Force the GLM algo to use the exact same linear algebra as the Kriging algorithm
  if (ResourceMap::GetAsString("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
  else glmAlgo_.setMethod(0);
}

/* Virtual constructor */
KrigingAlgorithm * KrigingAlgorithm::clone() const
{
  return new KrigingAlgorithm(*this);
}

void KrigingAlgorithm::computeGamma()
{
  // Get cholesky factor & rho from glm
  LOGINFO("Solve L^t.gamma = rho");
  if (ResourceMap::GetAsString("KrigingAlgorithm-LinearAlgebra") == "HMAT")
  {
    gamma_ = covarianceCholeskyFactorHMatrix_.solveLower(rho_, true);
  }
  else
  {
    // Arguments are keepIntact=true, matrix_lower=true & solving_transposed=true
    gamma_ = covarianceCholeskyFactor_.getImplementation()->solveLinearSystemTri(rho_, true, true, true);
  }
}

/* Perform regression */
void KrigingAlgorithm::run()
{
  LOGINFO("Launch GeneralLinearModelAlgorithm for the optimization");
  glmAlgo_.run();
  LOGINFO("End of GeneralLinearModelAlgorithm run");

  // Covariance coefficients are computed once, ever if optimiser is fixed
  rho_ = glmAlgo_.getRho();

  /* Method that returns the covariance factor - hmat */
  const GeneralLinearModelResult glmResult(glmAlgo_.getResult());
  if (ResourceMap::GetAsString("KrigingAlgorithm-LinearAlgebra") == "HMAT")
    covarianceCholeskyFactorHMatrix_ = glmResult.getHMatCholeskyFactor();
  else
    covarianceCholeskyFactor_ = glmResult.getCholeskyFactor();
  LOGINFO("Compute the interpolation part");
  computeGamma();
  LOGINFO("Store the estimates");
  LOGINFO("Build the output meta-model");
  Function metaModel;
  // We use directly the collection of points
  const BasisCollection basis(glmResult.getBasisCollection());
  const Sample normalizedInputSample(glmResult.getInputTransformedSample());
  const CovarianceModel conditionalCovarianceModel(glmResult.getCovarianceModel());
  const Collection<Point> trendCoefficients(glmResult.getTrendCoefficients());
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  Sample covarianceCoefficients(inputSample_.getSize(), outputDimension);
  covarianceCoefficients.getImplementation()->setData(gamma_);
  // Meta model definition
  metaModel.setEvaluation(new KrigingEvaluation(basis, normalizedInputSample, conditionalCovarianceModel, trendCoefficients, covarianceCoefficients));
  metaModel.setGradient(new KrigingGradient(basis, normalizedInputSample, conditionalCovarianceModel, trendCoefficients, covarianceCoefficients));
  metaModel.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), metaModel.getEvaluation()));
  // First build the meta-model on the transformed data
  // Then add the transformation if needed
  if (normalize_) metaModel = ComposedFunction(metaModel, glmResult.getTransformation());
  // compute residual, relative error
  const Point outputVariance(outputSample_.computeVariance());
  const Sample mY(metaModel(inputSample_));
  const Point squaredResiduals((outputSample_ - mY).computeRawMoment(2));

  const UnsignedInteger size = inputSample_.getSize();
  Point residuals(outputDimension);
  Point relativeErrors(outputDimension);
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    residuals[outputIndex] = sqrt(squaredResiduals[outputIndex] / size);
    relativeErrors[outputIndex] = squaredResiduals[outputIndex] / outputVariance[outputIndex];
  }
  result_ = KrigingResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis, trendCoefficients, conditionalCovarianceModel, covarianceCoefficients, covarianceCholeskyFactor_, covarianceCholeskyFactorHMatrix_);
  // If normalize, set input transformation
  if (normalize_)
  {
    const Function inputTransformation(glmResult.getTransformation());
    result_.setTransformation(inputTransformation);
  }
}


/* String converter */
String KrigingAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


Sample KrigingAlgorithm::getInputSample() const
{
  return inputSample_;
}


Sample KrigingAlgorithm::getOutputSample() const
{
  return outputSample_;
}


KrigingResult KrigingAlgorithm::getResult()
{
  return result_;
}

/* Optimization solver accessor */
OptimizationAlgorithm KrigingAlgorithm::getOptimizationAlgorithm() const
{
  return glmAlgo_.getOptimizationAlgorithm();
}

void KrigingAlgorithm::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  glmAlgo_.setOptimizationAlgorithm(solver);
}


/* Accessor to optimization bounds */
void KrigingAlgorithm::setOptimizationBounds(const Interval & optimizationBounds)
{
  glmAlgo_.setOptimizationBounds(optimizationBounds);
}

Interval KrigingAlgorithm::getOptimizationBounds() const
{
  return glmAlgo_.getOptimizationBounds();
}

/* Log-Likelihood function accessor */
Function KrigingAlgorithm::getReducedLogLikelihoodFunction()
{
  return glmAlgo_.getObjectiveFunction();
}

/* Optimize parameters flag accessor */
Bool KrigingAlgorithm::getOptimizeParameters() const
{
  return glmAlgo_.getOptimizeParameters();
}

void KrigingAlgorithm::setOptimizeParameters(const Bool optimizeParameters)
{
  glmAlgo_.setOptimizeParameters(optimizeParameters);
}

/* Observation noise accessor */
void KrigingAlgorithm::setNoise(const Point & noise)
{
  glmAlgo_.setNoise(noise);
}

Point KrigingAlgorithm::getNoise() const
{
  return glmAlgo_.getNoise();
}

String KrigingAlgorithm::getMethod() const
{
  const UnsignedInteger method = glmAlgo_.getMethod();
  if (method) return "HMAT";
  return "LAPACK";
}

void KrigingAlgorithm::setMethod(const String & method)
{
  if (method == "HMAT")
    glmAlgo_.setMethod(1);
  glmAlgo_.setMethod(0);
}

// Scale prior accessor
KrigingAlgorithm::ScalePrior KrigingAlgorithm::getScalePrior() const
{
  return glmAlgo_.getScalePrior();
}

void KrigingAlgorithm::setScalePrior(const ScalePrior scalePrior)
{
  glmAlgo_.setScalePrior(scalePrior);
}

/* Method save() stores the object through the StorageManager */
void KrigingAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "normalize_", normalize_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
}


/* Method load() reloads the object from the StorageManager */
void KrigingAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "normalize_", normalize_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
}

END_NAMESPACE_OPENTURNS
