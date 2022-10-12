//                                               -*- C++ -*-
/**
 *  @brief The class builds generalized linear models
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

#include "openturns/GeneralLinearModelAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/ProductCovarianceModel.hxx"
#include "openturns/TensorizedCovarianceModel.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/TNC.hxx"
#include "openturns/Cobyla.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicFunction.hxx"
#else
#include "openturns/DatabaseFunction.hxx"
#endif
#include "openturns/IdentityFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"
#include "openturns/MemoizeFunction.hxx"
#include "openturns/MultiStart.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralLinearModelAlgorithm)

static const Factory<GeneralLinearModelAlgorithm> Factory_GeneralLinearModelAlgorithm;

/* Default constructor */
GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm()
  : MetaModelAlgorithm(Sample(0, 1), Sample(0, 1))
  , covarianceModel_()
  , reducedCovarianceModel_()
  , solver_()
  , optimizationBounds_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basisCollection_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , optimizeParameters_(true)
  , analyticalAmplitude_(false)
  , lastReducedLogLikelihood_(SpecFunc::LowestScalar)
{
  // Set the default covariance to adapt the active parameters of the covariance model
  setCovarianceModel(CovarianceModel());
  initializeDefaultOptimizationAlgorithm();
}

/* Parameters constructor */
GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm(inputSample, outputSample)
  , covarianceModel_()
  , reducedCovarianceModel_()
  , solver_()
  , optimizationBounds_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basisCollection_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-OptimizeParameters"))
  , analyticalAmplitude_(false)
  , lastReducedLogLikelihood_(SpecFunc::LowestScalar)
{
  // If no basis then we suppose output sample centered
  checkYCentered(outputSample);
  // Set covariance model
  setCovarianceModel(covarianceModel);

  initializeMethod();
  initializeDefaultOptimizationAlgorithm();
}

GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Basis & basis,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm(inputSample, outputSample)
  , covarianceModel_()
  , reducedCovarianceModel_()
  , solver_()
  , optimizationBounds_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basisCollection_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-OptimizeParameters"))
  , analyticalAmplitude_(false)
  , lastReducedLogLikelihood_(SpecFunc::LowestScalar)
{
  // Set covariance model
  setCovarianceModel(covarianceModel);

  if (basis.getSize() > 0)
  {
    if (basis[0].getOutputDimension() > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but first function has dimension " << basis[0].getOutputDimension() << ". Only the first output component will be taken into account.");
    if (outputSample.getDimension() > 1) LOGWARN(OSS() << "The basis of functions will be applied to all output marginals" );
    // Set basis
    basisCollection_ = BasisCollection(outputSample.getDimension(), basis);
  }
  else
  {
    // If no basis then we suppose output sample centered
    checkYCentered(outputSample);
  }

  initializeMethod();
  initializeDefaultOptimizationAlgorithm();
}


/* Parameters constructor */
GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const BasisCollection & basisCollection,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm(inputSample, outputSample)
  , covarianceModel_()
  , reducedCovarianceModel_()
  , solver_()
  , optimizationBounds_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basisCollection_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-OptimizeParameters"))
  , analyticalAmplitude_(false)
  , lastReducedLogLikelihood_(SpecFunc::LowestScalar)
{
  LOGWARN(OSS() << "GeneralLinearModelAlgorithm(inputSample, outpuSample, covarianceModel, basisCollection, keepCholeskyFactor) is deprecated");
  // Set covariance model
  setCovarianceModel(covarianceModel);

  // Set basis collection
  if (basisCollection.getSize() > 0) setBasisCollection(basisCollection);

  initializeMethod();
  initializeDefaultOptimizationAlgorithm();
}

/* Covariance model accessors */
void GeneralLinearModelAlgorithm::setCovarianceModel(const CovarianceModel & covarianceModel)
{
  // Here we can store any modified version of the given covariance model wrt its parameters as it is mainly a parametric template
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  const UnsignedInteger dimension = outputSample_.getDimension();

  if (covarianceModel.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Covariance model input dimension is " << covarianceModel.getInputDimension() << ", expected " << inputDimension;
  if (covarianceModel.getOutputDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Covariance model output dimension is " << covarianceModel.getOutputDimension() << ", expected " << dimension;
  covarianceModel_ = covarianceModel;
  // All the computation will be done on the reduced covariance model. We keep the initial covariance model (ie the one we just built) in order to reinitialize the reduced covariance model if some flags are changed after the creation of the algorithm.
  reducedCovarianceModel_ = covarianceModel_;
  // Now, adapt the model parameters.
  // First, check if the parameters have to be optimized. If not, remove all the active parameters.
  analyticalAmplitude_ = false;
  if (!optimizeParameters_) reducedCovarianceModel_.setActiveParameter(Indices());
  // Second, check if the amplitude parameter is unique and active
  else if (ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate") && (!noise_.getSize()))
  {
    // The model has to be of dimension 1
    if (reducedCovarianceModel_.getOutputDimension() == 1)
    {
      const Description activeParametersDescription(reducedCovarianceModel_.getParameterDescription());
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
          // Now, the amplitude has disapear form the active parameters so it must
          // be updated using the amplitude accessor.
          reducedCovarianceModel_.setAmplitude(Point(1, 1.0));
          break;
        }
    } // reducedCovarianceModel_.getDimension() == 1
  } // optimizeParameters_
  LOGINFO(OSS() << "final active parameters=" << reducedCovarianceModel_.getActiveParameter());
  // Define the bounds of the optimization problem
  const UnsignedInteger optimizationDimension = reducedCovarianceModel_.getParameter().getSize();
  if (optimizationDimension > 0)
  {
    const Scalar scaleFactor(ResourceMap::GetAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor"));
    if (!(scaleFactor > 0))
      throw InvalidArgumentException(HERE) << "Scale factor set in ResourceMap is invalid. It should be a positive value. Here, scale = " << scaleFactor ;
    const Point lowerBound(optimizationDimension, ResourceMap::GetAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound"));
    Point upperBound(optimizationDimension, ResourceMap::GetAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound"));
    // We could set scale parameter if these parameters are enabled.
    // check if scale is active
    const Indices activeParameters(reducedCovarianceModel_.getActiveParameter());
    Bool isScaleActive(true);
    for (UnsignedInteger k = 0; k < reducedCovarianceModel_.getScale().getSize(); ++k)
    {
      if (!activeParameters.contains(k))
        isScaleActive = false;
    }
    if (isScaleActive)
    {
      const Point inputSampleRange(inputSample_.getMax() - inputSample_.getMin());
      for (UnsignedInteger k = 0; k < reducedCovarianceModel_.getScale().getSize(); ++k) upperBound[k] = inputSampleRange[k] * scaleFactor;
    }
    LOGWARN(OSS() <<  "Warning! For coherency we set scale upper bounds = " << upperBound.__str__());

    optimizationBounds_ = Interval(lowerBound, upperBound);
  }
  else optimizationBounds_ = Interval();
}

CovarianceModel GeneralLinearModelAlgorithm::getCovarianceModel() const
{
  return covarianceModel_;
}

CovarianceModel GeneralLinearModelAlgorithm::getReducedCovarianceModel() const
{
  return reducedCovarianceModel_;
}

/* Set basis collection method */
void GeneralLinearModelAlgorithm::setBasisCollection(const BasisCollection & basis)
{
  // If basis given, then its size should be the same as the output dimension (each marginal of multibasis is a basis that will be used for trend of the corresponding marginal.
  if (basis.getSize() != outputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm, output sample dimension=" << outputSample_.getDimension()  << " does not match multi-basis dimension=" << basis.getSize();
  // Get the output dimension of the basis
  // The first marginal may be an empty basis
  Bool continuationCondition = true;
  UnsignedInteger index = 0;
  UnsignedInteger outputDimension = 0;
  while(continuationCondition)
  {
    try
    {
      outputDimension =  basis[index][0].getOutputDimension();
      continuationCondition = false;
    }
    catch (const InvalidArgumentException &)
    {
      ++index;
      continuationCondition = continuationCondition && index < basis.getSize();
    }
  }
  if (outputDimension == 0)
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm, basisCollection argument contains basis with empty collection of functions";
  if (outputDimension > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but some function has dimension" << outputDimension << ". Only the first output component will be taken into account.");
  // Everything is ok, we set the basis
  basisCollection_ = basis;
}

void GeneralLinearModelAlgorithm::checkYCentered(const Sample & Y)
{
  const Scalar meanEpsilon = ResourceMap::GetAsScalar("GeneralLinearModelAlgorithm-MeanEpsilon");
  const Point meanY(Y.computeMean());
  for (UnsignedInteger k = 0; k < meanY.getDimension(); ++k)
  {
    if (std::abs(meanY[k]) > meanEpsilon)
      LOGWARN(OSS() << "In GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm, basis is empty and output sample is not centered, mean=" << meanY);
  }
}

void GeneralLinearModelAlgorithm::initializeDefaultOptimizationAlgorithm()
{
  const String solverName(ResourceMap::GetAsString("GeneralLinearModelAlgorithm-DefaultOptimizationAlgorithm"));
  solver_ = OptimizationAlgorithm::Build(solverName);
  Cobyla* cobyla = dynamic_cast<Cobyla *>(solver_.getImplementation().get());
  if (cobyla)
    cobyla->setIgnoreFailure(true);
  TNC* tnc = dynamic_cast<TNC *>(solver_.getImplementation().get());
  if (tnc)
    tnc->setIgnoreFailure(true);
}

/* Virtual constructor */
GeneralLinearModelAlgorithm * GeneralLinearModelAlgorithm::clone() const
{
  return new GeneralLinearModelAlgorithm(*this);
}

/* Compute the design matrix */
void GeneralLinearModelAlgorithm::computeF()
{
  // Nothing to do if the design matrix has already been computed
  if (F_.getNbRows() != 0) return;
  LOGINFO("Compute the design matrix");
  // No early exit based on the sample/basis size as F_ must be initialized with the correct dimensions
  // With a multivariate basis of size similar to output dimension, each ith-basis should be applied to elements
  // of corresponding marginal
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const UnsignedInteger sampleSize = inputSample_.getSize();
  const UnsignedInteger basisCollectionSize = basisCollection_.getSize();
  UnsignedInteger totalSize = 0;
  for (UnsignedInteger i = 0; i < basisCollectionSize; ++ i ) totalSize += basisCollection_[i].getSize();
  // if totalSize > 0, then basisCollection_.getSize() should be equal to outputDimension
  // This is checked in GeneralLinearModelAlgorithm::GeneralLinearModelAlgorithm
  F_ = Matrix(sampleSize * outputDimension, totalSize);
  if (totalSize == 0) return;
  // Compute F
  UnsignedInteger index = 0;
  for (UnsignedInteger outputMarginal = 0; outputMarginal < outputDimension; ++ outputMarginal )
  {
    const Basis localBasis = basisCollection_[outputMarginal];
    const UnsignedInteger localBasisSize = localBasis.getSize();
    for (UnsignedInteger j = 0; j < localBasisSize; ++j, ++index )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const Sample basisSample = localBasis[j](inputSample_);
      for (UnsignedInteger i = 0; i < sampleSize; ++i) F_(outputMarginal + i * outputDimension, index) = basisSample(i, 0);
    }
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

void GeneralLinearModelAlgorithm::run()
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
  //   computeReducedLogLikelyhood() which has side effects on covariance
  //   discretization and factorization, and it computes beta_
  Scalar optimalLogLikelihood = maximizeReducedLogLikelihood();

  LOGINFO("Store the estimates");
  // Here we do the work twice:
  // 1) To get a collection of Point for the result class
  // 2) To get same results as Sample for the trend NMF
  Collection<Point> trendCoefficients(basisCollection_.getSize());
  Sample trendCoefficientsSample(beta_.getSize(), reducedCovarianceModel_.getOutputDimension());

  UnsignedInteger cumulatedSize = 0;
  for (UnsignedInteger outputIndex = 0; outputIndex < basisCollection_.getSize(); ++ outputIndex)
  {
    const UnsignedInteger localBasisSize = basisCollection_[outputIndex].getSize();
    Point beta_i(localBasisSize);
    for(UnsignedInteger basisElement = 0; basisElement < localBasisSize; ++ basisElement)
    {
      beta_i[basisElement] = beta_[cumulatedSize];
      trendCoefficientsSample(cumulatedSize, outputIndex) =  beta_[cumulatedSize];
      ++cumulatedSize;
    }
    trendCoefficients[outputIndex] = beta_i;
  }

  LOGINFO("Build the output meta-model");
  // The meta model is of type DualLinearCombination function
  // We should write the coefficients into a Sample and build the basis into a collection
  Collection<Function> allFunctionsCollection;
  for (UnsignedInteger k = 0; k < basisCollection_.getSize(); ++k)
    for (UnsignedInteger l = 0; l < basisCollection_[k].getSize(); ++l)
      allFunctionsCollection.add(basisCollection_[k].build(l));
  Function metaModel;

  if (basisCollection_.getSize() > 0)
  {
    // Care ! collection should be non empty
    metaModel = DualLinearCombinationFunction(allFunctionsCollection, trendCoefficientsSample);
  }
  else
  {
    // If no basis ==> zero function
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
    metaModel = SymbolicFunction(Description::BuildDefault(covarianceModel_.getInputDimension(), "x"), Description(covarianceModel_.getOutputDimension(), "0.0"));
#else
    metaModel = DatabaseFunction(Sample(1, reducedCovarianceModel_.getInputDimension()), Sample(1, reducedCovarianceModel_.getOutputDimension()));
#endif
  }

  // compute residual, relative error
  const Point outputVariance(outputSample_.computeVariance());
  const Sample mY(metaModel(inputSample_));
  const Point squaredResiduals((outputSample_ - mY).computeRawMoment(2));

  Point residuals(outputDimension);
  Point relativeErrors(outputDimension);

  const UnsignedInteger size = inputSample_.getSize();
  for ( UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex )
  {
    residuals[outputIndex] = sqrt(squaredResiduals[outputIndex] / size);
    relativeErrors[outputIndex] = squaredResiduals[outputIndex] / outputVariance[outputIndex];
  }

  // return optimized covmodel with the original active parameters (see analyticalAmplitude_)
  CovarianceModel reducedCovarianceModelCopy(reducedCovarianceModel_);
  reducedCovarianceModelCopy.setActiveParameter(covarianceModel_.getActiveParameter());

  result_ = GeneralLinearModelResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basisCollection_, trendCoefficients, reducedCovarianceModelCopy, optimalLogLikelihood);

  // The scaling is done there because it has to be done as soon as some optimization has been done, either numerically or through an analytical formula
  if (keepCholeskyFactor_)
  {
    if (analyticalAmplitude_)
    {
      const Scalar sigma = reducedCovarianceModel_.getAmplitude()[0];
      // Case of LAPACK backend
      if (method_ == LAPACK) covarianceCholeskyFactor_ = covarianceCholeskyFactor_ * sigma;
      else covarianceCholeskyFactorHMatrix_.scale(sigma);
    }
    result_.setCholeskyFactor(covarianceCholeskyFactor_, covarianceCholeskyFactorHMatrix_);
  }
  else
    result_ = GeneralLinearModelResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basisCollection_, trendCoefficients, reducedCovarianceModelCopy, optimalLogLikelihood);
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
Scalar GeneralLinearModelAlgorithm::maximizeReducedLogLikelihood()
{
  // initial guess
  Point initialParameters(reducedCovarianceModel_.getParameter());
  Indices initialActiveParameters(reducedCovarianceModel_.getActiveParameter());
  // We use the functional form of the log-likelihood computation to benefit from the cache mechanism
  Function reducedLogLikelihoodFunction(getObjectiveFunction());
  const Bool noNumericalOptimization = initialParameters.getSize() == 0;
  // Early exit if the parameters are known
  if (noNumericalOptimization)
  {
    // We only need to compute the log-likelihood function at the initial parameters in order to get the Cholesky factor and the trend coefficients
    const Scalar initialReducedLogLikelihood = reducedLogLikelihoodFunction(initialParameters)[0];
    LOGINFO("No covariance parameter to optimize");
    LOGINFO(OSS() << "initial parameters=" << initialParameters << ", log-likelihood=" << initialReducedLogLikelihood);
    return initialReducedLogLikelihood;
  }
  // At this point we have an optimization problem to solve
  // Define the optimization problem
  OptimizationProblem problem(reducedLogLikelihoodFunction);
  problem.setMinimization(false);
  problem.setBounds(optimizationBounds_);
  solver_.setProblem(problem);
  try
  {
    // If the solver is single start, we can use its setStartingPoint method
    solver_.setStartingPoint(initialParameters);
  }
  catch (const NotDefinedException &) // setStartingPoint is not defined for the solver
  {
    // Nothing to do if setStartingPoint is not defined
  }
  LOGINFO(OSS(false) << "Solve problem=" << problem << " using solver=" << solver_);
  solver_.run();
  const OptimizationAlgorithm::Result result(solver_.getResult());
  const Scalar optimalLogLikelihood = result.getOptimalValue()[0];
  const Point optimalParameters = result.getOptimalPoint();
  const UnsignedInteger evaluationNumber = result.getEvaluationNumber();
  // Check if the optimal value corresponds to the last computed value, in order to
  // see if the by-products (Cholesky factor etc) are correct
  if (lastReducedLogLikelihood_ != optimalLogLikelihood)
  {
    LOGDEBUG(OSS(false) << "Need to evaluate the objective function one more time because the last computed reduced log-likelihood value=" << lastReducedLogLikelihood_ << " is different from the optimal one=" << optimalLogLikelihood);
    (void) computeReducedLogLikelihood(optimalParameters);
  }
  // Final call to reducedLogLikelihoodFunction() in order to update the amplitude
  // No additional cost since the cache mechanism is activated
  LOGINFO(OSS() << evaluationNumber << " evaluations, optimized parameters=" << optimalParameters << ", log-likelihood=" << optimalLogLikelihood);

  return optimalLogLikelihood;
}

Point GeneralLinearModelAlgorithm::computeReducedLogLikelihood(const Point & parameters) const
{
  // Check that the parameters have a size compatible with the covariance model
  if (parameters.getSize() != reducedCovarianceModel_.getParameter().getSize())
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelAlgorithm::computeReducedLogLikelihood, could not compute likelihood,"
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
  if (method_ == LAPACK)
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
    const Scalar sigma = std::sqrt(rho_.normSquare() / (ResourceMap::GetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance") ? size - beta_.getSize() : size));
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
  LOGINFO(OSS(false) << "Point " << parameters << " -> reduced log-likelihood=" << lastReducedLogLikelihood_);
  return Point(1, lastReducedLogLikelihood_);
}


Scalar GeneralLinearModelAlgorithm::computeLapackLogDeterminantCholesky() const
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = diag(sigma) * R(s,t) * diag(sigma) with R a correlation function
  LOGDEBUG(OSS(false) << "Compute the LAPACK log-determinant of the Cholesky factor for covariance=" << reducedCovarianceModel_);

  LOGDEBUG("Discretize the covariance model");
  CovarianceMatrix C(reducedCovarianceModel_.discretize(inputSample_));
  if (noise_.getDimension() > 0)
  {
    LOGDEBUG("Add noise to the covariance matrix");
    for (UnsignedInteger i = 0; i < C.getDimension(); ++ i) C(i, i) += noise_[i];
  }
  if (C.getDimension() < 20)
    LOGDEBUG(OSS(false) << "C=\n" << C);
  LOGDEBUG("Compute the Cholesky factor of the covariance matrix");
  Bool continuationCondition = true;
  Scalar maxEV = -1.0;
  const Scalar startingScaling = ResourceMap::GetAsScalar("GeneralLinearModelAlgorithm-StartingScaling");
  const Scalar maximalScaling = ResourceMap::GetAsScalar("GeneralLinearModelAlgorithm-MaximalScaling");
  Scalar cumulatedScaling = 0.0;
  Scalar scaling = startingScaling;
  while (continuationCondition)
  {
    try
    {
      covarianceCholeskyFactor_ = C.computeCholesky();
      continuationCondition = false;
    }
    // If the factorization failed regularize the matrix
    // Here we use a generic exception as different exceptions may be thrown
    catch (const Exception &)
    {
      // If the largest eigenvalue module has not been computed yet...
      if (maxEV < 0.0)
      {
        maxEV = C.computeLargestEigenValueModule();
        LOGDEBUG(OSS() << "maxEV=" << maxEV);
        scaling *= maxEV;
      }
      cumulatedScaling += scaling ;
      LOGDEBUG(OSS() << "scaling=" << scaling << ", cumulatedScaling=" << cumulatedScaling);
      // Unroll the regularization to optimize the computation
      for (UnsignedInteger i = 0; i < C.getDimension(); ++i) C(i, i) += scaling;
      scaling *= 2.0;
      continuationCondition = scaling < maxEV * maximalScaling;
    }
  }
  if (maxEV > 0.0 && scaling >= maximalScaling * maxEV)
    throw InvalidArgumentException(HERE) << "In GeneralLinearModelAlgorithm::computeLapackLogDeterminantCholesky, could not compute the Cholesky factor."
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");
  LOGDEBUG(OSS(false) << "L=\n" << covarianceCholeskyFactor_);

  // y corresponds to output data
  const Point y(outputSample_.getImplementation()->getData());
  LOGDEBUG(OSS(false) << "y=" << y);
  // rho = L^{-1}y
  LOGDEBUG("Solve L.rho = y");
  rho_ = covarianceCholeskyFactor_.solveLinearSystem(y);
  LOGDEBUG(OSS(false) << "rho_=L^{-1}y=" << rho_);
  // If trend to estimate
  if (basisCollection_.getSize() > 0)
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

Scalar GeneralLinearModelAlgorithm::computeHMatLogDeterminantCholesky() const
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
  Point y(outputSample_.getImplementation()->getData());
  // rho = L^{-1}y
  LOGDEBUG("Solve L.rho = y");
  rho_ = covarianceCholeskyFactorHMatrix_.solveLower(y);
  // If trend to estimate
  if (basisCollection_.getSize() > 0)
  {
    // Phi = L^{-1}F
    LOGDEBUG("Solve L.Phi = F");
    Matrix Phi(covarianceCholeskyFactorHMatrix_.solveLower(F_));
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
OptimizationAlgorithm GeneralLinearModelAlgorithm::getOptimizationAlgorithm() const
{
  return solver_;
}

void GeneralLinearModelAlgorithm::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
  hasRun_ = false;
}

/* Optimize parameters flag accessor */
Bool GeneralLinearModelAlgorithm::getOptimizeParameters() const
{
  return optimizeParameters_;
}

void GeneralLinearModelAlgorithm::setOptimizeParameters(const Bool optimizeParameters)
{
  if (optimizeParameters != optimizeParameters_)
  {
    optimizeParameters_ = optimizeParameters;
    // Here we have to call setCovarianceModel() as it computes reducedCovarianceModel from covarianceModel_ in a way influenced by optimizeParameters_ flag.
    setCovarianceModel(covarianceModel_);
  }
}

/* Accessor to optimization bounds */
void GeneralLinearModelAlgorithm::setOptimizationBounds(const Interval & optimizationBounds)
{
  if (!(optimizationBounds.getDimension() == optimizationBounds_.getDimension())) throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << optimizationBounds_.getDimension() << ", got dimension=" << optimizationBounds.getDimension();
  optimizationBounds_ = optimizationBounds;
  hasRun_ = false;
}

Interval GeneralLinearModelAlgorithm::getOptimizationBounds() const
{
  return optimizationBounds_;
}

/* Observation noise accessor */
void GeneralLinearModelAlgorithm::setNoise(const Point & noise)
{
  const UnsignedInteger size = inputSample_.getSize();
  if (noise.getSize() != size) throw InvalidArgumentException(HERE) << "Noise size=" << noise.getSize()  << " does not match sample size=" << size;
  // Currently setNoise is not handled with HMAT
  // We should first rework on the hmat side to promote this possibility
  if (getMethod() == HMAT)
    throw NotYetImplementedException(HERE) << "Noise is not be handled with HMAT method";
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (!(noise[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise must be positive";
  noise_ = noise;
  // If we update noise, we need to reset
  reset();
}

Point GeneralLinearModelAlgorithm::getNoise() const
{
  return noise_;
}


Point GeneralLinearModelAlgorithm::getRho() const
{
  return rho_;
}

/* String converter */
String GeneralLinearModelAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << ", inputSample=" << inputSample_
      << ", outputSample=" << outputSample_
      << ", basis=" << basisCollection_
      << ", covarianceModel=" << covarianceModel_
      << ", reducedCovarianceModel=" << reducedCovarianceModel_
      << ", solver=" << solver_
      << ", optimizeParameters=" << optimizeParameters_
      << ", noise=" << noise_;
  return oss;
}


GeneralLinearModelResult GeneralLinearModelAlgorithm::getResult()
{
  if (!hasRun_) run();
  return result_;
}


Function GeneralLinearModelAlgorithm::getObjectiveFunction()
{
  computeF();
  MemoizeFunction logLikelihood(ReducedLogLikelihoodEvaluation(*this));
  // Here we change the finite difference gradient for a non centered one in order to reduce the computational cost
  const Scalar finiteDifferenceEpsilon = ResourceMap::GetAsScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon" );
  logLikelihood.setGradient(NonCenteredFiniteDifferenceGradient(finiteDifferenceEpsilon, logLikelihood.getEvaluation()).clone());
  logLikelihood.enableCache();
  return logLikelihood;
}

void GeneralLinearModelAlgorithm::initializeMethod()
{
  if (ResourceMap::GetAsString("GeneralLinearModelAlgorithm-LinearAlgebra") == "HMAT")
    method_ = HMAT;
}

UnsignedInteger GeneralLinearModelAlgorithm::getMethod() const
{
  return method_;
}

void GeneralLinearModelAlgorithm::reset()
{
  // Reset elements for new computation
  // No need to update F_ as computeF /setBasisCollection are private
  // Same remark for setCovarianceModel & setData
  covarianceCholeskyFactor_ = TriangularMatrix();
  covarianceCholeskyFactorHMatrix_ = HMatrix();
  hasRun_ = false;
  lastReducedLogLikelihood_ = SpecFunc::LowestScalar;
}

/* Method accessor (lapack/hmat) - Protected but friend with GeneralLinearModelAlgorithm class */
void GeneralLinearModelAlgorithm::setMethod(const UnsignedInteger method)
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
void GeneralLinearModelAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "reducedCovarianceModel_", reducedCovarianceModel_ );
  adv.saveAttribute( "solver_", solver_ );
  adv.saveAttribute( "optimizationBounds_", optimizationBounds_ );
  adv.saveAttribute( "basisCollection_", basisCollection_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "method", method_ );
  adv.saveAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.saveAttribute( "optimizeParameters_", optimizeParameters_ );
  adv.saveAttribute( "noise_", noise_ );
}


/* Method load() reloads the object from the StorageManager */
void GeneralLinearModelAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "reducedCovarianceModel_", reducedCovarianceModel_ );
  adv.loadAttribute( "solver_", solver_ );
  adv.loadAttribute( "optimizationBounds_", optimizationBounds_ );
  adv.loadAttribute( "basisCollection_", basisCollection_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "method", method_ );
  adv.loadAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.loadAttribute( "optimizeParameters_", optimizeParameters_ );
  adv.loadAttribute( "noise_", noise_ );
}

END_NAMESPACE_OPENTURNS
