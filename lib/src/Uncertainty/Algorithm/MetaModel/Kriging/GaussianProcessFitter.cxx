//                                               -*- C++ -*-
/**
 *  @brief The class fits gaussian process models
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

#include "openturns/GaussianProcessFitter.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/ConstantFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/MemoizeFunction.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessFitter)

static const Factory<GaussianProcessFitter> Factory_GaussianProcessFitter;

/* Default constructor */
GaussianProcessFitter::GaussianProcessFitter()
  : MetaModelAlgorithm()
  , covarianceModel_()
  , reducedCovarianceModel_()
  , solver_()
  , optimizationBounds_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  // Nothing to do
}

GaussianProcessFitter::GaussianProcessFitter(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Basis & basis)
  : MetaModelAlgorithm(inputSample, outputSample)
  , covarianceModel_()
  , reducedCovarianceModel_()
  , solver_()
  , optimizationBounds_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
{
  // Set covariance model
  setCovarianceModel(covarianceModel);

  if (basis.getSize() > 0)
    setBasis(basis);

  initializeMethod();
  initializeDefaultOptimizationAlgorithm();
}

/* Covariance model accessors */
void GaussianProcessFitter::setCovarianceModel(const CovarianceModel & covarianceModel)
{
  // Here we can store any modified version of the given covariance model wrt its parameters as it is mainly a parametric template
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  const UnsignedInteger outputDimension = outputSample_.getDimension();

  if (covarianceModel.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Covariance model input dimension is " << covarianceModel.getInputDimension() << ", expected " << inputDimension;
  if (covarianceModel.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Covariance model output dimension is " << covarianceModel.getOutputDimension() << ", expected " << outputDimension;
  covarianceModel_ = covarianceModel;
  // All the computation will be done on the reduced covariance model. We keep the initial covariance model (ie the one we just built) in order to reinitialize the reduced covariance model if some flags are changed after the creation of the algorithm.
  reducedCovarianceModel_ = covarianceModel_;
  // Now, adapt the model parameters.
  // First, check if the parameters have to be optimized. If not, remove all the active parameters.
  analyticalAmplitude_ = false;
  const Description activeParametersDescription(reducedCovarianceModel_.getParameterDescription());
  if (!optimizeParameters_) reducedCovarianceModel_.setActiveParameter(Indices());
  // Second, check if the amplitude parameter is unique and active
  else if (ResourceMap::GetAsBool("GaussianProcessFitter-UseAnalyticalAmplitudeEstimate"))
  {
    // The model has to be of dimension 1
    if (reducedCovarianceModel_.getOutputDimension() == 1)
    {
      // And one of the active parameters must be called amplitude_0
      for (UnsignedInteger i = 0; i < activeParametersDescription.getSize(); ++i)
        if (activeParametersDescription[i] == "amplitude_0")
        {
          analyticalAmplitude_ = true;
          Indices newActiveParameters(reducedCovarianceModel_.getActiveParameter());
          newActiveParameters.erase(newActiveParameters.begin() + i);
          reducedCovarianceModel_.setActiveParameter(newActiveParameters);
          // Here we have to change the current value of the amplitude as it has
          // to be equal to 1 during the potential optimization step in order for
          // the analytical formula to be correct.
          // Now, the amplitude has disappear form the active parameters so it must
          // be updated using the amplitude accessor.
          reducedCovarianceModel_.setAmplitude(Point(1, 1.0));
          break;
        }
    } // reducedCovarianceModel_.getDimension() == 1
  } // optimizeParameters_
  LOGDEBUG(OSS() << "final active parameters=" << reducedCovarianceModel_.getActiveParameter());
  // Define the bounds of the optimization problem
  const UnsignedInteger optimizationDimension = reducedCovarianceModel_.getParameter().getSize();
  if (optimizationDimension > 0)
  {
    const Scalar lowerBoundScaleFactor = ResourceMap::GetAsScalar("GaussianProcessFitter-OptimizationLowerBoundScaleFactor");
    if (!(lowerBoundScaleFactor > 0.0))
      throw InvalidArgumentException(HERE) << "GPR lower bound scale factor set in ResourceMap should be positive, got " << lowerBoundScaleFactor;
    const Scalar upperBoundScaleFactor = ResourceMap::GetAsScalar("GaussianProcessFitter-OptimizationUpperBoundScaleFactor");
    if (!(upperBoundScaleFactor > 0.0))
      throw InvalidArgumentException(HERE) << "GPR upper bound scale factor set in ResourceMap should be positive, got " << upperBoundScaleFactor;
    Point lowerBound(optimizationDimension, ResourceMap::GetAsScalar("GaussianProcessFitter-DefaultOptimizationLowerBound"));
    Point upperBound(optimizationDimension, ResourceMap::GetAsScalar("GaussianProcessFitter-DefaultOptimizationUpperBound"));
    // We could set scale parameter if these parameters are enabled.
    // check if some scales are active
    // check if nugget factor is active
    Indices activeScalesPositions(0);
    Indices activeScalesIndices(0);
    Indices activeNugget(0);
    for (UnsignedInteger k = 0; k < optimizationDimension; ++k)
    {
      const String parameterName(activeParametersDescription[k]);
      if (parameterName.find("scale_") != String::npos)
      {
        activeScalesPositions.add(k);
        // Extract the scale index from its description
        activeScalesIndices.add(std::stoi(parameterName.substr(parameterName.find("_") + 1, parameterName.size())));
      }
      if (activeParametersDescription[k].find("nuggetFactor") != String::npos) activeNugget.add(k);
    }

    if (activeScalesPositions.getSize() > 0)
    {
      const Point inputSampleRange(inputSample_.computeRange());
      for (UnsignedInteger k = 0; k < activeScalesPositions.getSize(); ++k)
      {
        const Scalar rangeK = inputSampleRange[activeScalesIndices[k]];
        lowerBound[k] = rangeK * lowerBoundScaleFactor;
        upperBound[k] = rangeK * upperBoundScaleFactor;
      } // k (upper bounds setting)
    } // if active scale
    if (activeNugget.getSize() > 0)
      // Set the lower bound to 0 for nuggetFactor
      lowerBound[activeNugget[0]] = ResourceMap::GetAsScalar("GaussianProcessFitter-DefaultOptimizationNuggetLowerBound");
    LOGINFO(OSS() <<  "For coherency we set scale upper bounds = " << upperBound.__str__());

    optimizationBounds_ = Interval(lowerBound, upperBound);
  }
  else optimizationBounds_ = Interval();
}

CovarianceModel GaussianProcessFitter::getCovarianceModel() const
{
  return covarianceModel_;
}

CovarianceModel GaussianProcessFitter::getReducedCovarianceModel() const
{
  return reducedCovarianceModel_;
}

/* Set basis method */
void GaussianProcessFitter::setBasis(const Basis & basis)
{
  // Basis does not provide any getOutputDimension
  // getDimension checks also only the dimension of the first element in case of FiniteBasis
  // If basis given, then its size should be the same as the output dimension (each item of the basis is a function with the same input/output dimensions).
  if (!basis.isFinite())
    throw InvalidArgumentException(HERE) << "In GaussianProcessFitter::GaussianProcessFitter, basis should be finite!" ;
  const UnsignedInteger size = basis.getSize();
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    if (basis[index].getOutputDimension() != outputSample_.getDimension())
      throw InvalidArgumentException(HERE) << "In GaussianProcessFitter::GaussianProcessFitter, output sample dimension=" << outputSample_.getDimension() << " does not match basis[=" << index  << "] dimension=" << basis[index].getOutputDimension();
    if (basis[index].getInputDimension() != inputSample_.getDimension())
      throw InvalidArgumentException(HERE) << "In GaussianProcessFitter::GaussianProcessFitter, input sample dimension=" << inputSample_.getDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getInputDimension();
  }
  // Everything is ok, we set the basis
  basis_ = basis;
}

void GaussianProcessFitter::initializeDefaultOptimizationAlgorithm()
{
  const String solverName(ResourceMap::GetAsString("GaussianProcessFitter-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::GetByName(solverName);
  if ((solverName == "Cobyla") || (solverName == "TNC"))
    solver_.setCheckStatus(false);
}

/* Virtual constructor */
GaussianProcessFitter * GaussianProcessFitter::clone() const
{
  return new GaussianProcessFitter(*this);
}

/* Compute the design matrix */
void GaussianProcessFitter::computeF()
{
  // Nothing to do if the design matrix has already been computed
  if (F_.getNbRows() != 0) return;
  LOGDEBUG("Compute the design matrix");
  // No early exit based on the sample/basis size as F_ must be initialized with the correct dimensions
  // With a multivariate basis of size similar to output dimension, each ith-basis should be applied to elements
  // of corresponding marginal
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const UnsignedInteger sampleSize = inputSample_.getSize();
  const UnsignedInteger basisSize = basis_.getSize();
  // Basis \Phi is a function from R^{inputDimension} to R^{outputDimension}
  // As we get B functions, total number of values is B * outputDimension
  const UnsignedInteger totalSize = outputDimension * basisSize;

  F_ = Matrix(sampleSize * outputDimension, totalSize);
  if (totalSize == 0) return;

  // Compute F
  for (UnsignedInteger j = 0; j < basisSize; ++j)
  {
    // Compute phi_j (X)
    // Here we use potential parallelism in the evaluation of the basis functions
    // It generates a sample of shape (sampleSize, outputDimension)
    const Sample basisSample(basis_[j](inputSample_));
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
      for (UnsignedInteger outputMarginal = 0; outputMarginal < outputDimension; ++outputMarginal)
        F_(outputMarginal + i * outputDimension, j * outputDimension + outputMarginal) = basisSample(i, outputMarginal);
  }
}

/* Perform regression
1) Compute the design matrix
2) Call the parameters optimization
  a) Compute the log-likelihood with the initial parameters. It is mandatory
     even if no parameter has to be optimized as this computation has many side
     effects such as:
     * computing the trend coefficients beta
     * computing the discretized covariance matrix Cholesky factor
  b) If the amplitude can be computed analytically from the other parameters:
     * set its value to 1
     * remove it from the list of parameters
  c) If some parameters remain, perform the optimization
  d) Deduce the associated value of the amplitude by the analytical formula if possible
3) Build the result:
  a) Extract the different parts of the trend
  b) Update the covariance model if needed
 */

Bool GaussianProcessFitter::getKeepCholeskyFactor() const
{
  return keepCholeskyFactor_;
}

void GaussianProcessFitter::setKeepCholeskyFactor(const Bool keepCholeskyFactor)
{
  keepCholeskyFactor_ = keepCholeskyFactor;
  reset();
}

void GaussianProcessFitter::run()
{
  // Do not run again if already computed
  if (hasRun_) return;
  computeF();
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  // optimization of likelihood function if provided
  // Here we call the optimizeReducedLogLikelihood() method even if the covariance
  // model has no active parameter, because:
  // + it can be due to the fact that the amplitude is obtained through an
  //   analytical formula and this situation is taken into account in
  //   maximizeReducedLogLikelihood()
  // + even if there is actually no parameter to optimize,
  //   maximizeReducedLogLikelihood() is the entry point to
  //   computeReducedLogLikelihood() which has side effects on covariance
  //   discretization and factorization, and it computes beta_
  Scalar optimalLogLikelihood = maximizeReducedLogLikelihood();

  LOGDEBUG("Store the estimates");
  LOGDEBUG("Build the output meta-model");
  Collection<Function> marginalCollections(basis_.getSize());
  Collection<Function> marginalFunctions(outputDimension);
  Point beta_k(basis_.getSize());
  Function metaModel;

  if (basis_.getSize() > 0)
  {
    for (UnsignedInteger outputMarginal = 0; outputMarginal < outputDimension; ++outputMarginal)
    {
      for (UnsignedInteger k = 0; k < basis_.getSize(); ++k)
      {
        marginalCollections[k] = basis_[k].getMarginal(outputMarginal);
        beta_k[k] = beta_[k * outputDimension + outputMarginal];
      }
      LinearCombinationFunction marginalFunction(marginalCollections, beta_k);
      marginalFunctions[outputMarginal] = marginalFunction;
    }

    // Care ! collection should be non empty
    metaModel = AggregatedFunction(marginalFunctions);
  }
  else
  {
    // If no basis ==> zero function
    metaModel = ConstantFunction(covarianceModel_.getInputDimension(), Point(covarianceModel_.getOutputDimension(), 0.0));
  }

  // return optimized covmodel with the original active parameters (see analyticalAmplitude_)
  CovarianceModel reducedCovarianceModelCopy(reducedCovarianceModel_);
  reducedCovarianceModelCopy.setActiveParameter(covarianceModel_.getActiveParameter());

  result_ = GaussianProcessFitterResult(inputSample_, outputSample_, metaModel, F_, basis_, beta_, reducedCovarianceModelCopy, optimalLogLikelihood, method_);
  result_.setStandardizedOutput(rho_);

  // The scaling is done there because it has to be done as soon as some optimization has been done, either numerically or through an analytical formula
  if (keepCholeskyFactor_)
  {
    if (analyticalAmplitude_)
    {
      const Scalar sigma = reducedCovarianceModel_.getAmplitude()[0];
      // Case of LAPACK backend
      if (method_ == GaussianProcessFitterResult::LAPACK) covarianceCholeskyFactor_ = covarianceCholeskyFactor_ * sigma;
      else covarianceCholeskyFactorHMatrix_.scale(sigma);
    }
    result_.setCholeskyFactor(covarianceCholeskyFactor_, covarianceCholeskyFactorHMatrix_);
  }
  hasRun_ = true;
}

// Maximize the log-likelihood of the Gaussian process model wrt the observations
// If the covariance model has no active parameter, no numerical optimization
// is done. There are two cases:
// + no parameter has to be optimized, in which case a single call to
//   computeReducedLogLikelihood() is made in order to compute beta_ and to
//   factor the covariance matrix
// + the amplitude is the only covariance parameter to be estimated and it is
//   done thanks to an analytical formula
// The method returns the optimal log-likelihood (which is equal to the optimal
// reduced log-likelihood), the corresponding parameters being directly stored
// into the covariance model
Scalar GaussianProcessFitter::maximizeReducedLogLikelihood()
{
  // initial guess
  Point initialParameters(reducedCovarianceModel_.getParameter());
  // We use the functional form of the log-likelihood computation to benefit from the cache mechanism
  Function reducedLogLikelihoodFunction(getReducedLogLikelihoodFunction());
  const Bool noNumericalOptimization = initialParameters.getSize() == 0 || !getOptimizeParameters();
  // Early exit if the parameters are known
  if (noNumericalOptimization)
  {
    // We only need to compute the log-likelihood function at the initial parameters in order to get the Cholesky factor and the trend coefficients
    const Scalar initialReducedLogLikelihood = reducedLogLikelihoodFunction(initialParameters)[0];
    LOGDEBUG("No covariance parameter to optimize");
    LOGDEBUG(OSS() << "initial parameters=" << initialParameters << ", log-likelihood=" << initialReducedLogLikelihood);
    return initialReducedLogLikelihood;
  }
  // Thus here we perform an optimization. First let us check the initial point is inside the
  // optimization bounds search, otherwise define one arbitrary inside these bounds
  if (!optimizationBounds_.contains(initialParameters))
  {
    // Define starting point for the optimization as the center of the bounds
    // We should ensure somehow that the upper/lower bounds scale are nearly the same
    initialParameters = (optimizationBounds_.getUpperBound() + optimizationBounds_.getLowerBound())/2;
  }

  // internal normalization into (0,1)^n
  Interval bounds(optimizationBounds_);
  const Bool normalization = ResourceMap::GetAsBool("GaussianProcessFitter-OptimizationNormalization");
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
    reducedLogLikelihoodFunction = ComposedFunction(reducedLogLikelihoodFunction, uToX);
    bounds = Interval(parameterDimension);
  }

  // At this point we have an optimization problem to solve
  // Define the optimization problem
  OptimizationProblem problem(reducedLogLikelihoodFunction);
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
  const Point optimalLogLikelihoodPoint = result.getOptimalValue();
  if (!optimalLogLikelihoodPoint.getSize())
    throw InvalidArgumentException(HERE) << "optimization in GaussianProcessFitter did not yield feasible points";
  const Scalar optimalLogLikelihood = optimalLogLikelihoodPoint[0];
  Point optimalParameters(result.getOptimalPoint());
  if (normalization)
    optimalParameters = uToX(optimalParameters);

  const UnsignedInteger evaluationNumber = result.getCallsNumber();
  // Check if the optimal value corresponds to the last computed value, in order to
  // see if the by-products (Cholesky factor etc) are correct
  if (lastReducedLogLikelihood_ != optimalLogLikelihood)
  {
    LOGDEBUG(OSS(false) << "Need to evaluate the objective function one more time because the last computed reduced log-likelihood value=" << lastReducedLogLikelihood_ << " is different from the optimal one=" << optimalLogLikelihood);
    (void) computeReducedLogLikelihood(optimalParameters);
  }
  // Final call to reducedLogLikelihoodFunction() in order to update the amplitude
  // No additional cost since the cache mechanism is activated
  LOGDEBUG(OSS() << evaluationNumber << " evaluations, optimized parameters=" << optimalParameters << ", log-likelihood=" << optimalLogLikelihood);

  return optimalLogLikelihood;
}

Point GaussianProcessFitter::computeReducedLogLikelihood(const Point & parameters)
{
  // Check that the parameters have a size compatible with the covariance model
  if (parameters.getSize() != reducedCovarianceModel_.getParameter().getSize())
    throw InvalidArgumentException(HERE) << "In GaussianProcessFitter::computeReducedLogLikelihood, could not compute likelihood,"
                                         << " covariance model requires an argument of size " << reducedCovarianceModel_.getParameter().getSize()
                                         << " but here we got " << parameters.getSize();
  LOGDEBUG(OSS(false) << "Compute reduced log-likelihood for parameters=" << parameters);
  const Scalar constant = - SpecFunc::LOGSQRT2PI * static_cast<Scalar>(inputSample_.getSize()) * static_cast<Scalar>(outputSample_.getDimension());
  Scalar logDeterminant = 0.0;
  // If the amplitude is deduced from the other parameters, work with
  // the correlation function
  if (analyticalAmplitude_) reducedCovarianceModel_.setAmplitude(Point(1, 1.0));
  reducedCovarianceModel_.setParameter(parameters);
  // First, compute the log-determinant of the Cholesky factor of the covariance
  // matrix. As a by-product, also compute rho.
  if (method_ == GaussianProcessFitterResult::LAPACK)
    logDeterminant = computeLapackLogDeterminantCholesky();
  else
    logDeterminant = computeHMatLogDeterminantCholesky();
  // Compute the amplitude using an analytical formula if needed
  // and update the reduced log-likelihood.
  if (analyticalAmplitude_)
  {
    LOGDEBUG("Analytical amplitude");
    // J(\sigma)=-\log(\sqrt{\sigma^{2N}\det{R}})-(Y-M)^tR^{-1}(Y-M)/(2\sigma^2)
    //          =-N\log(\sigma)-\log(\det{R})/2-(Y-M)^tR^{-1}(Y-M)/(2\sigma^2)
    // dJ/d\sigma=-N/\sigma+(Y-M)^tR^{-1}(Y-M)/\sigma^3=0
    // \sigma=\sqrt{(Y-M)^tR^{-1}(Y-M)/N}
    const UnsignedInteger size = inputSample_.getSize();
    const Scalar sigma = std::sqrt(rho_.normSquare() / (ResourceMap::GetAsBool("GaussianProcessFitter-UnbiasedVariance") ? size - beta_.getSize() : size));
    LOGDEBUG(OSS(false) << "sigma=" << sigma);
    reducedCovarianceModel_.setAmplitude(Point(1, sigma));
    logDeterminant += 2.0 * size * std::log(sigma);
    rho_ /= sigma;
    LOGDEBUG(OSS(false) << "rho_=" << rho_);
  } // analyticalAmplitude

  LOGDEBUG(OSS(false) << "log-determinant=" << logDeterminant << ", rho=" << rho_);
  const Scalar epsilon = rho_.normSquare();
  LOGDEBUG(OSS(false) << "epsilon=||rho||^2=" << epsilon);
  if (epsilon <= 0) lastReducedLogLikelihood_ = SpecFunc::LowestScalar;
  // For the general multidimensional case, we have to compute the general log-likelihood (ie including marginal variances)
  else lastReducedLogLikelihood_ = constant - 0.5 * (logDeterminant + epsilon);
  LOGDEBUG(OSS(false) << "Point " << parameters << " -> reduced log-likelihood=" << lastReducedLogLikelihood_);
  return Point(1, lastReducedLogLikelihood_);
}


Scalar GaussianProcessFitter::computeLapackLogDeterminantCholesky()
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = diag(sigma) * R(s,t) * diag(sigma) with R a correlation function
  LOGDEBUG(OSS(false) << "Compute the LAPACK log-determinant of the Cholesky factor for covariance=" << reducedCovarianceModel_);

  LOGDEBUG("Discretize the covariance model");
  CovarianceMatrix C(reducedCovarianceModel_.discretize(inputSample_));
  if (C.getDimension() < 20)
    LOGDEBUG(OSS(false) << "C=\n" << C);
  LOGDEBUG("Compute the Cholesky factor of the covariance matrix");
  covarianceCholeskyFactor_ = C.computeRegularizedCholesky();

  // y corresponds to output data
  const Point y(outputSample_.getImplementation()->getData());
  LOGDEBUG(OSS(false) << "y=" << y);
  // rho = L^{-1}y
  LOGDEBUG("Solve L.rho = y");
  rho_ = covarianceCholeskyFactor_.solveLinearSystem(y);
  LOGDEBUG(OSS(false) << "rho_=L^{-1}y=" << rho_);
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = L^{-1}F
    LOGDEBUG("Solve L.Phi = F");
    LOGDEBUG(OSS(false) << "F_=\n" << F_);
    Matrix Phi(covarianceCholeskyFactor_.solveLinearSystem(F_));
    LOGDEBUG(OSS(false) << "Phi=\n" << Phi);
    LOGDEBUG("Solve min_beta||Phi.beta - rho||^2");
    beta_ = Phi.solveLinearSystem(rho_);
    LOGDEBUG(OSS(false) << "beta_=" << beta_);
    LOGDEBUG("Update rho");
    rho_ -= Phi * beta_;
    LOGDEBUG(OSS(false) << "rho_=L^{-1}y-L^{-1}F.beta=" << rho_);
  }
  LOGDEBUG("Compute log(|det(L)|)=log(sqrt(|det(C)|))");
  Scalar logDetL = 0.0;
  for (UnsignedInteger i = 0; i < covarianceCholeskyFactor_.getDimension(); ++i )
  {
    const Scalar lii = covarianceCholeskyFactor_(i, i);
    if (lii <= 0.0) return SpecFunc::LowestScalar;
    logDetL += log(lii);
  }
  LOGDEBUG(OSS(false) << "logDetL=" << logDetL);
  return 2.0 * logDetL;
}

Scalar GaussianProcessFitter::computeHMatLogDeterminantCholesky()
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = \sigma^2 * R(s,t) with R a correlation function
  LOGDEBUG(OSS(false) << "Compute the HMAT log-determinant of the Cholesky factor for covariance=" << reducedCovarianceModel_);

  const UnsignedInteger covarianceDimension = reducedCovarianceModel_.getOutputDimension();

  HMatrixFactory hmatrixFactory;
  HMatrixParameters hmatrixParameters;

  covarianceCholeskyFactorHMatrix_ = hmatrixFactory.build(inputSample_, covarianceDimension, true, hmatrixParameters);
  if (covarianceDimension == 1)
  {
    CovarianceAssemblyFunction simple(reducedCovarianceModel_, inputSample_);
    covarianceCholeskyFactorHMatrix_.assemble(simple, 'L');
  }
  else
  {
    CovarianceBlockAssemblyFunction block(reducedCovarianceModel_, inputSample_);
    covarianceCholeskyFactorHMatrix_.assemble(block, 'L');
  }
  // Factorize
  covarianceCholeskyFactorHMatrix_.factorize(hmatrixParameters.getFactorizationMethod());
  // y corresponds to output data
  // The PersistentCollection is returned as Point with the right memory map
  const Point y(outputSample_.getImplementation()->getData());
  // rho = L^{-1}y
  LOGDEBUG("Solve L.rho = y");
  rho_ = covarianceCholeskyFactorHMatrix_.solveLower(y);
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = L^{-1}F
    LOGDEBUG("Solve L.Phi = F");
    const Matrix Phi(covarianceCholeskyFactorHMatrix_.solveLower(F_));
    LOGDEBUG("Solve min_beta||Phi.beta - rho||^2");
    beta_ = Phi.solveLinearSystem(rho_);
    rho_ -= Phi * beta_;
  }
  LOGDEBUG("Compute log(sqrt(|det(C)|)) = log(|det(L)|)");
  Scalar logDetL = 0.0;
  Point diagonal(covarianceCholeskyFactorHMatrix_.getDiagonal());
  for (UnsignedInteger i = 0; i < rho_.getSize(); ++i )
  {
    const Scalar lii = diagonal[i];
    if (lii <= 0.0) return SpecFunc::LowestScalar;
    logDetL += log(lii);
  }
  return 2.0 * logDetL;
}

/* Optimization solver accessor */
OptimizationAlgorithm GaussianProcessFitter::getOptimizationAlgorithm() const
{
  return solver_;
}

void GaussianProcessFitter::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
  reset();
}

/* Optimize parameters flag accessor */
Bool GaussianProcessFitter::getOptimizeParameters() const
{
  return optimizeParameters_;
}

void GaussianProcessFitter::setOptimizeParameters(const Bool optimizeParameters)
{
  if (optimizeParameters != optimizeParameters_)
  {
    optimizeParameters_ = optimizeParameters;
    // Here we have to call setCovarianceModel() as it computes reducedCovarianceModel from covarianceModel_ in a way influenced by optimizeParameters_ flag.
    setCovarianceModel(covarianceModel_);
  }
}

/* Accessor to optimization bounds */
void GaussianProcessFitter::setOptimizationBounds(const Interval & optimizationBounds)
{
  if (!(optimizationBounds.getDimension() == optimizationBounds_.getDimension())) throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << optimizationBounds_.getDimension() << ", got dimension=" << optimizationBounds.getDimension();
  optimizationBounds_ = optimizationBounds;
  reset();
}

Interval GaussianProcessFitter::getOptimizationBounds() const
{
  return optimizationBounds_;
}

/* String converter */
String GaussianProcessFitter::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << ", inputSample=" << inputSample_
      << ", outputSample=" << outputSample_
      << ", basis=" << basis_
      << ", covarianceModel=" << covarianceModel_
      << ", reducedCovarianceModel=" << reducedCovarianceModel_
      << ", solver=" << solver_
      << ", optimizeParameters=" << optimizeParameters_;
  return oss;
}


GaussianProcessFitterResult GaussianProcessFitter::getResult()
{
  if (!hasRun_) run();
  return result_;
}


Function GaussianProcessFitter::getReducedLogLikelihoodFunction()
{
  computeF();
  MemoizeFunction logLikelihood(ReducedLogLikelihoodEvaluation(*this));
  // Here we change the finite difference gradient for a non centered one in order to reduce the computational cost
  const Scalar finiteDifferenceEpsilon = ResourceMap::GetAsScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon" );
  logLikelihood.setGradient(NonCenteredFiniteDifferenceGradient(finiteDifferenceEpsilon, logLikelihood.getEvaluation()).clone());
  logLikelihood.enableCache();
  return logLikelihood;
}

void GaussianProcessFitter::initializeMethod()
{
  if (ResourceMap::GetAsString("GaussianProcessFitter-LinearAlgebra") == "HMAT")
    setMethod(GaussianProcessFitterResult::HMAT);
}

GaussianProcessFitter::LinearAlgebra GaussianProcessFitter::getMethod() const
{
  return method_;
}

void GaussianProcessFitter::reset()
{
  // Reset elements for new computation
  // No need to update F_ as computeF /setBasis are private
  // Same remark for setCovarianceModel & setData
  covarianceCholeskyFactor_ = TriangularMatrix();
  covarianceCholeskyFactorHMatrix_ = HMatrix();
  hasRun_ = false;
  lastReducedLogLikelihood_ = SpecFunc::LowestScalar;
  beta_ = Point();
  rho_ = Point();
  // The current output Gram matrix
  F_ = Matrix();
}

/* Method accessor (lapack/hmat) - Protected but friend with GaussianProcessFitter class */
void GaussianProcessFitter::setMethod(const LinearAlgebra method)
{
  // First update only if method has changed. It avoids useless reset
  if (method != method_)
  {
    if (method > 1)
      throw InvalidArgumentException(HERE) << "Expecting 0 (LAPACK) or 1 (HMAT)";
    // Set new method
    method_ = method;
    // reset for new computation
    reset();
  }
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessFitter::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "reducedCovarianceModel_", reducedCovarianceModel_ );
  adv.saveAttribute( "solver_", solver_ );
  adv.saveAttribute( "optimizationBounds_", optimizationBounds_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "rho_", rho_ );
  adv.saveAttribute( "F_", F_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.saveAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  UnsignedInteger method  = static_cast<LinearAlgebra>(method_);
  adv.saveAttribute( "method_", method );
  adv.saveAttribute( "hasRun_", hasRun_ );
  adv.saveAttribute( "optimizeParameters_", optimizeParameters_ );
  adv.saveAttribute( "analyticalAmplitude_", analyticalAmplitude_ );
  adv.saveAttribute( "lastReducedLogLikelihood_", lastReducedLogLikelihood_ );
}


/* Method load() reloads the object from the StorageManager */
void GaussianProcessFitter::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "reducedCovarianceModel_", reducedCovarianceModel_ );
  adv.loadAttribute( "solver_", solver_ );
  adv.loadAttribute( "optimizationBounds_", optimizationBounds_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "rho_", rho_ );
  adv.loadAttribute( "F_", F_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.loadAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  UnsignedInteger method = 0;
  adv.loadAttribute( "method_", method );
  method_ = static_cast<LinearAlgebra>(method);
  adv.loadAttribute( "hasRun_", hasRun_ );
  adv.loadAttribute( "optimizeParameters_", optimizeParameters_ );
  adv.loadAttribute( "analyticalAmplitude_", analyticalAmplitude_ );
  adv.loadAttribute( "lastReducedLogLikelihood_", lastReducedLogLikelihood_ );
}

END_NAMESPACE_OPENTURNS
