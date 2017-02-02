//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/KrigingAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ProductCovarianceModel.hxx"
#include "openturns/KrigingEvaluation.hxx"
#include "openturns/KrigingGradient.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/GeneralizedLinearModelResult.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingAlgorithm);

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
  , keepCholeskyFactor_(true)
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  // Nothing to do
}


/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const Basis & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool normalize,
                                   const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , normalize_(normalize)
  , covarianceModel_()
  , glmAlgo_()
  , gamma_(0)
  , rho_(0)
  , result_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  glmAlgo_ = GeneralizedLinearModelAlgorithm(inputSample, outputSample, covarianceModel, basis, normalize, keepCholeskyFactor);
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
}


/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const BasisCollection & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool normalize,
                                   const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , normalize_(normalize)
  , covarianceModel_(covarianceModel)
  , glmAlgo_()
  , gamma_(0)
  , rho_(0)
  , result_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  glmAlgo_ = GeneralizedLinearModelAlgorithm(inputSample, outputSample, covarianceModel, basis, normalize, keepCholeskyFactor);
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
}

/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalMathFunction & inputTransformation,
                                   const NumericalSample & outputSample,
                                   const Basis & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , normalize_(true)
  , covarianceModel_(covarianceModel)
  , gamma_(0)
  , rho_(0)
  , result_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  glmAlgo_ = GeneralizedLinearModelAlgorithm(inputSample, inputTransformation, outputSample, covarianceModel, basis, keepCholeskyFactor);
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
}

/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalMathFunction & inputTransformation,
                                   const NumericalSample & outputSample,
                                   const BasisCollection & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , normalize_(true)
  , covarianceModel_(covarianceModel)
  , glmAlgo_()
  , gamma_(0)
  , rho_(0)
  , result_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  glmAlgo_ = GeneralizedLinearModelAlgorithm(inputSample, inputTransformation, outputSample, covarianceModel, basis, keepCholeskyFactor);
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT") glmAlgo_.setMethod(1);
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
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT")
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
  LOGINFO("Launch GeneralizedLinearModelAlgorithm for the optimization");
  glmAlgo_.run();
  LOGINFO("End of GeneralizedLinearModelAlgorithm run");

  // Covariance coefficients are computed once, ever if optimiser is fixed
  rho_ = glmAlgo_.getRho();

  /** Method that returns the covariance factor - hmat */
  const GeneralizedLinearModelResult glmResult(glmAlgo_.getResult());
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT")
    covarianceCholeskyFactorHMatrix_ = glmResult.getHMatCholeskyFactor();
  else
    covarianceCholeskyFactor_ = glmResult.getCholeskyFactor();
  LOGINFO("Compute the interpolation part");
  computeGamma();
  LOGINFO("Store the estimates");
  LOGINFO("Build the output meta-model");
  NumericalMathFunction metaModel;
  // We use directly the collection of points
  const BasisCollection basis(glmResult.getBasisCollection());
  const NumericalSample normalizedInputSample(glmResult.getInputTransformedSample());
  const CovarianceModel conditionalCovarianceModel(glmResult.getCovarianceModel());
  const Collection<NumericalPoint> trendCoefficients(glmResult.getTrendCoefficients());
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  NumericalSample covarianceCoefficients(inputSample_.getSize(), outputDimension);
  covarianceCoefficients.getImplementation()->setData(gamma_);
  // Meta model definition
  metaModel.setEvaluation(new KrigingEvaluation(basis, normalizedInputSample, conditionalCovarianceModel, trendCoefficients, covarianceCoefficients));
  metaModel.setGradient(new KrigingGradient(basis, normalizedInputSample, conditionalCovarianceModel, trendCoefficients, covarianceCoefficients));
  metaModel.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), metaModel.getEvaluation()));
  // First build the meta-model on the transformed data
  // Then add the transformation if needed
  if (normalize_) metaModel = ComposedFunction(metaModel, glmResult.getTransformation());
  // compute residual, relative error
  const NumericalPoint outputVariance(outputSample_.computeVariance());
  const NumericalSample mY(metaModel(inputSample_));
  const NumericalPoint squaredResiduals((outputSample_ - mY).computeRawMoment(2));

  const UnsignedInteger size = inputSample_.getSize();
  NumericalPoint residuals(outputDimension);
  NumericalPoint relativeErrors(outputDimension);
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    residuals[outputIndex] = sqrt(squaredResiduals[outputIndex] / size);
    relativeErrors[outputIndex] = squaredResiduals[outputIndex] / outputVariance[outputIndex];
  }
  if (keepCholeskyFactor_)
    result_ = KrigingResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis, trendCoefficients, conditionalCovarianceModel, covarianceCoefficients, covarianceCholeskyFactor_, covarianceCholeskyFactorHMatrix_);
  else
    result_ = KrigingResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis, trendCoefficients, conditionalCovarianceModel, covarianceCoefficients);
  // If normalize, set input transformation
  if (normalize_)
  {
    const NumericalMathFunction inputTransformation(glmResult.getTransformation());
    result_.setTransformation(inputTransformation);
  }
}


/* String converter */
String KrigingAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


NumericalSample KrigingAlgorithm::getInputSample() const
{
  return inputSample_;
}


NumericalSample KrigingAlgorithm::getOutputSample() const
{
  return outputSample_;
}


KrigingResult KrigingAlgorithm::getResult()
{
  return result_;
}

/** Optimization solver accessor */
OptimizationSolver KrigingAlgorithm::getOptimizationSolver() const
{
  return glmAlgo_.getOptimizationSolver();
}

void KrigingAlgorithm::setOptimizationSolver(const OptimizationSolver & solver)
{
  glmAlgo_.setOptimizationSolver(solver);
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

/** Log-Likelihood function accessor */
NumericalMathFunction KrigingAlgorithm::getLogLikelihoodFunction()
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

/** Observation noise accessor */
void KrigingAlgorithm::setNoise(const NumericalPoint & noise)
{
  glmAlgo_.setNoise(noise);
}

NumericalPoint KrigingAlgorithm::getNoise() const
{
  return glmAlgo_.getNoise();
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
  adv.saveAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
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
  adv.loadAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
}

END_NAMESPACE_OPENTURNS
