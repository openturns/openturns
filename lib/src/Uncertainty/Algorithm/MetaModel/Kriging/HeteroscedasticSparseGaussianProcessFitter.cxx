//                                               -*- C++ -*-
/**
 *  @brief The class fits heteroscedastic sparse gaussian process models
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

#include "openturns/HeteroscedasticSparseGaussianProcessFitter.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/MemoizeFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SparseGaussianProcessEvaluation.hxx"
#include "openturns/SparseGaussianProcessGradient.hxx"
#include "openturns/SparseGaussianProcessHessian.hxx"
#include "openturns/CholAdjoint.hxx"

#include <algorithm>
#include <vector>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HeteroscedasticSparseGaussianProcessFitter)

static const Factory<HeteroscedasticSparseGaussianProcessFitter> Factory_HeteroscedasticSparseGaussianProcessFitter;

/* Default constructor */
HeteroscedasticSparseGaussianProcessFitter::HeteroscedasticSparseGaussianProcessFitter()
  : MetaModelAlgorithm()
  , mu0_(2.0 * std::log(ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDev")))
{
  // Nothing to do
}

/* Parameters constructor */
HeteroscedasticSparseGaussianProcessFitter::HeteroscedasticSparseGaussianProcessFitter(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModelF,
    const CovarianceModel & covarianceModelG,
    const Sample & inducingPointsF,
    const Sample & inducingPointsG)
  : MetaModelAlgorithm(inputSample, outputSample)
  , mu0_(2.0 * std::log(ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultNoiseStdDev")))
{
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::HeteroscedasticSparseGaussianProcessFitter, the input sample size (" << inputSample.getSize() << ") should be equal to the output sample size (" << outputSample.getSize() << ")";
  setCovarianceModelF(covarianceModelF);
  setCovarianceModelG(covarianceModelG);
  setInducingPointsF(inducingPointsF);
  setInducingPointsG(inducingPointsG);
  buildReplicateStructure();
  // Default variational posteriors: prior-matched (zero mean, identity covariance)
  const UnsignedInteger M = inducingPointsF.getSize();
  variationalMeanF_ = Point(M, 0.0);
  variationalCovarianceF_ = CovarianceMatrix(M);
  for (UnsignedInteger i = 0; i < M; ++i)
    variationalCovarianceF_(i, i) = 1.0;
  const UnsignedInteger U = inducingPointsG.getSize();
  variationalMeanG_ = Point(U, 0.0);
  variationalCovarianceG_ = CovarianceMatrix(U);
  for (UnsignedInteger i = 0; i < U; ++i)
    variationalCovarianceG_(i, i) = 1.0;
  initializeDefaultOptimizationAlgorithm();
  buildOptimizationBounds();
}

/* Virtual constructor */
HeteroscedasticSparseGaussianProcessFitter * HeteroscedasticSparseGaussianProcessFitter::clone() const
{
  return new HeteroscedasticSparseGaussianProcessFitter(*this);
}

/* String converter */
String HeteroscedasticSparseGaussianProcessFitter::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << ", inputSample=" << inputSample_
      << ", outputSample=" << outputSample_
      << ", covarianceModelF=" << covarianceModelF_
      << ", covarianceModelG=" << covarianceModelG_
      << ", inducingPointsF=" << inducingPointsF_
      << ", inducingPointsG=" << inducingPointsG_
      << ", mu0=" << mu0_
      << ", solver=" << solver_
      << ", optimizeParameters=" << optimizeParameters_
      << ", optimizeVariational=" << optimizeVariational_;
  return oss;
}

/* Perform regression */
void HeteroscedasticSparseGaussianProcessFitter::run()
{
  // Do not run again if already computed
  if (hasRun_) return;
  // optimization of the ELBO if there is at least one parameter to optimize
  Scalar optimalELBO = maximizeELBO();

  LOGDEBUG("Store the estimates");
  LOGDEBUG("Build the output meta-model");
  // return optimized covmodels with the original active parameters
  CovarianceModel reducedCovarianceModelFCopy(reducedCovarianceModelF_);
  reducedCovarianceModelFCopy.setActiveParameter(covarianceModelF_.getActiveParameter());

  // The mean prediction reuses the sparse evaluation machinery of the
  // homoscedastic fitter with the f-process quantities
  SparseGaussianProcessEvaluation evaluation(reducedCovarianceModelFCopy, inducingPointsF_, whiteningFactorF_, variationalMeanF_, variationalCovarianceF_, HMatrix(), SparseGaussianProcessFitterResult::LAPACK);
  Function metaModel(evaluation);
  metaModel.setInputDescription(inputSample_.getDescription());
  metaModel.setOutputDescription(outputSample_.getDescription());
  metaModel.setGradient(new SparseGaussianProcessGradient(reducedCovarianceModelFCopy, inducingPointsF_, whiteningFactorF_, variationalMeanF_, HMatrix(), SparseGaussianProcessFitterResult::LAPACK));
  metaModel.setHessian(new SparseGaussianProcessHessian(reducedCovarianceModelFCopy, inducingPointsF_, whiteningFactorF_, variationalMeanF_, HMatrix(), SparseGaussianProcessFitterResult::LAPACK));

  CovarianceModel reducedCovarianceModelGCopy(reducedCovarianceModelG_);
  reducedCovarianceModelGCopy.setActiveParameter(covarianceModelG_.getActiveParameter());
  result_ = HeteroscedasticSparseGaussianProcessFitterResult(inputSample_, outputSample_, reducedCovarianceModelFCopy, reducedCovarianceModelGCopy, inducingPointsF_, inducingPointsG_, whiteningFactorF_, whiteningFactorG_, variationalMeanF_, variationalCovarianceF_, variationalMeanG_, variationalCovarianceG_, mu0_, optimalELBO, metaModel);
  hasRun_ = true;
}

/* Result accessor */
HeteroscedasticSparseGaussianProcessFitterResult HeteroscedasticSparseGaussianProcessFitter::getResult()
{
  if (!hasRun_) run();
  return result_;
}

/* Objective function accessor */
Function HeteroscedasticSparseGaussianProcessFitter::getObjectiveFunction()
{
  MemoizeFunction objective(ELBOEvaluation(*this));
  objective.setGradient(ELBOGradient(*this).clone());
  objective.enableCache();
  return objective;
}

/* Optimization solver accessor */
OptimizationAlgorithm HeteroscedasticSparseGaussianProcessFitter::getOptimizationAlgorithm() const
{
  return solver_;
}

void HeteroscedasticSparseGaussianProcessFitter::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
  reset();
}

/* Optimize parameters flag accessor */
Bool HeteroscedasticSparseGaussianProcessFitter::getOptimizeParameters() const
{
  return optimizeParameters_;
}

void HeteroscedasticSparseGaussianProcessFitter::setOptimizeParameters(const Bool optimizeParameters)
{
  if (optimizeParameters != optimizeParameters_)
  {
    optimizeParameters_ = optimizeParameters;
    // Here we have to call the covariance setters as they compute the reduced models
    // in a way influenced by optimizeParameters_ flag.
    setCovarianceModelF(covarianceModelF_);
    setCovarianceModelG(covarianceModelG_);
  }
}

/* Optimize variational parameters flag accessor */
Bool HeteroscedasticSparseGaussianProcessFitter::getOptimizeVariational() const
{
  return optimizeVariational_;
}

void HeteroscedasticSparseGaussianProcessFitter::setOptimizeVariational(const Bool optimizeVariational)
{
  if (optimizeVariational != optimizeVariational_)
  {
    optimizeVariational_ = optimizeVariational;
    reset();
    buildOptimizationBounds();
  }
}

/* Prior mean of the log-variance process accessor */
Scalar HeteroscedasticSparseGaussianProcessFitter::getMu0() const
{
  return mu0_;
}

void HeteroscedasticSparseGaussianProcessFitter::setMu0(const Scalar mu0)
{
  if (mu0 != mu0_)
  {
    mu0_ = mu0;
    reset();
  }
}

/* Inducing points accessors */
Sample HeteroscedasticSparseGaussianProcessFitter::getInducingPointsF() const
{
  return inducingPointsF_;
}

void HeteroscedasticSparseGaussianProcessFitter::setInducingPointsF(const Sample & inducingPointsF)
{
  if (inducingPointsF.getDimension() != inputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setInducingPointsF, the inducing points dimension (" << inducingPointsF.getDimension() << ") should match the input sample dimension (" << inputSample_.getDimension() << ")";
  const UnsignedInteger size = inputSample_.getSize();
  if (inducingPointsF.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setInducingPointsF, the number of inducing points should be positive";
  if (inducingPointsF.getSize() > size)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setInducingPointsF, the number of inducing points (" << inducingPointsF.getSize() << ") should not exceed the number of observations (" << size << ")";
  inducingPointsF_ = inducingPointsF;
  reset();
  buildOptimizationBounds();
}

Sample HeteroscedasticSparseGaussianProcessFitter::getInducingPointsG() const
{
  return inducingPointsG_;
}

void HeteroscedasticSparseGaussianProcessFitter::setInducingPointsG(const Sample & inducingPointsG)
{
  if (inducingPointsG.getDimension() != inputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setInducingPointsG, the inducing points dimension (" << inducingPointsG.getDimension() << ") should match the input sample dimension (" << inputSample_.getDimension() << ")";
  const UnsignedInteger size = inputSample_.getSize();
  if (inducingPointsG.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setInducingPointsG, the number of inducing points should be positive";
  if (inducingPointsG.getSize() > size)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setInducingPointsG, the number of inducing points (" << inducingPointsG.getSize() << ") should not exceed the number of observations (" << size << ")";
  inducingPointsG_ = inducingPointsG;
  reset();
  buildOptimizationBounds();
}

/* Replicate structure accessors */
Sample HeteroscedasticSparseGaussianProcessFitter::getUniqueInputSample() const
{
  return uniqueInputSample_;
}

Indices HeteroscedasticSparseGaussianProcessFitter::getReplicateCounts() const
{
  return replicateCounts_;
}

Sample HeteroscedasticSparseGaussianProcessFitter::getReplicateMeanOutput() const
{
  const UnsignedInteger siteNumber = uniqueInputSample_.getSize();
  Sample means(siteNumber, 1);
  for (UnsignedInteger s = 0; s < siteNumber; ++s)
    means(s, 0) = replicateSums_[s] / replicateCounts_[s];
  return means;
}

/* Detect exact input replicates and build the per-site sufficient statistics */
void HeteroscedasticSparseGaussianProcessFitter::buildReplicateStructure()
{
  const UnsignedInteger size = inputSample_.getSize();
  const UnsignedInteger dimension = inputSample_.getDimension();
  // Order the observation indices lexicographically for exact-duplicate grouping
  std::vector<UnsignedInteger> order(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    order[i] = i;
  const Sample inputSample(inputSample_);
  std::sort(order.begin(), order.end(), [&inputSample, dimension](UnsignedInteger a, UnsignedInteger b)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      if (inputSample(a, j) < inputSample(b, j)) return true;
      if (inputSample(a, j) > inputSample(b, j)) return false;
    }
    return a < b;
  });
  // Single-pass grouping of the ordered indices
  uniqueInputSample_ = Sample(0, dimension);
  replicateCounts_ = Indices(0);
  replicateSums_ = Point(0);
  replicateSumSquares_ = Point(0);
  const Point observations(outputSample_.getImplementation()->getData());
  UnsignedInteger start = 0;
  while (start < size)
  {
    UnsignedInteger end = start + 1;
    while (end < size)
    {
      Bool equal = true;
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if (inputSample(order[end], j) != inputSample(order[start], j))
        {
          equal = false;
          break;
        }
      if (!equal) break;
      end += 1;
    }
    uniqueInputSample_.add(inputSample_[order[start]]);
    replicateCounts_.add(end - start);
    Scalar sum = 0.0;
    Scalar sumSquares = 0.0;
    for (UnsignedInteger k = start; k < end; ++k)
    {
      sum += observations[order[k]];
      sumSquares += observations[order[k]] * observations[order[k]];
    }
    replicateSums_.add(sum);
    replicateSumSquares_.add(sumSquares);
    start = end;
  }
}
/* Covariance models accessors */
CovarianceModel HeteroscedasticSparseGaussianProcessFitter::getCovarianceModelF() const
{
  return covarianceModelF_;
}

CovarianceModel HeteroscedasticSparseGaussianProcessFitter::getCovarianceModelG() const
{
  return covarianceModelG_;
}

CovarianceModel HeteroscedasticSparseGaussianProcessFitter::getReducedCovarianceModelF() const
{
  return reducedCovarianceModelF_;
}

CovarianceModel HeteroscedasticSparseGaussianProcessFitter::getReducedCovarianceModelG() const
{
  return reducedCovarianceModelG_;
}

/* Variational parameters accessors */
Point HeteroscedasticSparseGaussianProcessFitter::getVariationalMeanF() const
{
  return variationalMeanF_;
}

CovarianceMatrix HeteroscedasticSparseGaussianProcessFitter::getVariationalCovarianceF() const
{
  return variationalCovarianceF_;
}

Point HeteroscedasticSparseGaussianProcessFitter::getVariationalMeanG() const
{
  return variationalMeanG_;
}

CovarianceMatrix HeteroscedasticSparseGaussianProcessFitter::getVariationalCovarianceG() const
{
  return variationalCovarianceG_;
}

void HeteroscedasticSparseGaussianProcessFitter::setCovarianceModelF(const CovarianceModel & covarianceModelF)
{
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  const UnsignedInteger outputDimension = outputSample_.getDimension();

  if (covarianceModelF.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Mean covariance model input dimension is " << covarianceModelF.getInputDimension() << ", expected " << inputDimension;
  if (covarianceModelF.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Mean covariance model output dimension is " << covarianceModelF.getOutputDimension() << ", expected " << outputDimension;
  if (outputDimension != 1)
    throw NotYetImplementedException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setCovarianceModelF, heteroscedastic sparse Gaussian processes only support scalar outputs for now";
  covarianceModelF_ = covarianceModelF;
  reducedCovarianceModelF_ = covarianceModelF_;
  if (!optimizeParameters_) reducedCovarianceModelF_.setActiveParameter(Indices());
  reset();
  buildOptimizationBounds();
}

void HeteroscedasticSparseGaussianProcessFitter::setCovarianceModelG(const CovarianceModel & covarianceModelG)
{
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  const UnsignedInteger outputDimension = outputSample_.getDimension();

  if (covarianceModelG.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Log-variance covariance model input dimension is " << covarianceModelG.getInputDimension() << ", expected " << inputDimension;
  if (covarianceModelG.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Log-variance covariance model output dimension is " << covarianceModelG.getOutputDimension() << ", expected " << outputDimension;
  if (outputDimension != 1)
    throw NotYetImplementedException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter::setCovarianceModelG, heteroscedastic sparse Gaussian processes only support scalar outputs for now";
  covarianceModelG_ = covarianceModelG;
  reducedCovarianceModelG_ = covarianceModelG_;
  if (!optimizeParameters_) reducedCovarianceModelG_.setActiveParameter(Indices());
  reset();
  buildOptimizationBounds();
}

// Maximize the ELBO of the heteroscedastic sparse Gaussian process model
Scalar HeteroscedasticSparseGaussianProcessFitter::maximizeELBO()
{
  // initial guess
  Point initialParameters(buildOptimizationParameters());
  // We use the functional form of the ELBO computation to benefit from the cache mechanism
  Function objectiveFunction(getObjectiveFunction());
  const Bool noNumericalOptimization = initialParameters.getSize() == 0;
  // Early exit if the parameters are known
  if (noNumericalOptimization)
  {
    // Call computeELBO() directly on *this to get the by-products. The function
    // wrapper is bypassed because the cache provides no benefit for a single
    // evaluation, and the direct call makes the side-effect intent explicit.
    const Scalar initialELBO = computeELBO(initialParameters)[0];
    LOGDEBUG("No parameter to optimize");
    LOGDEBUG(OSS() << "initial parameters=" << initialParameters << ", ELBO=" << initialELBO);
    return initialELBO;
  }
  // Thus here we perform an optimization. First let us check the initial point is inside the
  // optimization bounds search, otherwise define one arbitrary inside these bounds
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
    throw InvalidArgumentException(HERE) << "optimization in HeteroscedasticSparseGaussianProcessFitter did not yield feasible points";
  Scalar optimalELBO = optimalELBOPoint[0];
  Point optimalParameters(result.getOptimalPoint());
  if (normalization)
    optimalParameters = uToX(optimalParameters);

  const UnsignedInteger evaluationNumber = result.getCallsNumber();
  // Recompute the ELBO at the optimal parameters, so that the returned value is
  // consistent with the by-products stored at the same point.
  // No additional cost when the cache is enabled.
  LOGDEBUG(OSS(false) << "Need to evaluate the objective function one more time because the last computed ELBO value=" << lastELBO_ << " is different from the optimal one=" << optimalELBO);
  (void) computeELBO(optimalParameters);
  optimalELBO = lastELBO_;
  // Final call to objectiveFunction() in order to update the by-products
  // No additional cost since the cache mechanism is activated
  LOGDEBUG(OSS() << evaluationNumber << " evaluations, optimized parameters=" << optimalParameters << ", ELBO=" << optimalELBO);

  return optimalELBO;
}

/* Unpack the optimization parameter vector */
HeteroscedasticSparseGaussianProcessFitter::UnpackedParameters HeteroscedasticSparseGaussianProcessFitter::unpackParameters(const Point & parameters) const
{
  UnsignedInteger offset = 0;
  const UnsignedInteger covarianceParameterSizeF = reducedCovarianceModelF_.getParameter().getSize();
  const UnsignedInteger covarianceParameterSizeG = reducedCovarianceModelG_.getParameter().getSize();
  const UnsignedInteger M = inducingPointsF_.getSize();
  const UnsignedInteger U = inducingPointsG_.getSize();
  const UnsignedInteger expectedSize = getOptimizationParameterSize();
  if (parameters.getSize() != expectedSize)
    throw InvalidArgumentException(HERE) << "In HeteroscedasticSparseGaussianProcessFitter, the parameter vector should be of size " << expectedSize
                                         << " but here we got " << parameters.getSize();
  UnpackedParameters unpacked;
  unpacked.covarianceParametersF = Point(covarianceParameterSizeF);
  for (UnsignedInteger i = 0; i < covarianceParameterSizeF; ++i)
    unpacked.covarianceParametersF[i] = parameters[offset + i];
  offset += covarianceParameterSizeF;
  unpacked.covarianceParametersG = Point(covarianceParameterSizeG);
  for (UnsignedInteger i = 0; i < covarianceParameterSizeG; ++i)
    unpacked.covarianceParametersG[i] = parameters[offset + i];
  offset += covarianceParameterSizeG;
  unpacked.mu0 = mu0_;
  if (optimizeParameters_)
  {
    unpacked.mu0 = parameters[offset];
    offset += 1;
  }
  unpacked.variationalMeanF = variationalMeanF_;
  unpacked.variationalCholF = Matrix(M, M);
  unpacked.variationalMeanG = variationalMeanG_;
  unpacked.variationalCholG = Matrix(U, U);
  if (optimizeVariational_)
  {
    for (UnsignedInteger i = 0; i < M; ++i)
      unpacked.variationalMeanF[i] = parameters[offset + i];
    offset += M;
    for (UnsignedInteger p = 0; p < M; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
      {
        // Diagonal entries are stored as logarithms for unconstrained optimization
        if (p == q) unpacked.variationalCholF(p, q) = std::exp(parameters[offset]);
        else unpacked.variationalCholF(p, q) = parameters[offset];
        offset += 1;
      }
    for (UnsignedInteger i = 0; i < U; ++i)
      unpacked.variationalMeanG[i] = parameters[offset + i];
    offset += U;
    for (UnsignedInteger p = 0; p < U; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
      {
        if (p == q) unpacked.variationalCholG(p, q) = std::exp(parameters[offset]);
        else unpacked.variationalCholG(p, q) = parameters[offset];
        offset += 1;
      }
  }
  return unpacked;
}

Point HeteroscedasticSparseGaussianProcessFitter::computeELBO(const Point & parameters)
{
  LOGDEBUG(OSS(false) << "Compute ELBO for parameters=" << parameters);
  const UnpackedParameters unpacked = unpackParameters(parameters);
  reducedCovarianceModelF_.setParameter(unpacked.covarianceParametersF);
  reducedCovarianceModelG_.setParameter(unpacked.covarianceParametersG);
  // Store the unpacked values into the members so that they are consistent
  // with the last computed ELBO (see also run())
  if (optimizeParameters_) mu0_ = unpacked.mu0;
  if (optimizeVariational_)
  {
    variationalMeanF_ = unpacked.variationalMeanF;
    variationalMeanG_ = unpacked.variationalMeanG;
    const Matrix cholFt(unpacked.variationalCholF.transpose());
    const Matrix scatterF(unpacked.variationalCholF * cholFt);
    const UnsignedInteger M = inducingPointsF_.getSize();
    variationalCovarianceF_ = CovarianceMatrix(M);
    for (UnsignedInteger i = 0; i < M; ++i)
      for (UnsignedInteger j = 0; j < M; ++j)
        variationalCovarianceF_(i, j) = scatterF(i, j);
    const Matrix cholGt(unpacked.variationalCholG.transpose());
    const Matrix scatterG(unpacked.variationalCholG * cholGt);
    const UnsignedInteger U = inducingPointsG_.getSize();
    variationalCovarianceG_ = CovarianceMatrix(U);
    for (UnsignedInteger i = 0; i < U; ++i)
      for (UnsignedInteger j = 0; j < U; ++j)
        variationalCovarianceG_(i, j) = scatterG(i, j);
  }
  lastELBO_ = computeELBOValue();
  return Point(1, lastELBO_);
}

/* Compute the gradient of the uncollapsed ELBO wrt the optimization parameters */
Point HeteroscedasticSparseGaussianProcessFitter::computeELBOGradient(const Point & parameters)
{
  const UnpackedParameters unpacked = unpackParameters(parameters);
  reducedCovarianceModelF_.setParameter(unpacked.covarianceParametersF);
  reducedCovarianceModelG_.setParameter(unpacked.covarianceParametersG);
  // Save member state that the gradient computation temporarily modifies
  const Scalar savedMu0 = mu0_;
  const Point savedMeanF(variationalMeanF_);
  const CovarianceMatrix savedCovarianceF(variationalCovarianceF_);
  const Point savedMeanG(variationalMeanG_);
  const CovarianceMatrix savedCovarianceG(variationalCovarianceG_);
  if (optimizeParameters_) mu0_ = unpacked.mu0;
  Matrix cholF;
  Matrix cholG;
  if (optimizeVariational_)
  {
    variationalMeanF_ = unpacked.variationalMeanF;
    variationalMeanG_ = unpacked.variationalMeanG;
    const Matrix cholFt(unpacked.variationalCholF.transpose());
    const Matrix scatterF(unpacked.variationalCholF * cholFt);
    const Matrix cholGt(unpacked.variationalCholG.transpose());
    const Matrix scatterG(unpacked.variationalCholG * cholGt);
    const UnsignedInteger M = inducingPointsF_.getSize();
    variationalCovarianceF_ = CovarianceMatrix(M);
    for (UnsignedInteger i = 0; i < M; ++i)
      for (UnsignedInteger j = 0; j < M; ++j)
        variationalCovarianceF_(i, j) = scatterF(i, j);
    const UnsignedInteger U = inducingPointsG_.getSize();
    variationalCovarianceG_ = CovarianceMatrix(U);
    for (UnsignedInteger i = 0; i < U; ++i)
      for (UnsignedInteger j = 0; j < U; ++j)
        variationalCovarianceG_(i, j) = scatterG(i, j);
    cholF = unpacked.variationalCholF;
    cholG = unpacked.variationalCholG;
  }
  else
  {
    // Cholesky factors of the stored covariances for the vech chain rule
    const TriangularMatrix cholFt(variationalCovarianceF_.computeRegularizedCholesky());
    cholF = Matrix(cholFt);
    const TriangularMatrix cholGt(variationalCovarianceG_.computeRegularizedCholesky());
    cholG = Matrix(cholGt);
  }

  const UnsignedInteger N = uniqueInputSample_.getSize();
  const UnsignedInteger M = inducingPointsF_.getSize();
  const UnsignedInteger U = inducingPointsG_.getSize();
  const UnsignedInteger covarianceParameterSizeF = reducedCovarianceModelF_.getParameter().getSize();
  const UnsignedInteger covarianceParameterSizeG = reducedCovarianceModelG_.getParameter().getSize();

  // Forward sweep on the mean process
  const TriangularMatrix LuuF(reducedCovarianceModelF_.discretize(inducingPointsF_).computeRegularizedCholesky());
  const Matrix KfuF(reducedCovarianceModelF_.computeCrossCovariance(uniqueInputSample_, inducingPointsF_));
  const Matrix KufF(KfuF.transpose());
  const Matrix AF(LuuF.solveLinearSystem(KufF).transpose());
  const Point muF(AF * variationalMeanF_);
  Point qf(N, 0.0);
  Point vf(N, 0.0);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    Scalar norm2 = 0.0;
    for (UnsignedInteger j = 0; j < M; ++j)
      norm2 += AF(i, j) * AF(i, j);
    qf[i] = norm2;
    const Scalar kii = reducedCovarianceModelF_.computeAsScalar(uniqueInputSample_[i], uniqueInputSample_[i]);
    Point Sa(M, 0.0);
    for (UnsignedInteger p = 0; p < M; ++p)
      for (UnsignedInteger q = 0; q < M; ++q)
        Sa[p] += variationalCovarianceF_(p, q) * AF(i, q);
    Scalar aSa = 0.0;
    for (UnsignedInteger j = 0; j < M; ++j)
      aSa += AF(i, j) * Sa[j];
    vf[i] = kii - norm2 + aSa;
  }
  // Forward sweep on the log-variance process
  const TriangularMatrix LuuG(reducedCovarianceModelG_.discretize(inducingPointsG_).computeRegularizedCholesky());
  const Matrix KfuG(reducedCovarianceModelG_.computeCrossCovariance(uniqueInputSample_, inducingPointsG_));
  const Matrix KufG(KfuG.transpose());
  const Matrix AG(LuuG.solveLinearSystem(KufG).transpose());
  const Point muGShift(AG * variationalMeanG_);
  Point muG(N, mu0_);
  for (UnsignedInteger i = 0; i < N; ++i)
    muG[i] += muGShift[i];
  Point vg(N, 0.0);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    Scalar norm2 = 0.0;
    for (UnsignedInteger j = 0; j < U; ++j)
      norm2 += AG(i, j) * AG(i, j);
    const Scalar kii = reducedCovarianceModelG_.computeAsScalar(uniqueInputSample_[i], uniqueInputSample_[i]);
    Point Sb(U, 0.0);
    for (UnsignedInteger p = 0; p < U; ++p)
      for (UnsignedInteger q = 0; q < U; ++q)
        Sb[p] += variationalCovarianceG_(p, q) * AG(i, q);
    Scalar bSb = 0.0;
    for (UnsignedInteger j = 0; j < U; ++j)
      bSb += AG(i, j) * Sb[j];
    vg[i] = kii - norm2 + bSb;
  }
  // Per-site expected log-likelihood terms from the sufficient statistics
  // (counts, sums and sums of squares): exact aggregation of the replicates
  Point mufBar(N), vfBar(N), mugBar(N), vgBar(N);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    const Scalar counts = 1.0 * replicateCounts_[i];
    const Scalar energy = replicateSumSquares_[i] - 2.0 * muF[i] * replicateSums_[i]
                          + counts * (muF[i] * muF[i] + vf[i]);
    const Scalar expo = std::exp(-muG[i] + 0.5 * vg[i]);
    mufBar[i] = (replicateSums_[i] - counts * muF[i]) * expo;
    vfBar[i] = -0.5 * counts * expo;
    mugBar[i] = -0.5 * counts + 0.5 * energy * expo;
    vgBar[i] = -0.25 * energy * expo;
  }
  // KL divergences in the whitened parametrisation
  // KL divergences in the whitened parametrisation with centered priors need
  // the inverse covariances only (log-determinants are value-only terms)
  const TriangularMatrix cholSf(variationalCovarianceF_.computeRegularizedCholesky());
  const Matrix cholSfInv(cholSf.solveLinearSystem(IdentityMatrix(M)));
  const Matrix SinvF(cholSfInv.transpose() * cholSfInv);
  const TriangularMatrix cholSg(variationalCovarianceG_.computeRegularizedCholesky());
  const Matrix cholSgInv(cholSg.solveLinearSystem(IdentityMatrix(U)));
  const Matrix SinvG(cholSgInv.transpose() * cholSgInv);

  // Reverse sweep on the mean process
  Point mBarF(AF.transpose() * mufBar);
  for (UnsignedInteger i = 0; i < M; ++i)
    mBarF[i] -= variationalMeanF_[i];
  Matrix ABarF(N, M);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      ABarF(i, j) = mufBar[i] * variationalMeanF_[j];
  Matrix SBarF(M, M);
  for (UnsignedInteger i = 0; i < M; ++i)
    for (UnsignedInteger j = 0; j < M; ++j)
      SBarF(i, j) = -0.5 * (i == j ? 1.0 - SinvF(i, j) : -SinvF(i, j));
  Point diagCoefF(N);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    diagCoefF[i] = vfBar[i];
    Point Sa(M, 0.0);
    for (UnsignedInteger p = 0; p < M; ++p)
      for (UnsignedInteger q = 0; q < M; ++q)
        Sa[p] += variationalCovarianceF_(p, q) * AF(i, q);
    for (UnsignedInteger j = 0; j < M; ++j)
    {
      ABarF(i, j) += 2.0 * vfBar[i] * (Sa[j] - AF(i, j));
      for (UnsignedInteger q = 0; q < M; ++q)
        SBarF(j, q) += vfBar[i] * AF(i, j) * AF(i, q);
    }
  }
  // Reverse sweep on the log-variance process
  Point mBarG(AG.transpose() * mugBar);
  for (UnsignedInteger i = 0; i < U; ++i)
    mBarG[i] -= variationalMeanG_[i];
  Matrix ABarG(N, U);
  for (UnsignedInteger i = 0; i < N; ++i)
    for (UnsignedInteger j = 0; j < U; ++j)
      ABarG(i, j) = mugBar[i] * variationalMeanG_[j];
  Matrix SBarG(U, U);
  for (UnsignedInteger i = 0; i < U; ++i)
    for (UnsignedInteger j = 0; j < U; ++j)
      SBarG(i, j) = -0.5 * (i == j ? 1.0 - SinvG(i, j) : -SinvG(i, j));
  Point diagCoefG(N);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    diagCoefG[i] = vgBar[i];
    Point Sb(U, 0.0);
    for (UnsignedInteger p = 0; p < U; ++p)
      for (UnsignedInteger q = 0; q < U; ++q)
        Sb[p] += variationalCovarianceG_(p, q) * AG(i, q);
    for (UnsignedInteger j = 0; j < U; ++j)
    {
      ABarG(i, j) += 2.0 * vgBar[i] * (Sb[j] - AG(i, j));
      for (UnsignedInteger q = 0; q < U; ++q)
        SBarG(j, q) += vgBar[i] * AG(i, j) * AG(i, q);
    }
  }
  Scalar mu0Bar = 0.0;
  for (UnsignedInteger i = 0; i < N; ++i)
    mu0Bar += mugBar[i];
  // Project onto the cross- and self-covariances
  const Matrix LuuFInv(LuuF.solveLinearSystem(IdentityMatrix(M)));
  const Matrix KfuBarF(ABarF * LuuFInv);
  const Matrix LuuFInvT(LuuF.transpose().solveLinearSystem(IdentityMatrix(M)));
  const Matrix LuuBarF(-1.0 * (LuuFInvT * (ABarF.transpose() * KfuF)) * LuuFInvT);
  const Matrix KuuBarF(cholAdjoint(LuuF, LuuBarF));
  const Matrix LuuGInv(LuuG.solveLinearSystem(IdentityMatrix(U)));
  const Matrix KfuBarG(ABarG * LuuGInv);
  const Matrix LuuGInvT(LuuG.transpose().solveLinearSystem(IdentityMatrix(U)));
  const Matrix LuuBarG(-1.0 * (LuuGInvT * (ABarG.transpose() * KfuG)) * LuuGInvT);
  const Matrix KuuBarG(cholAdjoint(LuuG, LuuBarG));
  // Project onto the covariance parameters
  Point covarianceGradientF(covarianceParameterSizeF, 0.0);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    for (UnsignedInteger j = 0; j < M; ++j)
    {
      const Scalar coef = KfuBarF(i, j);
      if (coef != 0.0)
      {
        const Matrix dk(reducedCovarianceModelF_.parameterGradient(uniqueInputSample_[i], inducingPointsF_[j]));
        for (UnsignedInteger k = 0; k < covarianceParameterSizeF; ++k)
          covarianceGradientF[k] += coef * dk(k, 0);
      }
    }
    if (diagCoefF[i] != 0.0)
    {
      const Matrix dk(reducedCovarianceModelF_.parameterGradient(uniqueInputSample_[i], uniqueInputSample_[i]));
      for (UnsignedInteger k = 0; k < covarianceParameterSizeF; ++k)
        covarianceGradientF[k] += diagCoefF[i] * dk(k, 0);
    }
  }
  for (UnsignedInteger p = 0; p < M; ++p)
  {
    for (UnsignedInteger q = 0; q < M; ++q)
    {
      const Scalar coef = KuuBarF(p, q);
      if (coef != 0.0)
      {
        const Matrix dk(reducedCovarianceModelF_.parameterGradient(inducingPointsF_[p], inducingPointsF_[q]));
        for (UnsignedInteger k = 0; k < covarianceParameterSizeF; ++k)
          covarianceGradientF[k] += coef * dk(k, 0);
      }
    }
  }
  Point covarianceGradientG(covarianceParameterSizeG, 0.0);
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    for (UnsignedInteger j = 0; j < U; ++j)
    {
      const Scalar coef = KfuBarG(i, j);
      if (coef != 0.0)
      {
        const Matrix dk(reducedCovarianceModelG_.parameterGradient(uniqueInputSample_[i], inducingPointsG_[j]));
        for (UnsignedInteger k = 0; k < covarianceParameterSizeG; ++k)
          covarianceGradientG[k] += coef * dk(k, 0);
      }
    }
    if (diagCoefG[i] != 0.0)
    {
      const Matrix dk(reducedCovarianceModelG_.parameterGradient(uniqueInputSample_[i], uniqueInputSample_[i]));
      for (UnsignedInteger k = 0; k < covarianceParameterSizeG; ++k)
        covarianceGradientG[k] += diagCoefG[i] * dk(k, 0);
    }
  }
  for (UnsignedInteger p = 0; p < U; ++p)
  {
    for (UnsignedInteger q = 0; q < U; ++q)
    {
      const Scalar coef = KuuBarG(p, q);
      if (coef != 0.0)
      {
        const Matrix dk(reducedCovarianceModelG_.parameterGradient(inducingPointsG_[p], inducingPointsG_[q]));
        for (UnsignedInteger k = 0; k < covarianceParameterSizeG; ++k)
          covarianceGradientG[k] += coef * dk(k, 0);
      }
    }
  }
  // Project the covariance adjoints onto the Cholesky factors (log-diagonal)
  Point cholGradientF(M * (M + 1) / 2, 0.0);
  Point cholGradientG(U * (U + 1) / 2, 0.0);
  if (optimizeVariational_)
  {
    const Matrix LBarF(SBarF + SBarF.transpose());
    const Matrix LBarFt(LBarF * cholF);
    UnsignedInteger offset = 0;
    for (UnsignedInteger p = 0; p < M; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
      {
        if (p == q) cholGradientF[offset] = LBarFt(p, q) * cholF(p, q);
        else cholGradientF[offset] = LBarFt(p, q);
        offset += 1;
      }
    const Matrix LBarG(SBarG + SBarG.transpose());
    const Matrix LBarGt(LBarG * cholG);
    offset = 0;
    for (UnsignedInteger p = 0; p < U; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
      {
        if (p == q) cholGradientG[offset] = LBarGt(p, q) * cholG(p, q);
        else cholGradientG[offset] = LBarGt(p, q);
        offset += 1;
      }
  }
  // Assemble the gradient in the optimization parameter layout
  Point gradient(getOptimizationParameterSize(), 0.0);
  UnsignedInteger offset = 0;
  for (UnsignedInteger k = 0; k < covarianceParameterSizeF; ++k)
    gradient[offset + k] = covarianceGradientF[k];
  offset += covarianceParameterSizeF;
  for (UnsignedInteger k = 0; k < covarianceParameterSizeG; ++k)
    gradient[offset + k] = covarianceGradientG[k];
  offset += covarianceParameterSizeG;
  if (optimizeParameters_)
  {
    gradient[offset] = mu0Bar;
    offset += 1;
  }
  if (optimizeVariational_)
  {
    for (UnsignedInteger i = 0; i < M; ++i)
      gradient[offset + i] = mBarF[i];
    offset += M;
    for (UnsignedInteger i = 0; i < M * (M + 1) / 2; ++i)
      gradient[offset + i] = cholGradientF[i];
    offset += M * (M + 1) / 2;
    for (UnsignedInteger i = 0; i < U; ++i)
      gradient[offset + i] = mBarG[i];
    offset += U;
    for (UnsignedInteger i = 0; i < U * (U + 1) / 2; ++i)
      gradient[offset + i] = cholGradientG[i];
  }
  // Restore member state to the values before this gradient call
  mu0_ = savedMu0;
  variationalMeanF_ = savedMeanF;
  variationalCovarianceF_ = savedCovarianceF;
  variationalMeanG_ = savedMeanG;
  variationalCovarianceG_ = savedCovarianceG;
  return gradient;
}

/* Compute the uncollapsed ELBO and store its by-products */
Scalar HeteroscedasticSparseGaussianProcessFitter::computeELBOValue()
{
  const UnsignedInteger N = uniqueInputSample_.getSize();
  const UnsignedInteger M = inducingPointsF_.getSize();
  const UnsignedInteger U = inducingPointsG_.getSize();
  LOGDEBUG(OSS(false) << "Compute the heteroscedastic ELBO for M=" << M << " and U=" << U << " inducing points");
  // Whitened cross-covariances and marginal moments of the mean process
  const TriangularMatrix LuuF(reducedCovarianceModelF_.discretize(inducingPointsF_).computeRegularizedCholesky());
  const Matrix KfuF(reducedCovarianceModelF_.computeCrossCovariance(uniqueInputSample_, inducingPointsF_));
  const Matrix AF(LuuF.solveLinearSystem(KfuF.transpose()).transpose());
  const Point muF(AF * variationalMeanF_);
  // Whitened cross-covariances and marginal moments of the log-variance process
  const TriangularMatrix LuuG(reducedCovarianceModelG_.discretize(inducingPointsG_).computeRegularizedCholesky());
  const Matrix KfuG(reducedCovarianceModelG_.computeCrossCovariance(uniqueInputSample_, inducingPointsG_));
  const Matrix AG(LuuG.solveLinearSystem(KfuG.transpose()).transpose());
  // Per-point terms and KL divergences
  Scalar dataTerm = 0.0;
  for (UnsignedInteger i = 0; i < N; ++i)
  {
    Scalar qf = 0.0;
    for (UnsignedInteger j = 0; j < M; ++j)
      qf += AF(i, j) * AF(i, j);
    const Scalar kfi = reducedCovarianceModelF_.computeAsScalar(uniqueInputSample_[i], uniqueInputSample_[i]);
    Scalar aSa = 0.0;
    for (UnsignedInteger p = 0; p < M; ++p)
    {
      Scalar Sap = 0.0;
      for (UnsignedInteger q = 0; q < M; ++q)
        Sap += variationalCovarianceF_(p, q) * AF(i, q);
      aSa += AF(i, p) * Sap;
    }
    const Scalar muf = muF[i];
    const Scalar vf = kfi - qf + aSa;
    Scalar qg = 0.0;
    for (UnsignedInteger j = 0; j < U; ++j)
      qg += AG(i, j) * AG(i, j);
    const Scalar kgi = reducedCovarianceModelG_.computeAsScalar(uniqueInputSample_[i], uniqueInputSample_[i]);
    Scalar bSb = 0.0;
    for (UnsignedInteger p = 0; p < U; ++p)
    {
      Scalar Sbp = 0.0;
      for (UnsignedInteger q = 0; q < U; ++q)
        Sbp += variationalCovarianceG_(p, q) * AG(i, q);
      bSb += AG(i, p) * Sbp;
    }
    Scalar mug = mu0_;
    for (UnsignedInteger j = 0; j < U; ++j)
      mug += AG(i, j) * variationalMeanG_[j];
    const Scalar vg = kgi - qg + bSb;
    // Exact per-site aggregation through counts, sums and sums of squares
    const Scalar counts = 1.0 * replicateCounts_[i];
    const Scalar energy = replicateSumSquares_[i] - 2.0 * muf * replicateSums_[i]
                          + counts * (muf * muf + vf);
    dataTerm += -0.5 * (counts * (2.0 * SpecFunc::LOGSQRT2PI + mug) + energy * std::exp(-mug + 0.5 * vg));
  }
  const TriangularMatrix cholSf(variationalCovarianceF_.computeRegularizedCholesky());
  Scalar logDetSf = 0.0;
  Scalar traceSf = 0.0;
  for (UnsignedInteger i = 0; i < M; ++i)
  {
    logDetSf += 2.0 * std::log(cholSf(i, i));
    traceSf += variationalCovarianceF_(i, i);
  }
  // KL divergences in the whitened parametrisation with centered priors.
  // The g-process prior mean cancels: with w^c = L_g^{-1} (g_u - mu0 * 1) both
  // q(w^c) and p(w^c) are centered up to (m_g, S_g), as for the f-process.
  const Scalar klF = 0.5 * (traceSf + variationalMeanF_.normSquare() - 1.0 * M - logDetSf);
  const TriangularMatrix cholSg(variationalCovarianceG_.computeRegularizedCholesky());
  Scalar logDetSg = 0.0;
  Scalar traceSg = 0.0;
  for (UnsignedInteger i = 0; i < U; ++i)
  {
    logDetSg += 2.0 * std::log(cholSg(i, i));
    traceSg += variationalCovarianceG_(i, i);
  }
  const Scalar klG = 0.5 * (traceSg + variationalMeanG_.normSquare() - 1.0 * U - logDetSg);
  const Scalar value = dataTerm - klF - klG;

  // Store the by-products of the ELBO evaluation
  whiteningFactorF_ = LuuF;
  whiteningFactorG_ = LuuG;
  lastELBO_ = value;
  LOGDEBUG(OSS(false) << "ELBO=" << value);
  return value;
}

/* Initialize default optimization algorithm */
void HeteroscedasticSparseGaussianProcessFitter::initializeDefaultOptimizationAlgorithm()
{
  const String solverName(ResourceMap::GetAsString("HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::GetByName(solverName);
  if ((solverName == "Cobyla") || (solverName == "TNC"))
    solver_.setCheckStatus(false);
}

/* Reset method */
void HeteroscedasticSparseGaussianProcessFitter::reset()
{
  // Reset elements for new computation
  whiteningFactorF_ = TriangularMatrix();
  whiteningFactorG_ = TriangularMatrix();
  hasRun_ = false;
  lastELBO_ = SpecFunc::LowestScalar;
  result_ = HeteroscedasticSparseGaussianProcessFitterResult();
}

/* Build the vector of optimization parameters */
Point HeteroscedasticSparseGaussianProcessFitter::buildOptimizationParameters() const
{
  Point parameters(reducedCovarianceModelF_.getParameter());
  const Point parametersG(reducedCovarianceModelG_.getParameter());
  for (UnsignedInteger i = 0; i < parametersG.getSize(); ++i)
    parameters.add(parametersG[i]);
  if (optimizeParameters_)
    parameters.add(mu0_);
  if (optimizeVariational_)
  {
    for (UnsignedInteger i = 0; i < variationalMeanF_.getSize(); ++i)
      parameters.add(variationalMeanF_[i]);
    const TriangularMatrix cholF(variationalCovarianceF_.computeRegularizedCholesky());
    const UnsignedInteger M = inducingPointsF_.getSize();
    for (UnsignedInteger p = 0; p < M; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
      {
        if (p == q) parameters.add(std::log(cholF(p, q)));
        else parameters.add(cholF(p, q));
      }
    for (UnsignedInteger i = 0; i < variationalMeanG_.getSize(); ++i)
      parameters.add(variationalMeanG_[i]);
    const TriangularMatrix cholG(variationalCovarianceG_.computeRegularizedCholesky());
    const UnsignedInteger U = inducingPointsG_.getSize();
    for (UnsignedInteger p = 0; p < U; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
      {
        if (p == q) parameters.add(std::log(cholG(p, q)));
        else parameters.add(cholG(p, q));
      }
  }
  return parameters;
}

/* Build the bounds of the optimization parameters */
void HeteroscedasticSparseGaussianProcessFitter::buildOptimizationBounds()
{
  const Scalar lowerBoundScalar = ResourceMap::GetAsScalar("HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationLowerBound");
  if (!(lowerBoundScalar > 0.0))
    throw InvalidArgumentException(HERE) << "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationLowerBound must be positive, got " << lowerBoundScalar;
  const Scalar upperBoundScalar = ResourceMap::GetAsScalar("HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationUpperBound");
  if (!(upperBoundScalar > 0.0))
    throw InvalidArgumentException(HERE) << "HeteroscedasticSparseGaussianProcessFitter-DefaultOptimizationUpperBound must be positive, got " << upperBoundScalar;
  Point lowerBound;
  Point upperBound;
  // Bounds for both covariance models with data-range scaled amplitudes
  const CovarianceModel models[2] = {reducedCovarianceModelF_, reducedCovarianceModelG_};
  for (UnsignedInteger model = 0; model < 2; ++model)
  {
    const UnsignedInteger parameterSize = models[model].getParameter().getSize();
    for (UnsignedInteger k = 0; k < parameterSize; ++k)
    {
      lowerBound.add(lowerBoundScalar);
      upperBound.add(upperBoundScalar);
    }
    const Description activeParametersDescription(models[model].getParameterDescription());
    Indices activeScalesPositions(0);
    Indices activeScalesIndices(0);
    for (UnsignedInteger k = 0; k < parameterSize; ++k)
    {
      const String parameterName(activeParametersDescription[k]);
      if (parameterName.find("scale_") != String::npos)
      {
        activeScalesPositions.add(k);
        activeScalesIndices.add(std::stoi(parameterName.substr(parameterName.find("_") + 1, parameterName.size())));
      }
    }
    if (activeScalesPositions.getSize() > 0)
    {
      const Scalar lowerBoundScaleFactor = ResourceMap::GetAsScalar("HeteroscedasticSparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor");
      if (!(lowerBoundScaleFactor > 0.0))
        throw InvalidArgumentException(HERE) << "HeteroscedasticSparseGaussianProcessFitter-OptimizationLowerBoundScaleFactor must be positive, got " << lowerBoundScaleFactor;
      const Scalar upperBoundScaleFactor = ResourceMap::GetAsScalar("HeteroscedasticSparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor");
      if (!(upperBoundScaleFactor > 0.0))
        throw InvalidArgumentException(HERE) << "HeteroscedasticSparseGaussianProcessFitter-OptimizationUpperBoundScaleFactor must be positive, got " << upperBoundScaleFactor;
      const Point inputSampleRange(inputSample_.computeRange());
      const UnsignedInteger base = lowerBound.getSize() - parameterSize;
      for (UnsignedInteger k = 0; k < activeScalesPositions.getSize(); ++k)
      {
        const Scalar rangeK = inputSampleRange[activeScalesIndices[k]];
        lowerBound[base + activeScalesPositions[k]] = rangeK * lowerBoundScaleFactor;
        upperBound[base + activeScalesPositions[k]] = rangeK * upperBoundScaleFactor;
      }
    }
  }
  // Bounds for the prior mean, reused from the log-variance function range
  if (optimizeParameters_)
  {
    const Scalar mu0LowerBound = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultVarianceFunctionLowerBound");
    const Scalar mu0UpperBound = ResourceMap::GetAsScalar("SparseGaussianProcessFitter-DefaultVarianceFunctionUpperBound");
    if (!(mu0LowerBound < mu0UpperBound))
      throw InvalidArgumentException(HERE) << "SparseGaussianProcessFitter-DefaultVarianceFunctionLowerBound (" << mu0LowerBound << ") must be strictly lower than SparseGaussianProcessFitter-DefaultVarianceFunctionUpperBound (" << mu0UpperBound << ")";
    lowerBound.add(mu0LowerBound);
    upperBound.add(mu0UpperBound);
  }
  // Bounds for the variational parameters, scaled by the output range for the means
  if (optimizeVariational_)
  {
    const Scalar variationalFactor = ResourceMap::GetAsScalar("HeteroscedasticSparseGaussianProcessFitter-VariationalBoundFactor");
    if (!(variationalFactor > 0.0))
      throw InvalidArgumentException(HERE) << "HeteroscedasticSparseGaussianProcessFitter-VariationalBoundFactor must be positive, got " << variationalFactor;
    const Scalar outputRange = outputSample_.computeRange()[0];
    const Scalar meanBound = variationalFactor * std::max(1.0, outputRange);
    const UnsignedInteger M = inducingPointsF_.getSize();
    for (UnsignedInteger i = 0; i < M; ++i)
    {
      lowerBound.add(-meanBound);
      upperBound.add(meanBound);
    }
    for (UnsignedInteger i = 0; i < M * (M + 1) / 2; ++i)
    {
      lowerBound.add(-variationalFactor);
      upperBound.add(variationalFactor);
    }
    const UnsignedInteger U = inducingPointsG_.getSize();
    for (UnsignedInteger i = 0; i < U; ++i)
    {
      lowerBound.add(-meanBound);
      upperBound.add(meanBound);
    }
    for (UnsignedInteger i = 0; i < U * (U + 1) / 2; ++i)
    {
      lowerBound.add(-variationalFactor);
      upperBound.add(variationalFactor);
    }
  }
  optimizationBounds_ = Interval(lowerBound, upperBound);
}

/* Build the optimization parameter description */
Description HeteroscedasticSparseGaussianProcessFitter::buildOptimizationParameterDescription() const
{
  Description description(reducedCovarianceModelF_.getParameterDescription());
  const Description descriptionG(reducedCovarianceModelG_.getParameterDescription());
  for (UnsignedInteger i = 0; i < descriptionG.getSize(); ++i)
    description.add(descriptionG[i]);
  if (optimizeParameters_)
    description.add("mu0");
  if (optimizeVariational_)
  {
    const UnsignedInteger M = inducingPointsF_.getSize();
    for (UnsignedInteger i = 0; i < M; ++i)
      description.add(OSS() << "mf_" << i);
    for (UnsignedInteger p = 0; p < M; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
        description.add(OSS() << "Lf_" << p << "_" << q);
    const UnsignedInteger U = inducingPointsG_.getSize();
    for (UnsignedInteger i = 0; i < U; ++i)
      description.add(OSS() << "mg_" << i);
    for (UnsignedInteger p = 0; p < U; ++p)
      for (UnsignedInteger q = 0; q <= p; ++q)
        description.add(OSS() << "Lg_" << p << "_" << q);
  }
  return description;
}

/* Size of the optimization parameter vector */
UnsignedInteger HeteroscedasticSparseGaussianProcessFitter::getOptimizationParameterSize() const
{
  const UnsignedInteger M = inducingPointsF_.getSize();
  const UnsignedInteger U = inducingPointsG_.getSize();
  return reducedCovarianceModelF_.getParameter().getSize()
         + reducedCovarianceModelG_.getParameter().getSize()
         + (optimizeParameters_ ? 1 : 0)
         + (optimizeVariational_ ? M + M * (M + 1) / 2 + U + U * (U + 1) / 2 : 0);
}

/* Method save() stores the object through the StorageManager */
void HeteroscedasticSparseGaussianProcessFitter::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute("covarianceModelF_", covarianceModelF_);
  adv.saveAttribute("covarianceModelG_", covarianceModelG_);
  adv.saveAttribute("reducedCovarianceModelF_", reducedCovarianceModelF_);
  adv.saveAttribute("reducedCovarianceModelG_", reducedCovarianceModelG_);
  adv.saveAttribute("inducingPointsF_", inducingPointsF_);
  adv.saveAttribute("inducingPointsG_", inducingPointsG_);
  adv.saveAttribute("mu0_", mu0_);
  adv.saveAttribute("variationalMeanF_", variationalMeanF_);
  adv.saveAttribute("variationalCovarianceF_", variationalCovarianceF_);
  adv.saveAttribute("variationalMeanG_", variationalMeanG_);
  adv.saveAttribute("variationalCovarianceG_", variationalCovarianceG_);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("optimizationBounds_", optimizationBounds_);
  adv.saveAttribute("optimizeParameters_", optimizeParameters_);
  adv.saveAttribute("optimizeVariational_", optimizeVariational_);
  adv.saveAttribute("hasRun_", hasRun_);
  adv.saveAttribute("lastELBO_", lastELBO_);
  adv.saveAttribute("whiteningFactorF_", whiteningFactorF_);
  adv.saveAttribute("whiteningFactorG_", whiteningFactorG_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void HeteroscedasticSparseGaussianProcessFitter::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute("covarianceModelF_", covarianceModelF_);
  adv.loadAttribute("reducedCovarianceModelF_", reducedCovarianceModelF_);
  adv.loadAttribute("covarianceModelG_", covarianceModelG_);
  adv.loadAttribute("reducedCovarianceModelG_", reducedCovarianceModelG_);
  adv.loadAttribute("inducingPointsF_", inducingPointsF_);
  adv.loadAttribute("inducingPointsG_", inducingPointsG_);
  adv.loadAttribute("mu0_", mu0_);
  adv.loadAttribute("variationalMeanF_", variationalMeanF_);
  adv.loadAttribute("variationalCovarianceF_", variationalCovarianceF_);
  adv.loadAttribute("variationalMeanG_", variationalMeanG_);
  adv.loadAttribute("variationalCovarianceG_", variationalCovarianceG_);
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("optimizationBounds_", optimizationBounds_);
  adv.loadAttribute("optimizeParameters_", optimizeParameters_);
  adv.loadAttribute("optimizeVariational_", optimizeVariational_);
  adv.loadAttribute("hasRun_", hasRun_);
  adv.loadAttribute("lastELBO_", lastELBO_);
  adv.loadAttribute("whiteningFactorF_", whiteningFactorF_);
  adv.loadAttribute("whiteningFactorG_", whiteningFactorG_);
  adv.loadAttribute("result_", result_);
  // Rebuild the derived replicate structure from the restored samples
  buildReplicateStructure();
}

END_NAMESPACE_OPENTURNS
