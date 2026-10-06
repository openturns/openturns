//                                               -*- C++ -*-
/**
 *  @brief The class fits sparse gaussian process models
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

#include "openturns/SparseGaussianProcessFitter.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/MemoizeFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SparseGaussianProcessEvaluation.hxx"
#include "openturns/SparseGaussianProcessGradient.hxx"
#include "openturns/SparseGaussianProcessHessian.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/HMatrixParameters.hxx"
#include "openturns/HMatrixImplementation.hxx"
#include "openturns/CholAdjoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseGaussianProcessFitter)

static const Factory<SparseGaussianProcessFitter> Factory_SparseGaussianProcessFitter;

/* Default constructor */
SparseGaussianProcessFitter::SparseGaussianProcessFitter()
  : MetaModelAlgorithm()
  , noiseStdDev_(ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDev"))
{
  if (!(noiseStdDev_ > 0.0))
    throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultNoiseStdDev must be positive, got " << noiseStdDev_;
  initializeMethod();
}

/* Parameters constructor */
SparseGaussianProcessFitter::SparseGaussianProcessFitter(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Sample & inducingPoints)
  : MetaModelAlgorithm(inputSample, outputSample)
  , noiseStdDev_(ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDev"))
{
  if (!(noiseStdDev_ > 0.0))
    throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultNoiseStdDev must be positive, got " << noiseStdDev_;
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::SparseGaussianProcessFitter, the input sample size (" << inputSample.getSize() << ") should be equal to the output sample size (" << outputSample.getSize() << ")";
  setCovarianceModel(covarianceModel);
  setInducingPoints(inducingPoints);
  initializeDefaultOptimizationAlgorithm();
  buildOptimizationBounds();
  initializeMethod();
}

/* Parameters constructor with an initial number of inducing points */
SparseGaussianProcessFitter::SparseGaussianProcessFitter(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const UnsignedInteger numberOfInducingPoints)
  : MetaModelAlgorithm(inputSample, outputSample)
  , noiseStdDev_(ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDev"))
{
  if (!(noiseStdDev_ > 0.0))
    throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultNoiseStdDev must be positive, got " << noiseStdDev_;
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::SparseGaussianProcessFitter, the input sample size (" << inputSample.getSize() << ") should be equal to the output sample size (" << outputSample.getSize() << ")";
  const UnsignedInteger size = inputSample.getSize();
  if (numberOfInducingPoints > size)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::SparseGaussianProcessFitter, the number of inducing points (" << numberOfInducingPoints << ") should not exceed the number of observations (" << size << ")";
  if (numberOfInducingPoints == 0)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::SparseGaussianProcessFitter, the number of inducing points should be positive";
  // Deterministic subsampling of the input sample
  Sample inducingPoints(numberOfInducingPoints, inputSample.getDimension());
  for (UnsignedInteger i = 0; i < numberOfInducingPoints; ++i)
  {
    const UnsignedInteger index = static_cast<UnsignedInteger>(std::floor((1.0 * i * size) / numberOfInducingPoints));
    inducingPoints[i] = inputSample[index];
  }
  setCovarianceModel(covarianceModel);
  setInducingPoints(inducingPoints);
  initializeDefaultOptimizationAlgorithm();
  buildOptimizationBounds();
  initializeMethod();
}

/* Virtual constructor */
SparseGaussianProcessFitter * SparseGaussianProcessFitter::clone() const
{
  return new SparseGaussianProcessFitter(*this);
}

/* String converter */
String SparseGaussianProcessFitter::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << ", inputSample=" << inputSample_
      << ", outputSample=" << outputSample_
      << ", covarianceModel=" << covarianceModel_
      << ", reducedCovarianceModel=" << reducedCovarianceModel_
      << ", inducingPoints=" << inducingPoints_
      << ", noiseStdDev=" << noiseStdDev_
      << ", noiseVariancesSize=" << noiseVariances_.getSize()
      << ", solver=" << solver_
      << ", optimizeParameters=" << optimizeParameters_
      << ", optimizeInducingPoints=" << optimizeInducingPoints_
      << ", optimizeNoiseStdDev=" << optimizeNoiseStdDev_;
  return oss;
}

/* Perform regression */
void SparseGaussianProcessFitter::run()
{
  // Do not run again if already computed
  if (hasRun_) return;
  // optimization of the ELBO if there is at least one parameter to optimize
  Scalar optimalELBO = maximizeELBO();

  LOGDEBUG("Store the estimates");
  LOGDEBUG("Build the output meta-model");
  // return optimized covmodel with the original active parameters
  CovarianceModel reducedCovarianceModelCopy(reducedCovarianceModel_);
  reducedCovarianceModelCopy.setActiveParameter(covarianceModel_.getActiveParameter());

  SparseGaussianProcessEvaluation evaluation(reducedCovarianceModelCopy, inducingPoints_, whiteningFactor_, posteriorMean_, posteriorCovariance_, whiteningFactorHMatrix_, method_);
  Function metaModel(evaluation);
  metaModel.setInputDescription(inputSample_.getDescription());
  metaModel.setOutputDescription(outputSample_.getDescription());
  metaModel.setGradient(new SparseGaussianProcessGradient(reducedCovarianceModelCopy, inducingPoints_, whiteningFactor_, posteriorMean_, whiteningFactorHMatrix_, method_));
  metaModel.setHessian(new SparseGaussianProcessHessian(reducedCovarianceModelCopy, inducingPoints_, whiteningFactor_, posteriorMean_, whiteningFactorHMatrix_, method_));

  result_ = SparseGaussianProcessFitterResult(inputSample_, outputSample_, reducedCovarianceModelCopy, inducingPoints_, whiteningFactor_, posteriorMean_, posteriorCovariance_, noiseStdDev_, optimalELBO, metaModel, method_);
  result_.setNoiseVariances(noiseVariances_);
  result_.setWhiteningFactorHMatrix(whiteningFactorHMatrix_);
  hasRun_ = true;
}

/* Result accessor */
SparseGaussianProcessFitterResult SparseGaussianProcessFitter::getResult()
{
  if (!hasRun_) run();
  return result_;
}

/* Objective function accessor */
Function SparseGaussianProcessFitter::getObjectiveFunction()
{
  MemoizeFunction objective(ELBOEvaluation(*this));
  // The analytic gradient is only implemented for the LAPACK backend.
  // With HMAT, rely on the default finite-difference gradient.
  if (method_ == SparseGaussianProcessFitterResult::LAPACK)
    objective.setGradient(ELBOGradient(*this).clone());
  objective.enableCache();
  return objective;
}

/* Optimization solver accessor */
OptimizationAlgorithm SparseGaussianProcessFitter::getOptimizationAlgorithm() const
{
  return solver_;
}

void SparseGaussianProcessFitter::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
  reset();
}

/* Optimize parameters flag accessor */
Bool SparseGaussianProcessFitter::getOptimizeParameters() const
{
  return optimizeParameters_;
}

void SparseGaussianProcessFitter::setOptimizeParameters(const Bool optimizeParameters)
{
  if (optimizeParameters != optimizeParameters_)
  {
    optimizeParameters_ = optimizeParameters;
    // Here we have to call setCovarianceModel() as it computes reducedCovarianceModel from covarianceModel_ in a way influenced by optimizeParameters_ flag.
    setCovarianceModel(covarianceModel_);
  }
}

/* Optimize inducing points flag accessor */
Bool SparseGaussianProcessFitter::getOptimizeInducingPoints() const
{
  return optimizeInducingPoints_;
}

void SparseGaussianProcessFitter::setOptimizeInducingPoints(const Bool optimizeInducingPoints)
{
  if (optimizeInducingPoints != optimizeInducingPoints_)
  {
    optimizeInducingPoints_ = optimizeInducingPoints;
    reset();
    buildOptimizationBounds();
  }
}

/* Optimize noise variance flag accessor */
Bool SparseGaussianProcessFitter::getOptimizeNoiseStdDev() const
{
  return optimizeNoiseStdDev_;
}

void SparseGaussianProcessFitter::setOptimizeNoiseStdDev(const Bool optimizeNoiseStdDev)
{
  if (optimizeNoiseStdDev && hasNoiseVariances())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setOptimizeNoiseStdDev, the noise standard deviation cannot be optimized when per-observation noise variances are set, as they are fixed";
  if (optimizeNoiseStdDev != optimizeNoiseStdDev_)
  {
    optimizeNoiseStdDev_ = optimizeNoiseStdDev;
    reset();
    buildOptimizationBounds();
  }
}

/* Noise standard deviation accessor */
Scalar SparseGaussianProcessFitter::getNoiseStdDev() const
{
  return noiseStdDev_;
}

void SparseGaussianProcessFitter::setNoiseStdDev(const Scalar noiseStdDev)
{
  if (!(noiseStdDev > 0.0))
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setNoiseStdDev, the noise standard deviation should be positive, got " << noiseStdDev;
  if (noiseStdDev != noiseStdDev_)
  {
    noiseStdDev_ = noiseStdDev;
    reset();
  }
}

/* Per-observation noise variances accessor */
Point SparseGaussianProcessFitter::getNoiseVariances() const
{
  return noiseVariances_;
}

void SparseGaussianProcessFitter::setNoiseVariances(const Point & noiseVariances)
{
  const UnsignedInteger size = inputSample_.getSize();
  if (noiseVariances.getSize() != 0)
  {
    if (noiseVariances.getSize() != size)
      throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setNoiseVariances, the number of noise variances (" << noiseVariances.getSize() << ") should match the number of observations (" << size << "), or be zero to use the homoscedastic likelihood";
    for (UnsignedInteger i = 0; i < size; ++i)
      if (!(noiseVariances[i] > 0.0))
        throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setNoiseVariances, the noise variances should be positive, got " << noiseVariances[i] << " at index " << i;
  }
  if (noiseVariances != noiseVariances_)
  {
    noiseVariances_ = noiseVariances;
    // Fixed variances are not optimized: the scalar noise parameter leaves
    // the optimization problem while the variances are set
    if (hasNoiseVariances()) optimizeNoiseStdDev_ = false;
    reset();
    buildOptimizationBounds();
  }
}

/* Whether fixed per-observation noise variances are set */
Bool SparseGaussianProcessFitter::hasNoiseVariances() const
{
  return noiseVariances_.getSize() != 0;
}

/* Inducing points accessor */
Sample SparseGaussianProcessFitter::getInducingPoints() const
{
  return inducingPoints_;
}

void SparseGaussianProcessFitter::setInducingPoints(const Sample & inducingPoints)
{
  if (inducingPoints.getDimension() != inputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setInducingPoints, the inducing points dimension (" << inducingPoints.getDimension() << ") should match the input sample dimension (" << inputSample_.getDimension() << ")";
  const UnsignedInteger size = inputSample_.getSize();
  if (inducingPoints.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setInducingPoints, the number of inducing points should be positive";
  if (inducingPoints.getSize() > size)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setInducingPoints, the number of inducing points (" << inducingPoints.getSize() << ") should not exceed the number of observations (" << size << ")";
  inducingPoints_ = inducingPoints;
  reset();
  buildOptimizationBounds();
}

/* Covariance model accessor */
CovarianceModel SparseGaussianProcessFitter::getCovarianceModel() const
{
  return covarianceModel_;
}

CovarianceModel SparseGaussianProcessFitter::getReducedCovarianceModel() const
{
  return reducedCovarianceModel_;
}

/* Linear algebra method accessor */
SparseGaussianProcessFitter::LinearAlgebra SparseGaussianProcessFitter::getMethod() const
{
  return method_;
}

/* Linear algebra method setter */
void SparseGaussianProcessFitter::setMethod(const LinearAlgebra method)
{
  if (method != method_)
  {
    if (method != SparseGaussianProcessFitterResult::LAPACK &&
        method != SparseGaussianProcessFitterResult::HMAT)
      throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter::setMethod, expecting LAPACK or HMAT, got " << static_cast<UnsignedInteger>(method);
    method_ = method;
    reset();
  }
}

void SparseGaussianProcessFitter::initializeMethod()
{
  if (ResourceMap::GetAsString("SparseGaussianProcessFitter-LinearAlgebra") == "HMAT")
    setMethod(SparseGaussianProcessFitterResult::HMAT);
}

void SparseGaussianProcessFitter::setCovarianceModel(const CovarianceModel & covarianceModel)
{
  // Here we can store any modified version of the given covariance model wrt its parameters as it is mainly a parametric template
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  const UnsignedInteger outputDimension = outputSample_.getDimension();

  if (covarianceModel.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Covariance model input dimension is " << covarianceModel.getInputDimension() << ", expected " << inputDimension;
  if (covarianceModel.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Covariance model output dimension is " << covarianceModel.getOutputDimension() << ", expected " << outputDimension;
  if (outputDimension != 1)
    throw NotYetImplementedException(HERE) << "In SparseGaussianProcessFitter::setCovarianceModel, sparse Gaussian processes only support scalar outputs for now";
  covarianceModel_ = covarianceModel;
  // All the computation will be done on the reduced covariance model. We keep the initial covariance model (ie the one we just built) in order to reinitialize the reduced covariance model if some flags are changed after the creation of the algorithm.
  reducedCovarianceModel_ = covarianceModel_;
  // Now, adapt the model parameters.
  // First, check if the parameters have to be optimized. If not, remove all the active parameters.
  if (!optimizeParameters_) reducedCovarianceModel_.setActiveParameter(Indices());
  reset();
  buildOptimizationBounds();
}

// Maximize the ELBO of the sparse Gaussian process model wrt the observations
// If there is no parameter to optimize, no numerical optimization is done:
// a single call to computeELBO() is made in order to compute the by-products
// (whitening factor, variational posterior mean and covariance).
// The method returns the optimal ELBO (which is equal to the initial one if no
// parameter has to be optimized), the corresponding parameters being directly
// stored into the members.
Scalar SparseGaussianProcessFitter::maximizeELBO()
{
  // initial guess
  Point initialParameters(buildOptimizationParameters());
  // We use the functional form of the ELBO computation to benefit from the cache mechanism
  Function objectiveFunction(getObjectiveFunction());
  const Bool noNumericalOptimization = initialParameters.getSize() == 0;
  // Early exit if the parameters are known
  if (noNumericalOptimization)
  {
    // Call computeELBO() directly on *this to get the by-products (whitening
    // factor, posterior mean/covariance).  The function wrapper is bypassed
    // because the cache provides no benefit for a single evaluation, and the
    // direct call makes the side-effect intent explicit (defense in depth).
    const Scalar initialELBO = computeELBO(initialParameters)[0];
    LOGDEBUG("No parameter to optimize");
    LOGDEBUG(OSS() << "initial parameters=" << initialParameters << ", ELBO=" << initialELBO);
    return initialELBO;
  }
  // Thus here we perform an optimization. First let us check the initial point is inside the
  // optimization bounds search, otherwise define one arbitrary inside these bounds
  // NOTE: the optimization is local and can converge to a degenerate optimum (amplitude at its
  // lower bound, large noise variance) when the inducing points are too few or clustered, see
  // the documentation of the class.
  if (!optimizationBounds_.contains(initialParameters))
  {
    // Define starting point for the optimization as the center of the bounds
    // We should ensure somehow that the upper/lower bounds scale are nearly the same
    initialParameters = (optimizationBounds_.getUpperBound() + optimizationBounds_.getLowerBound()) / 2;
  }

  // internal normalization into (0,1)^n
  Interval bounds(optimizationBounds_);
  const Bool normalization = ResourceMap::GetAsBool("SparseGaussianProcessFitter-OptimizationNormalization");
  const UnsignedInteger parameterDimension = initialParameters.getDimension();
  Function uToX;
  if (normalization)
  {
    Matrix linear(parameterDimension, parameterDimension);
    for (UnsignedInteger i = 0; i < parameterDimension; ++ i)
    {
      linear(i, i) = (optimizationBounds_.getUpperBound()[i] - optimizationBounds_.getLowerBound()[i]);
      initialParameters[i] = (initialParameters[i] - optimizationBounds_.getLowerBound()[i]) / linear(i, i);
    }
    uToX = LinearFunction(Point(parameterDimension), optimizationBounds_.getLowerBound(), linear);
    objectiveFunction = ComposedFunction(objectiveFunction, uToX);
    bounds = Interval(parameterDimension);
  }

  // At this point we have an optimization problem to solve
  // Define the optimization problem
  OptimizationProblem problem(objectiveFunction);
  problem.setMinimization(false);
  problem.setBounds(bounds);
  solver_.setProblem(problem);
  try
  {
    // If the solver is single start, we can use its setStartingPoint method
    solver_.setStartingPoint(initialParameters);
  }
  catch (const NotDefinedException &) // setStartingPoint is not defined for the solver
  {
    // Define starting point for the optimization as the center of the bounds if necessary
    Sample initialPoints(solver_.getStartingSample());
    const Point center(0.5 * (optimizationBounds_.getUpperBound() + optimizationBounds_.getLowerBound()));
    for (UnsignedInteger i = 0; i < initialPoints.getSize(); ++ i)
    {
      if (!optimizationBounds_.contains(initialPoints[i]))
        initialPoints[i] = center;
    }
    solver_.setStartingSample(initialPoints);

    if (normalization)
    {
      Point linear(parameterDimension);
      for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
        linear[j] = (optimizationBounds_.getUpperBound()[j] - optimizationBounds_.getLowerBound()[j]);
      for (UnsignedInteger i = 0; i < initialPoints.getSize(); ++ i)
        for (UnsignedInteger j = 0; j < parameterDimension; ++ j)
          initialPoints(i, j) = (initialPoints(i, j) - optimizationBounds_.getLowerBound()[j]) / linear[j];
      solver_.setStartingSample(initialPoints);
    }
  }
  LOGDEBUG(OSS(false) << "Solve problem=" << problem << " using solver=" << solver_);
  solver_.run();
  const OptimizationAlgorithm::Result result(solver_.getResult());
  const Point optimalELBOPoint = result.getOptimalValue();
  if (!optimalELBOPoint.getSize())
    throw InvalidArgumentException(HERE) << "optimization in SparseGaussianProcessFitter did not yield feasible points";
  Scalar optimalELBO = optimalELBOPoint[0];
  Point optimalParameters(result.getOptimalPoint());
  if (normalization)
    optimalParameters = uToX(optimalParameters);

  const UnsignedInteger evaluationNumber = result.getCallsNumber();
  // Recompute the ELBO at the optimal parameters, so that the returned value is
  // consistent with the by-products (whitening factor, posterior mean/covariance)
  // stored at the same point. No additional cost when the cache is enabled.
  LOGDEBUG(OSS(false) << "Need to evaluate the objective function one more time because the last computed ELBO value=" << lastELBO_ << " is different from the optimal one=" << optimalELBO);
  (void) computeELBO(optimalParameters);
  optimalELBO = lastELBO_;
  // Final call to objectiveFunction() in order to update the by-products
  // No additional cost since the cache mechanism is activated
  LOGDEBUG(OSS() << evaluationNumber << " evaluations, optimized parameters=" << optimalParameters << ", ELBO=" << optimalELBO);

  return optimalELBO;
}

/* Unpack the optimization parameter vector */
SparseGaussianProcessFitter::UnpackedParameters SparseGaussianProcessFitter::unpackParameters(const Point & parameters) const
{
  UnsignedInteger offset = 0;
  const UnsignedInteger covarianceParameterSize = reducedCovarianceModel_.getParameter().getSize();
  const UnsignedInteger expectedSize = covarianceParameterSize + (optimizeNoiseStdDev_ ? 1 : 0) + (optimizeInducingPoints_ ? inducingPoints_.getSize() * inducingPoints_.getDimension() : 0);
  if (parameters.getSize() != expectedSize)
    throw InvalidArgumentException(HERE) << "In SparseGaussianProcessFitter, the parameter vector should be of size " << expectedSize
                                         << " but here we got " << parameters.getSize();
  UnpackedParameters unpacked;
  unpacked.covarianceParameters = Point(covarianceParameterSize);
  for (UnsignedInteger i = 0; i < covarianceParameterSize; ++i)
    unpacked.covarianceParameters[i] = parameters[offset + i];
  offset += covarianceParameterSize;
  unpacked.noiseStdDev = noiseStdDev_;
  if (optimizeNoiseStdDev_)
  {
    unpacked.noiseStdDev = std::exp(parameters[offset]);
    offset += 1;
  }
  unpacked.inducingPoints = Sample(inducingPoints_);
  if (optimizeInducingPoints_)
  {
    const UnsignedInteger M = inducingPoints_.getSize();
    const UnsignedInteger dimension = inducingPoints_.getDimension();
    for (UnsignedInteger i = 0; i < M; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
        unpacked.inducingPoints[i][j] = parameters[offset + i * dimension + j];
    offset += M * dimension;
  }
  return unpacked;
}

Point SparseGaussianProcessFitter::computeELBO(const Point & parameters)
{
  LOGDEBUG(OSS(false) << "Compute ELBO for parameters=" << parameters);
  const UnpackedParameters unpacked = unpackParameters(parameters);
  reducedCovarianceModel_.setParameter(unpacked.covarianceParameters);
  // Store the unpacked values into the members so that they are consistent
  // with the last computed ELBO (see also run())
  if (optimizeNoiseStdDev_) noiseStdDev_ = unpacked.noiseStdDev;
  if (optimizeInducingPoints_) inducingPoints_ = unpacked.inducingPoints;
  lastELBO_ = computeELBOValue(unpacked.inducingPoints, unpacked.noiseStdDev);
  return Point(1, lastELBO_);
}

/* Project the cross- and self-covariance adjoints onto the active covariance
 * parameters and the inducing points */
void SparseGaussianProcessFitter::accumulateGradient(const Matrix & KfuBar,
    const Matrix & KuuBar,
    const Point & tightCoefficients,
    const Sample & inducingPoints,
    Point & covarianceGradient,
    Point & inducingPointsGradient) const
{
  const UnsignedInteger N = KfuBar.getNbRows();
  const UnsignedInteger M = KfuBar.getNbColumns();
  const UnsignedInteger dimension = inducingPoints.getDimension();
  const UnsignedInteger covarianceParameterSize = reducedCovarianceModel_.getParameter().getSize();
  // Gradient wrt the active covariance parameters
  covarianceGradient = Point(covarianceParameterSize, 0.0);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    for (UnsignedInteger j = 0; j < M; ++j)
    {
      const Scalar coef = KfuBar(i, j);
      if (coef != 0.0)
      {
        const Matrix dk(reducedCovarianceModel_.parameterGradient(inputSample_[i], inducingPoints[j]));
        for (UnsignedInteger k = 0; k < covarianceParameterSize; ++k)
          covarianceGradient[k] += coef * dk(k, 0);
      }
    }
  }
  for (UnsignedInteger p = 0; p < M; ++p)
  {
    for (UnsignedInteger q = 0; q < M; ++q)
    {
      const Scalar coef = KuuBar(p, q);
      if (coef != 0.0)
      {
        const Matrix dk(reducedCovarianceModel_.parameterGradient(inducingPoints[p], inducingPoints[q]));
        for (UnsignedInteger k = 0; k < covarianceParameterSize; ++k)
          covarianceGradient[k] += coef * dk(k, 0);
      }
    }
  }
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    const Scalar coefficient = tightCoefficients[i];
    if (coefficient != 0.0)
    {
      const Matrix dk(reducedCovarianceModel_.parameterGradient(inputSample_[i], inputSample_[i]));
      for (UnsignedInteger k = 0; k < covarianceParameterSize; ++k)
        covarianceGradient[k] += coefficient * dk(k, 0);
    }
  }

  // Gradient wrt the inducing points, dk(s, t) / ds
  inducingPointsGradient = Point(M * dimension, 0.0);
  if (optimizeInducingPoints_)
  {
    for (UnsignedInteger j = 0; j < M; ++j)
    {
      Point gradD(dimension, 0.0);
      for (UnsignedInteger i = 0; i < N; ++i)
      {
        const Scalar coef = KfuBar(i, j);
        if (coef != 0.0)
        {
          const Matrix pg(reducedCovarianceModel_.partialGradient(inducingPoints[j], inputSample_[i]));
          for (UnsignedInteger d = 0; d < dimension; ++d)
            gradD[d] += coef * pg(d, 0);
        }
      }
      for (UnsignedInteger q = 0; q < M; ++q)
      {
        if (q == j) continue;
        const Scalar coef = KuuBar(j, q);
        if (coef != 0.0)
        {
          const Matrix pg(reducedCovarianceModel_.partialGradient(inducingPoints[j], inducingPoints[q]));
          for (UnsignedInteger d = 0; d < dimension; ++d)
            gradD[d] += coef * pg(d, 0);
        }
      }
      for (UnsignedInteger p = 0; p < M; ++p)
      {
        if (p == j) continue;
        const Scalar coef = KuuBar(p, j);
        if (coef != 0.0)
        {
          const Matrix pg(reducedCovarianceModel_.partialGradient(inducingPoints[j], inducingPoints[p]));
          for (UnsignedInteger d = 0; d < dimension; ++d)
            gradD[d] += coef * pg(d, 0);
        }
      }
      // Diagonal contribution: d/dz_j Kuu(j,j) = 2 * partialGradient(z_j, z_j).
      // For stationary models partialGradient at coincident points is zero, so
      // this term only contributes for nonstationary covariance models.
      {
        const Scalar coef = KuuBar(j, j);
        if (coef != 0.0)
        {
          const Matrix pg(reducedCovarianceModel_.partialGradient(inducingPoints[j], inducingPoints[j]));
          for (UnsignedInteger d = 0; d < dimension; ++d)
            gradD[d] += 2.0 * coef * pg(d, 0);
        }
      }
      for (UnsignedInteger d = 0; d < dimension; ++d)
        inducingPointsGradient[j * dimension + d] = gradD[d];
    }
  }
}

/* Compute the gradient of the collapsed ELBO wrt the optimization parameters */
Point SparseGaussianProcessFitter::computeELBOGradient(const Point & parameters)
{
  if (method_ == SparseGaussianProcessFitterResult::HMAT)
    throw NotYetImplementedException(HERE) << "In SparseGaussianProcessFitter::computeELBOGradient, the analytic ELBO gradient is LAPACK-only for now, setMethod(LAPACK) to use it";
  if (hasNoiseVariances())
    return computeHeteroscedasticELBOGradient(parameters);
  const UnpackedParameters unpacked = unpackParameters(parameters);
  reducedCovarianceModel_.setParameter(unpacked.covarianceParameters);
  // Save member state that the gradient computation temporarily modifies,
  // so that the optimizer can evaluate the objective and gradient at
  // different parameter points without leaving the fitter in an
  // inconsistent state.
  const Scalar savedNoiseStdDev = noiseStdDev_;
  const Sample savedInducingPoints(inducingPoints_);
  if (optimizeNoiseStdDev_) noiseStdDev_ = unpacked.noiseStdDev;
  if (optimizeInducingPoints_) inducingPoints_ = unpacked.inducingPoints;

  const UnsignedInteger N = inputSample_.getSize();
  const UnsignedInteger M = unpacked.inducingPoints.getSize();
  const UnsignedInteger dimension = unpacked.inducingPoints.getDimension();
  const UnsignedInteger covarianceParameterSize = reducedCovarianceModel_.getParameter().getSize();
  // noiseStdDev is the noise standard deviation; sigma2 is the actual variance
  const Scalar sigma2 = unpacked.noiseStdDev * unpacked.noiseStdDev;
  const Bool hasTrace = (M < N);

  // Forward sweep, as in computeELBOValue()
  const TriangularMatrix Luu(reducedCovarianceModel_.discretize(unpacked.inducingPoints).computeRegularizedCholesky());
  const Matrix Kfu(reducedCovarianceModel_.computeCrossCovariance(inputSample_, unpacked.inducingPoints));
  const Matrix Kuf(Kfu.transpose());
  const Matrix LuuInvKuf(Luu.solveLinearSystem(Kuf));
  const Matrix A(LuuInvKuf.transpose());
  const Matrix AtA(A.transpose() * A);
  CovarianceMatrix G(M);
  CovarianceMatrix B(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      G(i, j) = AtA(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      B(i, j) = G(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    B(i, i) += sigma2;
  const TriangularMatrix Lb(B.computeRegularizedCholesky());
  const Point y(outputSample_.getImplementation()->getData());
  const Point Aty(A.transpose() * y);
  const TriangularMatrix Lg(G.computeRegularizedCholesky());
  const Point w(Lg.transpose().solveLinearSystem(Lg.solveLinearSystem(Aty)));
  const Point u(Lb.solveLinearSystem(w));
  const Point yperp(y - A * w);
  // Tighter-bound weights w_i = 1 / (sigma^2 + r_i) with r_i = k_ii - q_ii >= 0,
  // q_ii the squared norm of row i of A. Only needed when M < N.
  Point tightWeights(N, 0.0);
  Point tightResiduals(N, 0.0);
  if (hasTrace)
  {
    // Only the diagonal of Kff is needed here, evaluate it pointwise
    // instead of discretizing the full matrix
    for (UnsignedInteger i = 0; i < N; ++i)
    {
      Scalar qii = 0.0;
      for (UnsignedInteger j = 0; j < M; ++j)
        qii += A(i, j) * A(i, j);
      const Scalar kii = reducedCovarianceModel_.computeAsScalar(inputSample_[i], inputSample_[i]);
      const Scalar residual = std::max(kii - qii, 0.0);
      tightResiduals[i] = residual;
      tightWeights[i] = 1.0 / (sigma2 + residual);
    }
  }

  // Reverse sweep
  // wBar = -w + A^T yperp / sigma^2 + sigma^2 * Lb^{-T} u
  Point wBar(-w);
  wBar += A.transpose() * (yperp / sigma2);
  wBar += (Lb.transpose().solveLinearSystem(u)) * sigma2;
  // LbBar = -sigma^2 * (Lb^{-T} u) u^T, with diagonal terms -1 / Lb_ii
  const Point LbInvTu(Lb.transpose().solveLinearSystem(u));
  Matrix LbBar(M, M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      LbBar(i, j) = -sigma2 * LbInvTu[i] * u[j];
  for (UnsignedInteger i = 0; i < M; ++i)
    LbBar(i, i) += -1.0 / Lb(i, i);
  // tBar = G^{-1} wBar, GBar = -tBar w^T
  const Point tBar(Lg.transpose().solveLinearSystem(Lg.solveLinearSystem(wBar)));
  Matrix GBar(M, M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      GBar(i, j) = -tBar[i] * w[j];
  // ABar = y tBar^T + yperp w^T / sigma^2
  Matrix ABar(N, M);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      ABar(i, j) = y[i] * tBar[j] + yperp[i] * w[j] / sigma2;
  // BBar = cholAdjoint(Lb, LbBar), GBar += BBar
  const Matrix BBar(cholAdjoint(Lb, LbBar));
  GBar = GBar + BBar;
  // sigma2Bar from the log-det term
  Scalar sigma2Bar = 0.0;
  for (UnsignedInteger i = 0; i < M; ++i)
    sigma2Bar += BBar(i, i);
  // ABar += A (GBar + GBar^T)
  ABar = ABar + A * (GBar + GBar.transpose());
  // Tighter regularization term, only when M < N: ABar(i, :) += w_i * A(i, :)
  if (hasTrace)
    for (UnsignedInteger i = 0; i < N; ++i)
      for (UnsignedInteger j = 0; j < M; ++j)
        ABar(i, j) += tightWeights[i] * A(i, j);
  // sigma2Bar from the quadratic and the tighter regularization terms
  sigma2Bar += 0.5 * u.normSquare() + 0.5 * yperp.normSquare() / (sigma2 * sigma2);
  if (hasTrace)
  {
    Scalar tightSigma2Bar = 0.0;
    for (UnsignedInteger i = 0; i < N; ++i)
      tightSigma2Bar += tightWeights[i] * tightResiduals[i];
    sigma2Bar += 0.5 * tightSigma2Bar / sigma2;
  }
  sigma2Bar -= (SignedInteger(N) - SignedInteger(M)) / (2.0 * sigma2);
  // KfuBar = ABar Luu^{-1}, LuuBar = -Luu^{-T} (ABar^T Kfu) Luu^{-T}
  const Matrix LuuInv(Luu.solveLinearSystem(IdentityMatrix(M)));
  const Matrix KfuBar(ABar * LuuInv);
  const Matrix LuuInvT(Luu.transpose().solveLinearSystem(IdentityMatrix(M)));
  const Matrix LuuBar(-1.0 * (LuuInvT * (ABar.transpose() * Kfu)) * LuuInvT);
  const Matrix KuuBar(cholAdjoint(Luu, LuuBar));
  // Tighter regularization: per-point coefficient -w_i / 2 on k_ii, only when M < N

  // Project the adjoints onto the active covariance parameters and the inducing points
  Point tightCoefficients(N, 0.0);
  if (hasTrace)
    for (UnsignedInteger i = 0; i < N; ++i)
      tightCoefficients[i] = -0.5 * tightWeights[i];
  Point covarianceGradient;
  Point zGradient;
  accumulateGradient(KfuBar, KuuBar, tightCoefficients, unpacked.inducingPoints, covarianceGradient, zGradient);

  // Assemble the gradient in the optimization parameter layout
  Point gradient(getOptimizationParameterSize(), 0.0);
  UnsignedInteger offset = 0;
  for (UnsignedInteger k = 0; k < covarianceParameterSize; ++k)
    gradient[offset + k] = covarianceGradient[k];
  offset += covarianceParameterSize;
  if (optimizeNoiseStdDev_)
  {
    gradient[offset] = 2.0 * sigma2 * sigma2Bar;
    offset += 1;
  }
  if (optimizeInducingPoints_)
  {
    for (UnsignedInteger i = 0; i < M * dimension; ++i)
      gradient[offset + i] = zGradient[i];
  }
  // Restore member state to the values before this gradient call
  noiseStdDev_ = savedNoiseStdDev;
  inducingPoints_ = savedInducingPoints;
  return gradient;
}

/* Compute the gradient of the heteroscedastic collapsed ELBO wrt the optimization parameters.
 * The fixed noise variances carry no gradient: with D = diag(noiseVariances) the bound is
 * differentiated through the scaled quantities tildeA = D^{-1/2} A and tildeY = D^{-1/2} y,
 * on which the likelihood is homoscedastic with unit noise. */
Point SparseGaussianProcessFitter::computeHeteroscedasticELBOGradient(const Point & parameters)
{
  const UnpackedParameters unpacked = unpackParameters(parameters);
  reducedCovarianceModel_.setParameter(unpacked.covarianceParameters);
  // Save member state that the gradient computation temporarily modifies,
  // so that the optimizer can evaluate the objective and gradient at
  // different parameter points without leaving the fitter in an
  // inconsistent state.
  const Sample savedInducingPoints(inducingPoints_);
  if (optimizeInducingPoints_) inducingPoints_ = unpacked.inducingPoints;

  const UnsignedInteger N = inputSample_.getSize();
  const UnsignedInteger M = unpacked.inducingPoints.getSize();
  const UnsignedInteger dimension = unpacked.inducingPoints.getDimension();
  const UnsignedInteger covarianceParameterSize = reducedCovarianceModel_.getParameter().getSize();
  const Bool hasTrace = (M < N);

  // Inverse noise scales s_i = 1 / sqrt(D_i)
  Point inverseScales(N);
  for (UnsignedInteger i = 0; i < N; ++i)
    inverseScales[i] = 1.0 / std::sqrt(noiseVariances_[i]);

  // Forward sweep, as in computeHeteroscedasticELBOValue()
  const TriangularMatrix Luu(reducedCovarianceModel_.discretize(unpacked.inducingPoints).computeRegularizedCholesky());
  const Matrix Kfu(reducedCovarianceModel_.computeCrossCovariance(inputSample_, unpacked.inducingPoints));
  const Matrix Kuf(Kfu.transpose());
  const Matrix LuuInvKuf(Luu.solveLinearSystem(Kuf));
  const Matrix A(LuuInvKuf.transpose());
  const Point y(outputSample_.getImplementation()->getData());
  Matrix scaledA(N, M);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      scaledA(i, j) = inverseScales[i] * A(i, j);
  Point scaledY(N);
  for (UnsignedInteger i = 0; i < N; ++i)
    scaledY[i] = inverseScales[i] * y[i];
  const Matrix scaledAtA(scaledA.transpose() * scaledA);
  CovarianceMatrix G(M);
  CovarianceMatrix B(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      G(i, j) = scaledAtA(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      B(i, j) = G(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    B(i, i) += 1.0;
  const TriangularMatrix Lb(B.computeRegularizedCholesky());
  const Point scaledAty(scaledA.transpose() * scaledY);
  const TriangularMatrix Lg(G.computeRegularizedCholesky());
  const Point w(Lg.transpose().solveLinearSystem(Lg.solveLinearSystem(scaledAty)));
  const Point u(Lb.solveLinearSystem(w));
  const Point scaledYperp(scaledY - scaledA * w);
  // Tighter-bound weights w_i = 1 / (D_i + r_i) with r_i = k_ii - q_ii >= 0,
  // q_ii the squared norm of row i of the UNSCALED factor A. Only needed when M < N.
  Point tightWeights(N, 0.0);
  if (hasTrace)
  {
    for (UnsignedInteger i = 0; i < N; ++i)
    {
      Scalar qii = 0.0;
      for (UnsignedInteger j = 0; j < M; ++j)
        qii += A(i, j) * A(i, j);
      const Scalar kii = reducedCovarianceModel_.computeAsScalar(inputSample_[i], inputSample_[i]);
      const Scalar residual = std::max(kii - qii, 0.0);
      tightWeights[i] = 1.0 / (noiseVariances_[i] + residual);
    }
  }

  // Reverse sweep on the scaled (unit noise) problem
  // wBar = -w + tildeA^T tildeYperp + Lb^{-T} u
  Point wBar(-w);
  wBar += scaledA.transpose() * scaledYperp;
  wBar += Lb.transpose().solveLinearSystem(u);
  // LbBar = -(Lb^{-T} u) u^T, with diagonal terms -1 / Lb_ii
  const Point LbInvTu(Lb.transpose().solveLinearSystem(u));
  Matrix LbBar(M, M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      LbBar(i, j) = -LbInvTu[i] * u[j];
  for (UnsignedInteger i = 0; i < M; ++i)
    LbBar(i, i) += -1.0 / Lb(i, i);
  // tBar = G^{-1} wBar, GBar = -tBar w^T
  const Point tBar(Lg.transpose().solveLinearSystem(Lg.solveLinearSystem(wBar)));
  Matrix GBar(M, M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      GBar(i, j) = -tBar[i] * w[j];
  // tildeABar = tildeY tBar^T + tildeYperp w^T
  Matrix scaledABar(N, M);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      scaledABar(i, j) = scaledY[i] * tBar[j] + scaledYperp[i] * w[j];
  // BBar = cholAdjoint(Lb, LbBar), GBar += BBar
  const Matrix BBar(cholAdjoint(Lb, LbBar));
  GBar = GBar + BBar;
  // tildeABar += tildeA (GBar + GBar^T)
  scaledABar = scaledABar + scaledA * (GBar + GBar.transpose());
  // Chain rule back to the unscaled factor: ABar = D^{-1/2} tildeABar, plus the
  // tighter regularization term w_i * a_i on the unscaled rows, only when M < N
  Matrix ABar(N, M);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      ABar(i, j) = inverseScales[i] * scaledABar(i, j);
  if (hasTrace)
    for (UnsignedInteger i = 0; i < N; ++i)
      for (UnsignedInteger j = 0; j < M; ++j)
        ABar(i, j) += tightWeights[i] * A(i, j);
  // KfuBar = ABar Luu^{-1}, LuuBar = -Luu^{-T} (ABar^T Kfu) Luu^{-T}
  const Matrix LuuInv(Luu.solveLinearSystem(IdentityMatrix(M)));
  const Matrix KfuBar(ABar * LuuInv);
  const Matrix LuuInvT(Luu.transpose().solveLinearSystem(IdentityMatrix(M)));
  const Matrix LuuBar(-1.0 * (LuuInvT * (ABar.transpose() * Kfu)) * LuuInvT);
  const Matrix KuuBar(cholAdjoint(Luu, LuuBar));

  // Project the adjoints onto the covariance parameters and inducing points.
  // The tighter regularization acts on k(x_i, x_i) with coefficient -w_i / 2.
  Point tightCoefficients(N, 0.0);
  if (hasTrace)
    for (UnsignedInteger i = 0; i < N; ++i)
      tightCoefficients[i] = -0.5 * tightWeights[i];
  Point covarianceGradient;
  Point zGradient;
  accumulateGradient(KfuBar, KuuBar, tightCoefficients, unpacked.inducingPoints, covarianceGradient, zGradient);

  // Assemble the gradient in the optimization parameter layout (no noise parameter)
  Point gradient(getOptimizationParameterSize(), 0.0);
  UnsignedInteger offset = 0;
  for (UnsignedInteger k = 0; k < covarianceParameterSize; ++k)
    gradient[offset + k] = covarianceGradient[k];
  offset += covarianceParameterSize;
  if (optimizeInducingPoints_)
  {
    for (UnsignedInteger i = 0; i < M * dimension; ++i)
      gradient[offset + i] = zGradient[i];
  }
  // Restore member state to the values before this gradient call
  inducingPoints_ = savedInducingPoints;
  return gradient;
}

/* Compute the collapsed ELBO for the given inducing points and noise standard deviation */
Scalar SparseGaussianProcessFitter::computeELBOValue(const Sample & inducingPoints,
    const Scalar noiseStdDev)
{
  const UnsignedInteger N = inputSample_.getSize();
  const UnsignedInteger M = inducingPoints.getSize();
  LOGDEBUG(OSS(false) << "Compute the ELBO for M=" << M << " inducing points and noiseStdDev=" << noiseStdDev);
  // Luu = chol(K_uu)
  LOGDEBUG("Discretize the covariance model on the inducing points");
  TriangularMatrix Luu;
  if (method_ == SparseGaussianProcessFitterResult::HMAT)
  {
#ifdef OPENTURNS_HAVE_HMAT
    HMatrixFactory hmatrixFactory;
    HMatrixParameters hmatrixParameters;
    whiteningFactorHMatrix_ = hmatrixFactory.build(inducingPoints, 1, true, hmatrixParameters);
    CovarianceAssemblyFunction simple(reducedCovarianceModel_, inducingPoints);
    whiteningFactorHMatrix_.assemble(simple, 'L');
    whiteningFactorHMatrix_.factorize(hmatrixParameters.getFactorizationMethod());
#else
    throw NotYetImplementedException(HERE) << "OpenTURNS has been built without HMat support";
#endif
  }
  else
    Luu = reducedCovarianceModel_.discretize(inducingPoints).computeRegularizedCholesky();
  // K_fu (N x M)
  LOGDEBUG("Compute the cross covariance matrix");
  const Matrix Kfu(reducedCovarianceModel_.computeCrossCovariance(inputSample_, inducingPoints));
  // A = K_fu * Luu^{-T}, computed as A = (Luu^{-1} * K_fu^T)^T
  LOGDEBUG("Whiten the cross covariance matrix");
  const Matrix Kuf(Kfu.transpose());
  Matrix A;
  if (method_ == SparseGaussianProcessFitterResult::HMAT)
    A = whiteningFactorHMatrix_.solveLower(Kuf).transpose();
  else
    A = Luu.solveLinearSystem(Kuf).transpose();
  if (hasNoiseVariances())
    return computeHeteroscedasticELBOValue(inducingPoints, A, Luu);
  // B = noise^2 * I + A^T A
  const Matrix AtA(A.transpose() * A);
  CovarianceMatrix G(M);
  CovarianceMatrix B(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      G(i, j) = AtA(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      B(i, j) = G(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    B(i, i) += noiseStdDev * noiseStdDev;
  // Lb = chol(B)
  const TriangularMatrix Lb(B.computeRegularizedCholesky());
  // y
  const Point y(outputSample_.getImplementation()->getData());
  // A^T y
  const Point Aty(A.transpose() * y);
  // c = Lb^{-1} (A^T y)
  const Point c(Lb.solveLinearSystem(Aty));
  // m_w = Lb^{-T} c
  const Point mw(Lb.transpose().solveLinearSystem(c));
  // S_ww = noise^2 * B^{-1} = noise^2 * (Lb^{-1})^T (Lb^{-1})
  const Matrix LbInv(Lb.solveLinearSystem(IdentityMatrix(M)));
  const Matrix S(LbInv.transpose() * LbInv);
  CovarianceMatrix Sww(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      Sww(i, j) = noiseStdDev * noiseStdDev * S(i, j);

  LOGDEBUG("Compute the ELBO value");
  // log det(B) = 2 * sum_i log(Lb(i, i))
  Scalar logDetB = 0.0;
  for (UnsignedInteger i = 0; i < M; ++i)
    logDetB += 2.0 * std::log(Lb(i, i));
  const Scalar sigma2 = noiseStdDev * noiseStdDev;
  // Stable evaluation of the quadratic term y^T (Q_ff + sigma^2 I)^{-1} y.
  // Decompose y = A w + y_perp with w the least-squares weights so that
  // y^T (Q_ff + sigma^2 I)^{-1} y = ||w||^2 - sigma^2 ||Lb^{-1} w||^2 + ||y_perp||^2 / sigma^2,
  // which avoids the catastrophic cancellation of (y^T y - c^T c) / sigma^2 for small noise.
  const TriangularMatrix Lg(G.computeRegularizedCholesky());
  const Point w(Lg.transpose().solveLinearSystem(Lg.solveLinearSystem(Aty)));
  const Point u(Lb.solveLinearSystem(w));
  const Point yperp(y - A * w);
  const Scalar quadratic = w.normSquare() - sigma2 * u.normSquare() + yperp.normSquare() / sigma2;
  // Tighter collapsed bound of Titsias (2025): replace tr(K_ff - Q_ff)/(2 sigma^2)
  // with 1/2 sum_i log(1 + (k_ii - q_ii)/sigma^2), q_ii = squared norm of row i of A
  // Collapsed ELBO, see Titsias (2009) with the tighter regularization of
  // Titsias (2025), arXiv:2502.08730 for the whitened formulation
  // When M == N the cross covariance Q_ff = K_fu K_uu^{-1} K_uf equals K_ff, hence
  // all residuals k_ii - q_ii are zero exactly. Skipping the term avoids amplifying
  // the cancellation residue by 1 / sigma^2 for vanishing noise.
  Scalar regularizationTerm = 0.0;
  if (M < N)
  {
    // Only the diagonal of Kff is needed here, evaluate it pointwise
    // instead of discretizing the full matrix
    for (UnsignedInteger i = 0; i < N; ++i)
    {
      Scalar qii = 0.0;
      for (UnsignedInteger j = 0; j < M; ++j)
        qii += A(i, j) * A(i, j);
      const Scalar kii = reducedCovarianceModel_.computeAsScalar(inputSample_[i], inputSample_[i]);
      const Scalar residual = std::max(kii - qii, 0.0);
      regularizationTerm += 0.5 * std::log1p(residual / sigma2);
    }
  }
  const Scalar value = -0.5 * (2.0 * N * SpecFunc::LOGSQRT2PI + (N - M) * std::log(sigma2) + logDetB + quadratic)
                       - regularizationTerm;

  // Store the by-products of the ELBO evaluation
  if (method_ == SparseGaussianProcessFitterResult::HMAT)
    whiteningFactor_ = TriangularMatrix();
  else
    whiteningFactor_ = Luu;
  posteriorMean_ = mw;
  posteriorCovariance_ = Sww;
  lastELBO_ = value;
  LOGDEBUG(OSS(false) << "ELBO=" << value);
  return value;
}

/* Compute the collapsed ELBO with fixed per-observation noise variances D = diag(noiseVariances).
 * With tildeA = D^{-1/2} A and tildeY = D^{-1/2} y the bound reads as in the homoscedastic
 * case with unit noise on the scaled problem: B = tildeA^T tildeA + I_m, m_w = B^{-1} tildeA^T tildeY,
 * S_ww = B^{-1}, and the tighter regularization sums 1/2 log(1 + (k_ii - q_ii) / D_i) with q_ii
 * from the UNSCALED rows of A. */
Scalar SparseGaussianProcessFitter::computeHeteroscedasticELBOValue(const Sample & inducingPoints,
    const Matrix & crossFactor,
    const TriangularMatrix & whiteningFactor)
{
  const UnsignedInteger N = inputSample_.getSize();
  const UnsignedInteger M = inducingPoints.getSize();
  LOGDEBUG(OSS(false) << "Compute the heteroscedastic ELBO for M=" << M << " inducing points");
  // Inverse noise scales s_i = 1 / sqrt(D_i) and log-determinant of D
  Point inverseScales(N);
  Scalar logDetNoise = 0.0;
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    inverseScales[i] = 1.0 / std::sqrt(noiseVariances_[i]);
    logDetNoise += std::log(noiseVariances_[i]);
  }
  // Scaled quantities tildeA = D^{-1/2} A, tildeY = D^{-1/2} y
  Matrix scaledA(N, M);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      scaledA(i, j) = inverseScales[i] * crossFactor(i, j);
  const Point y(outputSample_.getImplementation()->getData());
  Point scaledY(N);
  for (UnsignedInteger i = 0; i < N; ++i)
    scaledY[i] = inverseScales[i] * y[i];
  // B = I + tildeA^T tildeA
  const Matrix scaledAtA(scaledA.transpose() * scaledA);
  CovarianceMatrix G(M);
  CovarianceMatrix B(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      G(i, j) = scaledAtA(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      B(i, j) = G(i, j);
  for (UnsignedInteger i = 0; i < M; ++i)
    B(i, i) += 1.0;
  // Lb = chol(B)
  const TriangularMatrix Lb(B.computeRegularizedCholesky());
  // tildeA^T tildeY
  const Point scaledAty(scaledA.transpose() * scaledY);
  // c = Lb^{-1} (tildeA^T tildeY)
  const Point c(Lb.solveLinearSystem(scaledAty));
  // m_w = Lb^{-T} c
  const Point mw(Lb.transpose().solveLinearSystem(c));
  // S_ww = B^{-1} = (Lb^{-1})^T (Lb^{-1})
  const Matrix LbInv(Lb.solveLinearSystem(IdentityMatrix(M)));
  const Matrix S(LbInv.transpose() * LbInv);
  CovarianceMatrix Sww(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      Sww(i, j) = S(i, j);

  LOGDEBUG("Compute the heteroscedastic ELBO value");
  // log det(B) = 2 * sum_i log(Lb(i, i))
  Scalar logDetB = 0.0;
  for (UnsignedInteger i = 0; i < M; ++i)
    logDetB += 2.0 * std::log(Lb(i, i));
  // Stable evaluation of tildeY^T (tildeA tildeA^T + I)^{-1} tildeY, as in computeELBOValue()
  const TriangularMatrix Lg(G.computeRegularizedCholesky());
  const Point w(Lg.transpose().solveLinearSystem(Lg.solveLinearSystem(scaledAty)));
  const Point u(Lb.solveLinearSystem(w));
  const Point scaledYperp(scaledY - scaledA * w);
  const Scalar quadratic = w.normSquare() - u.normSquare() + scaledYperp.normSquare();
  // Tighter collapsed bound: 1/2 sum_i log(1 + (k_ii - q_ii) / D_i), q_ii the
  // squared norm of row i of the UNSCALED factor. Skipped when M == N, as in
  // computeELBOValue().
  Scalar regularizationTerm = 0.0;
  if (M < N)
  {
    // Only the diagonal of Kff is needed here, evaluate it pointwise
    // instead of discretizing the full matrix
    for (UnsignedInteger i = 0; i < N; ++i)
    {
      Scalar qii = 0.0;
      for (UnsignedInteger j = 0; j < M; ++j)
        qii += crossFactor(i, j) * crossFactor(i, j);
      const Scalar kii = reducedCovarianceModel_.computeAsScalar(inputSample_[i], inputSample_[i]);
      const Scalar residual = std::max(kii - qii, 0.0);
      regularizationTerm += 0.5 * std::log1p(residual / noiseVariances_[i]);
    }
  }
  const Scalar value = -0.5 * (2.0 * N * SpecFunc::LOGSQRT2PI + logDetNoise + logDetB + quadratic)
                       - regularizationTerm;

  // Store the by-products of the ELBO evaluation
  if (method_ == SparseGaussianProcessFitterResult::HMAT)
    whiteningFactor_ = TriangularMatrix();
  else
    whiteningFactor_ = whiteningFactor;
  posteriorMean_ = mw;
  posteriorCovariance_ = Sww;
  lastELBO_ = value;
  LOGDEBUG(OSS(false) << "ELBO=" << value);
  return value;
}

/* Initialize default optimization algorithm */
void SparseGaussianProcessFitter::initializeDefaultOptimizationAlgorithm()
{
  const String solverName(ResourceMap::GetAsString("SparseGaussianProcessFitter-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::GetByName(solverName);
  if ((solverName == "Cobyla") || (solverName == "TNC"))
    solver_.setCheckStatus(false);
}

/* Reset method */
void SparseGaussianProcessFitter::reset()
{
  // Reset elements for new computation
  whiteningFactor_ = TriangularMatrix();
  whiteningFactorHMatrix_ = HMatrix();
  posteriorMean_ = Point();
  posteriorCovariance_ = CovarianceMatrix();
  hasRun_ = false;
  lastELBO_ = SpecFunc::LowestScalar;
  result_ = SparseGaussianProcessFitterResult();
}

/* Build the vector of optimization parameters */
Point SparseGaussianProcessFitter::buildOptimizationParameters() const
{
  Point parameters(reducedCovarianceModel_.getParameter());
  if (optimizeNoiseStdDev_)
    parameters.add(std::log(noiseStdDev_));
  if (optimizeInducingPoints_)
  {
    const UnsignedInteger M = inducingPoints_.getSize();
    const UnsignedInteger dimension = inducingPoints_.getDimension();
    for (UnsignedInteger i = 0; i < M; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
        parameters.add(inducingPoints_(i, j));
  }
  return parameters;
}

/* Build the bounds of the optimization parameters */
void SparseGaussianProcessFitter::buildOptimizationBounds()
{
  // Bounds for the covariance model parameters
  const UnsignedInteger covarianceParameterSize = reducedCovarianceModel_.getParameter().getSize();
  const Scalar lowerBoundScalar = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultOptimizationLowerBound");
  if (!(lowerBoundScalar > 0.0))
    throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultOptimizationLowerBound must be positive, got " << lowerBoundScalar;
  const Scalar upperBoundScalar = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultOptimizationUpperBound");
  if (!(upperBoundScalar > 0.0))
    throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultOptimizationUpperBound must be positive, got " << upperBoundScalar;
  Point lowerBound(covarianceParameterSize, lowerBoundScalar);
  Point upperBound(covarianceParameterSize, upperBoundScalar);
  const Description activeParametersDescription(reducedCovarianceModel_.getParameterDescription());
  Indices activeScalesPositions(0);
  Indices activeScalesIndices(0);
  for (UnsignedInteger k = 0; k < covarianceParameterSize; ++k)
  {
    const String parameterName(activeParametersDescription[k]);
    if (parameterName.find("scale_") != String::npos)
    {
      activeScalesPositions.add(k);
      // Extract the scale index from its description
      activeScalesIndices.add(std::stoi(parameterName.substr(parameterName.find("_") + 1, parameterName.size())));
    }
  }
  if (activeScalesPositions.getSize() > 0)
  {
    const Scalar lowerBoundScaleFactor = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor");
    if (!(lowerBoundScaleFactor > 0.0))
      throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor must be positive, got " << lowerBoundScaleFactor;
    const Scalar upperBoundScaleFactor = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor");
    if (!(upperBoundScaleFactor > 0.0))
      throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor must be positive, got " << upperBoundScaleFactor;
    const Point inputSampleRange(inputSample_.computeRange());
    for (UnsignedInteger k = 0; k < activeScalesPositions.getSize(); ++k)
    {
      const Scalar rangeK = inputSampleRange[activeScalesIndices[k]];
      lowerBound[activeScalesPositions[k]] = rangeK * lowerBoundScaleFactor;
      upperBound[activeScalesPositions[k]] = rangeK * upperBoundScaleFactor;
    }
  }
  // Bounds for the logarithm of the noise variance
  if (optimizeNoiseStdDev_)
  {
    const Scalar noiseLowerBound = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDevLowerBound");
    if (!(noiseLowerBound > 0.0))
      throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultNoiseStdDevLowerBound must be positive, got " << noiseLowerBound;
    const Scalar noiseUpperBound = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDevUpperBound");
    if (!(noiseUpperBound > 0.0))
      throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultNoiseStdDevUpperBound must be positive, got " << noiseUpperBound;
    lowerBound.add(std::log(noiseLowerBound));
    upperBound.add(std::log(noiseUpperBound));
  }
  // Bounds for the inducing points
  if (optimizeInducingPoints_)
  {
    const UnsignedInteger M = inducingPoints_.getSize();
    const UnsignedInteger dimension = inducingPoints_.getDimension();
    const Point sampleMin(inputSample_.getMin());
    const Point sampleMax(inputSample_.getMax());
    const Point sampleRange(sampleMax - sampleMin);
    for (UnsignedInteger i = 0; i < M; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        lowerBound.add(sampleMin[j] - 0.1 * sampleRange[j]);
        upperBound.add(sampleMax[j] + 0.1 * sampleRange[j]);
      }
  }
  optimizationBounds_ = Interval(lowerBound, upperBound);
}

/* Build the optimization parameter description */
Description SparseGaussianProcessFitter::buildOptimizationParameterDescription() const
{
  Description description(reducedCovarianceModel_.getParameterDescription());
  if (optimizeNoiseStdDev_)
    description.add("logNoiseStdDev");
  if (optimizeInducingPoints_)
  {
    const UnsignedInteger M = inducingPoints_.getSize();
    const UnsignedInteger dimension = inducingPoints_.getDimension();
    for (UnsignedInteger i = 0; i < M; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
        description.add(OSS() << "z_" << i << "_" << j);
  }
  return description;
}

/* Size of the optimization parameter vector */
UnsignedInteger SparseGaussianProcessFitter::getOptimizationParameterSize() const
{
  return reducedCovarianceModel_.getParameter().getSize()
         + (optimizeNoiseStdDev_ ? 1 : 0)
         + (optimizeInducingPoints_ ? inducingPoints_.getSize() * inducingPoints_.getDimension() : 0);
}

/* Method save() stores the object through the StorageManager */
void SparseGaussianProcessFitter::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("reducedCovarianceModel_", reducedCovarianceModel_);
  adv.saveAttribute("inducingPoints_", inducingPoints_);
  adv.saveAttribute("noiseStdDev_", noiseStdDev_);
  adv.saveAttribute("noiseVariances_", noiseVariances_);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
  adv.saveAttribute("optimizeParameters_", optimizeParameters_);
  adv.saveAttribute("optimizeInducingPoints_", optimizeInducingPoints_);
  adv.saveAttribute("optimizeNoiseStdDev_", optimizeNoiseStdDev_);
  adv.saveAttribute("hasRun_", hasRun_);
  adv.saveAttribute("lastELBO_", lastELBO_);
  adv.saveAttribute("whiteningFactor_", whiteningFactor_);
  adv.saveAttribute("posteriorMean_", posteriorMean_);
  adv.saveAttribute("posteriorCovariance_", posteriorCovariance_);
  UnsignedInteger method = static_cast<UnsignedInteger>(method_);
  adv.saveAttribute("method_", method);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void SparseGaussianProcessFitter::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("reducedCovarianceModel_", reducedCovarianceModel_);
  adv.loadAttribute("inducingPoints_", inducingPoints_);
  adv.loadAttribute("noiseStdDev_", noiseStdDev_);
  if (adv.hasAttribute("noiseVariances_"))
    adv.loadAttribute("noiseVariances_", noiseVariances_);
  if (hasNoiseVariances()) optimizeNoiseStdDev_ = false;
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
  adv.loadAttribute("optimizeParameters_", optimizeParameters_);
  adv.loadAttribute("optimizeInducingPoints_", optimizeInducingPoints_);
  adv.loadAttribute("optimizeNoiseStdDev_", optimizeNoiseStdDev_);
  adv.loadAttribute("hasRun_", hasRun_);
  adv.loadAttribute("lastELBO_", lastELBO_);
  adv.loadAttribute("whiteningFactor_", whiteningFactor_);
  adv.loadAttribute("posteriorMean_", posteriorMean_);
  adv.loadAttribute("posteriorCovariance_", posteriorCovariance_);
  UnsignedInteger method = 0;
  adv.loadAttribute("method_", method);
  method_ = static_cast<LinearAlgebra>(method);
  if (method_ != SparseGaussianProcessFitterResult::LAPACK)
  {
    LOGDEBUG("SparseGaussianProcessFitter::load: forcing LAPACK after reload (HMatrix is not serializable)");
    method_ = SparseGaussianProcessFitterResult::LAPACK;
    whiteningFactor_ = reducedCovarianceModel_.discretize(inducingPoints_).computeRegularizedCholesky();
  }
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
