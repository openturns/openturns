//                                               -*- C++ -*-
/**
 *  @brief The class builds generalized linear models
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

#include "openturns/GeneralizedLinearModelAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/ProductCovarianceModel.hxx"
#include "openturns/TensorizedCovarianceModel.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/TNC.hxx"
#include "openturns/NLopt.hxx"
#include "openturns/AnalyticalFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedLinearModelAlgorithm);

static const Factory<GeneralizedLinearModelAlgorithm> Factory_GeneralizedLinearModelAlgorithm;

/** Default constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm ()
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , normalizedInputSample_(0, 0)
  , inputTransformation_()
  , normalize_(false)
  , outputSample_(0, 0)
  , covarianceModel_()
  , solver_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(false)
  , method_(0)
  , hasRun_(false)
  , optimizeParameters_(true)
{
  // Nothing to do
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const CovarianceModel & covarianceModel,
    const Bool normalize,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , normalizedInputSample_(0, 0)
  , inputTransformation_()
  , normalize_(normalize)
  , outputSample_(0, 0)
  , covarianceModel_()
  , solver_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , method_(0)
  , hasRun_(false)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-OptimizeParameters"))
{
  // set data & covariance model
  setData(inputSample, outputSample);
  // If no basis then we suppose output sample centered
  checkYCentered(outputSample);
  setCovariance(covarianceModel);

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension = inputSample_.getDimension();
    const NumericalPoint mean(inputSample_.computeMean());
    const NumericalPoint stdev(inputSample_.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (std::abs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearFunction(mean, zero, linear));
  }
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const CovarianceModel & covarianceModel,
    const Basis & basis,
    const Bool normalize,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_()
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(normalize)
  , outputSample_()
  , covarianceModel_()
  , solver_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , method_(0)
  , hasRun_(false)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-OptimizeParameters"))
{
  // set data & covariance model
  setData(inputSample, outputSample);
  setCovariance(covarianceModel);

  if (basis.getSize() > 0)
  {
    if (basis[0].getOutputDimension() > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but first function has dimension " << basis[0].getOutputDimension() << ". Only the first output component will be taken into account.");
    if (outputSample.getDimension() > 1) LOGWARN(OSS() << "The basis of functions will be applied to all output marginals" );
    // Set basis
    basis_ = BasisCollection(outputSample.getDimension(), basis);
  }
  else
  {
    // If no basis then we suppose output sample centered
    checkYCentered(outputSample);
  }

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension = inputSample_.getDimension();
    const NumericalPoint mean(inputSample_.computeMean());
    const NumericalPoint stdev(inputSample_.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (std::abs(stdev[j]) > SpecFunc::NumericalScalarEpsilon) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearFunction(mean, zero, linear));
  }
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
    const NumericalMathFunction & inputTransformation,
    const NumericalSample & outputSample,
    const CovarianceModel & covarianceModel,
    const Basis & basis,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_()
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(true)
  , outputSample_()
  , covarianceModel_()
  , solver_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , method_(0)
  , hasRun_(false)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-OptimizeParameters"))
{
  // set data & covariance model
  setData(inputSample, outputSample);
  setCovariance(covarianceModel);

  // basis setter
  if (basis.getSize() > 0)
  {
    if (basis[0].getOutputDimension() > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but first function has dimension" << basis[0].getOutputDimension() << ". Only the first output component will be taken into account.");
    if (outputSample.getDimension() > 1) LOGWARN(OSS() << "The basis of functions will be applied to all output marginals" );
    // Set basis
    basis_ = BasisCollection(outputSample.getDimension(), basis);
  }
  else
  {
    // If no basis then we suppose output sample centered
    checkYCentered(outputSample);
  }

  // Set the isoprobabilistic transformation
  setInputTransformation(inputTransformation);
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const CovarianceModel & covarianceModel,
    const BasisCollection & multivariateBasis,
    const Bool normalize,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(normalize)
  , outputSample_(outputSample)
  , covarianceModel_(covarianceModel)
  , solver_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , method_(0)
  , hasRun_(false)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-OptimizeParameters"))
{
  // set data & covariance model
  setData(inputSample, outputSample);
  setCovariance(covarianceModel);

  // Set basis collection
  if (multivariateBasis.getSize() > 0) setBasis(multivariateBasis);

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension = inputSample_.getDimension();
    const NumericalPoint mean(inputSample_.computeMean());
    const NumericalPoint stdev(inputSample_.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (std::abs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearFunction(mean, zero, linear));
  }
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
    const NumericalMathFunction & inputTransformation,
    const NumericalSample & outputSample,
    const CovarianceModel & covarianceModel,
    const BasisCollection & multivariateBasis,
    const Bool keepCholeskyFactor)
  : MetaModelAlgorithm()
  , inputSample_()
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(true)
  , outputSample_()
  , covarianceModel_()
  , solver_()
  , beta_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceCholeskyFactorHMatrix_()
  , keepCholeskyFactor_(keepCholeskyFactor)
  , method_(0)
  , hasRun_(false)
  , optimizeParameters_(ResourceMap::GetAsBool("GeneralizedLinearModelAlgorithm-OptimizeParameters"))
{
  // set data & covariance model
  setData(inputSample, outputSample);
  setCovariance(covarianceModel);

  // Set basis collection
  if (multivariateBasis.getSize() > 0)  setBasis(multivariateBasis);

  // Set the isoprobabilistic transformation
  setInputTransformation(inputTransformation);
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

/** set sample  method */
void GeneralizedLinearModelAlgorithm::setData(const NumericalSample & inputSample,
    const NumericalSample & outputSample)
{
  // Check the sample sizes
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, input sample size (" << inputSample.getSize() << ") does not match output sample size (" << outputSample.getSize() << ").";
  // Set samples
  inputSample_ = inputSample;
  outputSample_ = outputSample;
}

/** set covariance method */
void GeneralizedLinearModelAlgorithm::setCovariance(const CovarianceModel & covarianceModel)
{
  const UnsignedInteger inputDimension = inputSample_.getDimension();
  const UnsignedInteger dimension = outputSample_.getDimension();

  // Check dimensions of the covariance model
  // Normal case
  if ((covarianceModel.getDimension() == dimension) && (inputDimension == covarianceModel.getSpatialDimension()))
    covarianceModel_ = covarianceModel;
  else if ((covarianceModel.getDimension() == dimension) && (inputDimension != covarianceModel.getSpatialDimension()))
  {
    if ((covarianceModel.getSpatialDimension() == 1) && dimension == 1)
      // Define the product covariance model for dimension = 1
      covarianceModel_ = ProductCovarianceModel(ProductCovarianceModel::CovarianceModelCollection(inputDimension, covarianceModel));
    else
      throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, input sample dimension (" << inputDimension << ") does not match covariance model spatial dimension (" << covarianceModel.getSpatialDimension() << ").";
  }
  // dimension nok
  else
  {
    if (covarianceModel.getDimension() != 1)
      throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, output sample dimension (" << dimension << ") does not match covariance model dimension (" << covarianceModel.getDimension() << ").";
    // Case of dimension = 1
    // Creation of a TensorizedCovarianceModel
    // Check input sample dimension
    if (inputDimension == covarianceModel.getSpatialDimension())
      covarianceModel_ = TensorizedCovarianceModel(TensorizedCovarianceModel::CovarianceModelCollection(dimension, covarianceModel));
    else
    {
      if (covarianceModel.getSpatialDimension() != 1)
        throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, input sample dimension (" << inputDimension << ") does not match covariance model spatial dimension (" << covarianceModel.getSpatialDimension() << ").";
      // Define the product covariance model
      CovarianceModel productCovarianceModel = ProductCovarianceModel(ProductCovarianceModel::CovarianceModelCollection(inputDimension, covarianceModel));
      covarianceModel_ = TensorizedCovarianceModel(TensorizedCovarianceModel::CovarianceModelCollection(dimension, productCovarianceModel));
    }
  }
}

/** Set basis collection method */
void GeneralizedLinearModelAlgorithm::setBasis(const BasisCollection & basis)
{
  // If basis given, then its size should be the same as the output dimension (each marginal of multibasis is a basis that will be used for trend of the corresponding marginal.
  if (basis.getSize() != outputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, output sample dimension (" << outputSample_.getDimension()  << ") does not match multi-basis dimension (" << basis.getSize() << ")";
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
    catch (InvalidArgumentException)
    {
      ++index;
      continuationCondition = continuationCondition && index < basis.getSize();
    }
  }
  if (outputDimension == 0)
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, basisCollection argument contains basis with empty collection of functions";
  if (outputDimension > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but some function has dimension" << outputDimension << ". Only the first output component will be taken into account.");
  // Everything is ok, we set the basis
  basis_ = basis;
}

void GeneralizedLinearModelAlgorithm::checkYCentered(const NumericalSample & Y)
{
  const NumericalScalar meanEpsilon = ResourceMap::GetAsNumericalScalar("GeneralizedLinearModelAlgorithm-MeanEpsilon");
  const NumericalPoint meanY(Y.computeMean());
  for (UnsignedInteger k = 0; k < meanY.getDimension(); ++k)
  {
    if (std::abs(meanY[k]) > meanEpsilon)
      //      throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, basis is empty and output sample is not centered";
      LOGWARN(OSS() << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, basis is empty and output sample is not centered, mean=" << meanY);
  }
}

void GeneralizedLinearModelAlgorithm::initializeDefaultOptimizationSolver()
{
  const String solverName(ResourceMap::Get("GeneralizedLinearModelAlgorithm-DefaultOptimizationSolver"));
  if (solverName == "TNC")
    solver_ = TNC();
  else if (solverName == "NELDER-MEAD")
    solver_ = NLopt("LN_NELDERMEAD");
  else if (solverName == "LBFGS")
    solver_ = NLopt("LD_LBFGS");
  else
    throw InvalidArgumentException(HERE) << "Unknown optimization solver:" << solverName;

  // Bounds should be of size spatialDimension + dimension
  const UnsignedInteger optimizationDimension = covarianceModel_.getParameter().getSize();
  const NumericalPoint lowerBound(optimizationDimension, ResourceMap::GetAsNumericalScalar( "GeneralizedLinearModelAlgorithm-DefaultOptimizationLowerBound"));
  const NumericalPoint upperBound(optimizationDimension, ResourceMap::GetAsNumericalScalar( "GeneralizedLinearModelAlgorithm-DefaultOptimizationUpperBound"));
  optimizationBounds_ = Interval(lowerBound, upperBound);
}

/* Virtual constructor */
GeneralizedLinearModelAlgorithm * GeneralizedLinearModelAlgorithm::clone() const
{
  return new GeneralizedLinearModelAlgorithm(*this);
}


/* Normalize the input sample */
void GeneralizedLinearModelAlgorithm::normalizeInputSample()
{
  // Nothing to do if the sample has alredy been normalized
  if (normalizedInputSample_.getSize() != 0) return;
  // If we don't want to normalize the data
  if (!normalize_)
  {
    normalizedInputSample_ = inputSample_;
    return;
  }
  normalizedInputSample_ = inputTransformation_(inputSample_);
}

/* Compute the design matrix */
void GeneralizedLinearModelAlgorithm::computeF()
{
  // Nothing to do if the design matrix has already been computed
  if (F_.getNbRows() != 0) return;
  // No early exit based on the sample/basis size as F_ must be initialized with the correct dimensions
  // With a multivariate basis of size similar to output dimension, each ith-basis should be applied to elements
  // of corresponding marginal
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const UnsignedInteger sampleSize = normalizedInputSample_.getSize();
  const UnsignedInteger basisCollectionSize = basis_.getSize();
  UnsignedInteger totalSize = 0;
  for (UnsignedInteger i = 0; i < basisCollectionSize; ++ i ) totalSize += basis_[i].getSize();
  // if totalSize > 0, then basis_.getSize() should be equal to outputDimension
  // This is checked in GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm
  F_ = Matrix(sampleSize * outputDimension, totalSize);
  if (totalSize == 0) return;
  // Compute F
  UnsignedInteger index = 0;
  for (UnsignedInteger outputMarginal = 0; outputMarginal < outputDimension; ++ outputMarginal )
  {
    const Basis localBasis = basis_[outputMarginal];
    const UnsignedInteger localBasisSize = localBasis.getSize();
    for (UnsignedInteger j = 0; j < localBasisSize; ++j, ++index )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const NumericalSample basisSample = localBasis[j](normalizedInputSample_);
      for (UnsignedInteger i = 0; i < sampleSize; ++i) F_(outputMarginal + i * outputDimension, index) = basisSample[i][0];
    }
  }
}


/* Perform regression */
void GeneralizedLinearModelAlgorithm::run()
{
  // Do not run again if already computed
  if (hasRun_) return;
  LOGINFO("normalize the data");
  normalizeInputSample();
  LOGINFO("Compute the design matrix");
  computeF();
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  NumericalPointWithDescription covarianceModelParameters;
  // optimization of likelihood function if provided
  LOGINFO("Optimize the parameter of the marginal covariance model");
  // Here we call the optimizeLogLikelihood() method even if the
  // optimizeParameters flag is false, because we need the side-effect
  // (all the linear algebra) of this method at least for the current
  // covariance parameters. The flag is used in optimizeLogLikelihood()
  // to trigger an early exit so no optimization is made.
  covarianceModelParameters = optimizeLogLikelihood();
  LOGINFO("Store the estimates");
  // Here we do the work twice:
  // 1) To get a collection of NumericalPoint for the result class
  // 2) To get same results as NumericalSample for the trend NMF
  Collection<NumericalPoint> trendCoefficients(basis_.getSize());
  NumericalSample trendCoefficientsSample(beta_.getSize(), covarianceModel_.getDimension());

  UnsignedInteger cumulatedSize = 0;
  for (UnsignedInteger outputIndex = 0; outputIndex < basis_.getSize(); ++ outputIndex)
  {
    const UnsignedInteger localBasisSize = basis_[outputIndex].getSize();
    NumericalPoint beta_i(localBasisSize);
    for(UnsignedInteger basisElement = 0; basisElement < localBasisSize; ++ basisElement)
    {
      beta_i[basisElement] = beta_[cumulatedSize];
      trendCoefficientsSample[cumulatedSize][outputIndex] =  beta_[cumulatedSize];
      ++cumulatedSize;
    }
    trendCoefficients[outputIndex] = beta_i;
  }

  CovarianceModel conditionalCovarianceModel(covarianceModel_);
  conditionalCovarianceModel.setParameter(covarianceModelParameters);

  LOGINFO("Build the output meta-model");
  // The meta model is of type DualLinearCombination function
  // We should write the coefficients into a NumericalSample and build the basis into a collection
  Collection<NumericalMathFunction> allFunctionsCollection;
  for (UnsignedInteger k = 0; k < basis_.getSize(); ++k)
    for (UnsignedInteger l = 0; l < basis_[k].getSize(); ++l)
      allFunctionsCollection.add(basis_[k].build(l));
  NumericalMathFunction metaModel;

  if (basis_.getSize() > 0)
  {
    // Care ! collection should be non empty
    metaModel = NumericalMathFunction(allFunctionsCollection, trendCoefficientsSample);
  }
  else
  {
    // If no basis ==> zero function
#ifdef OPENTURNS_HAVE_MUPARSER
    metaModel = AnalyticalFunction(Description::BuildDefault(covarianceModel_.getSpatialDimension(), "x"), Description(covarianceModel_.getDimension(), "0.0"));
#else
    metaModel = NumericalMathFunction(NumericalSample(1, covarianceModel_.getSpatialDimension()), NumericalSample(1, covarianceModel_.getDimension()));
#endif
  }

  // Add transformation if needed
  if (normalize_) metaModel = ComposedFunction(metaModel, inputTransformation_);

  // compute residual, relative error
  const NumericalPoint outputVariance(outputSample_.computeVariance());
  const NumericalSample mY(metaModel(inputSample_));
  const NumericalPoint squaredResiduals((outputSample_ - mY).computeRawMoment(2));

  NumericalPoint residuals(outputDimension);
  NumericalPoint relativeErrors(outputDimension);

  const UnsignedInteger size = inputSample_.getSize();
  for ( UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex )
  {
    residuals[outputIndex] = sqrt(squaredResiduals[outputIndex] / size);
    relativeErrors[outputIndex] = squaredResiduals[outputIndex] / outputVariance[outputIndex];
  }

  if (keepCholeskyFactor_)
    result_ = GeneralizedLinearModelResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis_, trendCoefficients, conditionalCovarianceModel, covarianceCholeskyFactor_, covarianceCholeskyFactorHMatrix_);
  else
    result_ = GeneralizedLinearModelResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis_, trendCoefficients, conditionalCovarianceModel);
  // If normalize, set input transformation
  if (normalize_) result_.setTransformation(inputTransformation_);
  hasRun_ = true;
}

NumericalScalar GeneralizedLinearModelAlgorithm::computeLogLikelihood(const NumericalPoint & parameters) const
{
  if (parameters.getSize() != covarianceModel_.getParameter().getSize())
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::computeLogLikelihood, could not compute likelihood,"
                                         << " covariance model requires an argument of size " << covarianceModel_.getParameter().getSize()
                                         << " but here we got " << parameters.getSize();
  NumericalScalar logLikelihood = -1.0;
  if (method_ == 1)
    logLikelihood = computeHMatLogDeterminantCholesky(parameters);
  else
    logLikelihood = computeLapackLogDeterminantCholesky(parameters);
  // The lapack/hmat implementation computes :
  // 1) The log-determinant of the covariance matrix (log inverse). This is returned by the method
  // 2) rho_ : the point is updated
  // The next step is to compute the norm of this last one and added
  // rho is the residual choleskyFactor * (Y-F*beta)
  const NumericalScalar epsilon = rho_.normSquare();
  LOGDEBUG(OSS(false) << "epsilon=" << epsilon);
  if (epsilon <= 0) return SpecFunc::MaxNumericalScalar;
  // For the general multidimensional case, we have to compute the general log-likelihood (ie including marginal variances)
  logLikelihood -= 0.5 * epsilon;
  logLikelihood /= outputSample_.getSize();
  LOGINFO(OSS(false) << "Compute the estimated log-likelihood=" << logLikelihood);
  return logLikelihood;
}


NumericalScalar GeneralizedLinearModelAlgorithm::computeLapackLogDeterminantCholesky(const NumericalPoint & parameters) const
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = diag(sigma) * R(s,t) diag(sigma) with R a correlation function
  LOGINFO(OSS(false) << "Compute the LAPACK log-determinant of the Cholesky factor for theta=" << parameters);
  CovarianceModel model(covarianceModel_);
  model.setParameter(parameters);

  LOGINFO("Discretize the covariance model...");
  CovarianceMatrix C(model.discretize(normalizedInputSample_));
  if (noise_.getDimension() > 0)
  {
    for (UnsignedInteger i = 0; i < C.getDimension(); ++ i) C(i, i) += noise_[i];
  }
  LOGINFO("Compute the Cholesky factor of the covariance matrix");
  Bool continuationCondition = true;
  const NumericalScalar startingScaling = ResourceMap::GetAsNumericalScalar("GeneralizedLinearModelAlgorithm-StartingScaling");
  const NumericalScalar maximalScaling = ResourceMap::GetAsNumericalScalar("GeneralizedLinearModelAlgorithm-MaximalScaling");
  NumericalScalar cumulatedScaling = 0.0;
  NumericalScalar scaling = startingScaling;
  while (continuationCondition && (cumulatedScaling < maximalScaling))
  {
    try
    {
      covarianceCholeskyFactor_ = C.computeCholesky();
      continuationCondition = false;
    }
    // If it has not yet been computed, compute it and store it
    catch (InternalException &)
    {
      cumulatedScaling += scaling ;
      // Unroll the regularization to optimize the computation
      for (UnsignedInteger i = 0; i < C.getDimension(); ++i) C(i, i) += scaling;
      scaling *= 2.0;
    }
  }
  if (scaling >= maximalScaling)
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::computeLapackLogDeterminantCholesky, could not compute the Cholesky factor."
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");

  // y corresponds to output data
  const NumericalPoint y(outputSample_.getImplementation()->getData());
  LOGINFO(OSS(false) << "y=" << y);
  // rho = L^{-1}y
  LOGINFO("Solve L.rho = y");
  rho_ = covarianceCholeskyFactor_.solveLinearSystem(y);
  LOGINFO(OSS(false) << "rho_=" << rho_);
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = L^{-1}F
    LOGINFO("Solve L.Phi = F");
    LOGINFO(OSS(false) << "F_=\n" << F_);
    Matrix Phi(covarianceCholeskyFactor_.solveLinearSystem(F_));
    LOGINFO(OSS(false) << "Phi=\n" << Phi);
    LOGINFO("Solve min_beta||Phi.beta - rho||^2");
    beta_ = Phi.solveLinearSystem(rho_);
    LOGINFO(OSS(false) << "beta_=" << beta_);
    rho_ -= Phi * beta_;
    LOGINFO(OSS(false) << "rho_=" << rho_);
  }
  LOGINFO("Compute log(|det(L)|)=log(sqrt(|det(C)|))");
  NumericalScalar logDetL = 0.0;
  for ( UnsignedInteger i = 0; i < covarianceCholeskyFactor_.getDimension(); ++i )
  {
    const NumericalScalar lii = covarianceCholeskyFactor_(i, i);
    if (lii <= 0.0) return -SpecFunc::LogMaxNumericalScalar;
    logDetL += log(lii);
  }
  LOGINFO(OSS(false) << "-logDetL=" << -logDetL);
  return -logDetL;
}

NumericalScalar GeneralizedLinearModelAlgorithm::computeHMatLogDeterminantCholesky(const NumericalPoint & parameters) const
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = \sigma^2 * R(s,t) with R a correlation function
  LOGINFO(OSS(false) << "Compute the HMAT log-determinant of the Cholesky factor for parameters=" << parameters);
  CovarianceModel model(covarianceModel_);
  model.setParameter(parameters);

  Bool continuationCondition = true;
  const NumericalScalar startingScaling = ResourceMap::GetAsNumericalScalar("GeneralizedLinearModelAlgorithm-StartingScaling");
  const NumericalScalar maximalScaling = ResourceMap::GetAsNumericalScalar("GeneralizedLinearModelAlgorithm-MaximalScaling");
  NumericalScalar cumulatedScaling = 0.0;
  NumericalScalar scaling = startingScaling;
  const UnsignedInteger covarianceDimension = model.getDimension();

  HMatrixFactory hmatrixFactory;
  HMatrixParameters hmatrixParameters;

  while (continuationCondition && (cumulatedScaling < maximalScaling))
  {
    try
    {
      covarianceCholeskyFactorHMatrix_ = hmatrixFactory.build(normalizedInputSample_, covarianceDimension, true, hmatrixParameters);
      if (covarianceDimension == 1)
      {
        CovarianceAssemblyFunction simple(model, normalizedInputSample_, cumulatedScaling);
        covarianceCholeskyFactorHMatrix_.assemble(simple, 'L');
      }
      else
      {
        CovarianceBlockAssemblyFunction block(model, normalizedInputSample_, cumulatedScaling);
        covarianceCholeskyFactorHMatrix_.assemble(block, 'L');
      }
      // Factorize
      covarianceCholeskyFactorHMatrix_.factorize("LLt");
      continuationCondition = false;
    }
    // If it has not yet been computed, compute it and store it
    catch (InternalException &)
    {
      cumulatedScaling += scaling ;
      scaling *= 2.0;
      NumericalScalar assemblyEpsilon = hmatrixParameters.getAssemblyEpsilon() / 10.0;
      hmatrixParameters.setAssemblyEpsilon(assemblyEpsilon);
      NumericalScalar recompressionEpsilon = hmatrixParameters.getRecompressionEpsilon() / 10.0;
      hmatrixParameters.setRecompressionEpsilon(recompressionEpsilon);
      LOGDEBUG(OSS() <<  "Currently, scaling up to "  << cumulatedScaling << " to get an admissible covariance. Maybe compression & recompression factors are not adapted.");
      LOGDEBUG(OSS() <<  "Currently, assembly espilon = "  << assemblyEpsilon );
      LOGDEBUG(OSS() <<  "Currently, recompression epsilon "  <<  recompressionEpsilon);
    }
  }
  if (scaling >= maximalScaling)
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::computeHMatLogLikelihood, could not compute the Cholesky factor"
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");

  // y corresponds to output data
  // The PersistentCollection is returned as NumericalPoint with the right memory map
  NumericalPoint y(outputSample_.getImplementation()->getData());
  // rho = L^{-1}y
  LOGINFO("Solve L.rho = y");
  rho_ = covarianceCholeskyFactorHMatrix_.solveLower(y);
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = L^{-1}F
    LOGINFO("Solve L.Phi = F");
    Matrix Phi(covarianceCholeskyFactorHMatrix_.solveLower(F_));
    LOGINFO("Solve min_beta||Phi.beta - rho||^2");
    beta_ = Phi.solveLinearSystem(rho_);
    rho_ -= Phi * beta_;
  }
  LOGINFO("Compute log(sqrt(|det(C)|)) = log(|det(L)|)");
  NumericalScalar logDetL = 0.0;
  NumericalPoint diagonal(covarianceCholeskyFactorHMatrix_.getDiagonal());
  for ( UnsignedInteger i = 0; i < rho_.getSize(); ++i )
  {
    const NumericalScalar lii = diagonal[i];
    if (lii <= 0.0) return SpecFunc::MaxNumericalScalar;
    logDetL += log(lii);
  }
  return -logDetL;
}

NumericalPoint GeneralizedLinearModelAlgorithm::optimizeLogLikelihood()
{
  // initial guess
  const NumericalPoint initialParameters(covarianceModel_.getParameter());
  // We use the functional form of the log-likelihood computation to benefit from the cache mechanism
  NumericalMathFunction logLikelihoodFunction(getObjectiveFunction());
  const NumericalScalar initialLogLikelihood = logLikelihoodFunction(initialParameters)[0];
  LOGINFO(OSS() << "Initial parameters=" << initialParameters << ", log-likelihood=" << initialLogLikelihood);

  // Early exit if no parameter optimization
  if (!optimizeParameters_) return initialParameters;

  // Define Optimization problem
  OptimizationProblem problem;
  problem.setObjective(logLikelihoodFunction);
  problem.setMinimization(false);
  problem.setBounds(optimizationBounds_);
  solver_.setStartingPoint(initialParameters);
  solver_.setProblem(problem);
  solver_.run();

  // check result
  const NumericalScalar optimizedLogLikelihood = solver_.getResult().getOptimalValue()[0];
  const NumericalPoint optimizedParameters(solver_.getResult().getOptimalPoint());
  LOGINFO(OSS() << "Optimized parameters=" << optimizedParameters << ", log-likelihood=" << optimizedLogLikelihood);
  const NumericalPoint finalParameters(optimizedLogLikelihood > initialLogLikelihood ? optimizedParameters : initialParameters);
  // the last optimized point is not necessarily the last evaluated, so update intermediate results
  const NumericalScalar finalLogLikelihood = logLikelihoodFunction(finalParameters)[0];
  LOGINFO(OSS() << "Final parameters=" << finalParameters << ", log-likelihood=" << finalLogLikelihood);

  return finalParameters;
}

/** Optimization solver accessor */
OptimizationSolver GeneralizedLinearModelAlgorithm::getOptimizationSolver() const
{
  return solver_;
}

void GeneralizedLinearModelAlgorithm::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
  hasRun_ = false;
}

void GeneralizedLinearModelAlgorithm::setInputTransformation(const NumericalMathFunction & inputTransformation)
{
  if (inputTransformation.getInputDimension() != inputSample_.getDimension()) throw InvalidDimensionException(HERE)
        << "In GeneralizedLinearModelAlgorithm::setInputTransformation, input dimension of the transformation (" << inputTransformation.getInputDimension() << ") should match input sample dimension (" << inputSample_.getDimension() << ")";
  if (inputTransformation.getOutputDimension() != inputSample_.getDimension()) throw InvalidDimensionException(HERE)
        << "In GeneralizedLinearModelAlgorithm::setInputTransformation, output dimension of the transformation (" << inputTransformation.getOutputDimension() << ") should match output sample dimension (" << inputSample_.getDimension() << ")";
  inputTransformation_ = inputTransformation;
  // Set normalize to true
  normalize_ = true;
}

NumericalMathFunction GeneralizedLinearModelAlgorithm::getInputTransformation() const
{
  // If normlize is false, we return identity function
  if (!normalize_)
  {
    const UnsignedInteger dimension = inputSample_.getDimension();
    return LinearFunction(NumericalPoint(dimension), NumericalPoint(dimension), IdentityMatrix(dimension));
  }
  return inputTransformation_;
}

/* Optimize parameters flag accessor */
Bool GeneralizedLinearModelAlgorithm::getOptimizeParameters() const
{
  return optimizeParameters_;
}

void GeneralizedLinearModelAlgorithm::setOptimizeParameters(const Bool optimizeParameters)
{
  if (optimizeParameters != optimizeParameters_)
  {
    // some intermediate results depend on the covariance model
    hasRun_ = false;

    optimizeParameters_ = optimizeParameters;
  }
}

/* Accessor to optimization bounds */
void GeneralizedLinearModelAlgorithm::setOptimizationBounds(const Interval & optimizationBounds)
{
  optimizationBounds_ = optimizationBounds;
}

Interval GeneralizedLinearModelAlgorithm::getOptimizationBounds() const
{
  return optimizationBounds_;
}

/* Observation noise accessor */
void GeneralizedLinearModelAlgorithm::setNoise(const NumericalPoint & noise)
{
  const UnsignedInteger size = inputSample_.getSize();
  if (noise.getSize() != size) throw InvalidArgumentException(HERE) << "Noise size (" << noise.getSize()  << ") does not match sample size (" << size << ")";
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (!(noise[i] >= 0.0)) throw InvalidArgumentException(HERE) << "Noise must be positive";
  noise_ = noise;
}


NumericalPoint GeneralizedLinearModelAlgorithm::getNoise() const
{
  return noise_;
}


NumericalPoint GeneralizedLinearModelAlgorithm::getRho() const
{
  return rho_;
}

/* String converter */
String GeneralizedLinearModelAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << ", inputSample=" << inputSample_
      << ", outputSample=" << outputSample_
      << ", basis=" << basis_
      << ", covarianceModel=" << covarianceModel_
      << ", solver=" << solver_
      << ", optimizeParameters=" << optimizeParameters_
      << ", noise=" << noise_;
  return oss;
}


NumericalSample GeneralizedLinearModelAlgorithm::getInputSample() const
{
  return inputSample_;
}


NumericalSample GeneralizedLinearModelAlgorithm::getOutputSample() const
{
  return outputSample_;
}


GeneralizedLinearModelResult GeneralizedLinearModelAlgorithm::getResult()
{
  if (!hasRun_) run();
  return result_;
}


NumericalMathFunction GeneralizedLinearModelAlgorithm::getObjectiveFunction()
{
  LOGINFO("Normalizing the data...");
  normalizeInputSample();
  LOGINFO("Compute the design matrix");
  computeF();
  NumericalMathFunction logLikelihood(bindMethod <GeneralizedLinearModelAlgorithm, NumericalScalar, NumericalPoint> ( *this, &GeneralizedLinearModelAlgorithm::computeLogLikelihood, covarianceModel_.getParameter().getSize(), 1 ));
  // Here we change the finite difference gradient for a non centered one in order to reduce the computational cost
  logLikelihood.setGradient(NonCenteredFiniteDifferenceGradient(ResourceMap::GetAsNumericalScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon" ), logLikelihood.getEvaluation()).clone());
  logLikelihood.enableCache();
  return logLikelihood;
}

void GeneralizedLinearModelAlgorithm::initializeMethod()
{

  if (ResourceMap::Get("GeneralizedLinearModelAlgorithm-LinearAlgebra") == "HMAT")
    method_ = 1;
}

/** Method accessor (lapack/hmat) - Protected but friend with GeneralizedLinearModelAlgorithm class */
void GeneralizedLinearModelAlgorithm::setMethod(const UnsignedInteger method)
{
  method_ = method;
}

/* Method save() stores the object through the StorageManager */
void GeneralizedLinearModelAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "inputTransformation_", inputTransformation_ );
  adv.saveAttribute( "normalize_", normalize_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "solver_", solver_ );
  adv.saveAttribute( "optimizationBounds_", optimizationBounds_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "method", method_ );
  adv.saveAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.saveAttribute( "optimizeParameters_", optimizeParameters_ );
  adv.saveAttribute( "noise_", noise_ );
}


/* Method load() reloads the object from the StorageManager */
void GeneralizedLinearModelAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "inputTransformation_", inputTransformation_ );
  adv.loadAttribute( "normalize_", normalize_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "solver_", solver_ );
  adv.loadAttribute( "optimizationBounds_", optimizationBounds_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "method", method_ );
  adv.loadAttribute( "keepCholeskyFactor_", keepCholeskyFactor_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.loadAttribute( "optimizeParameters_", optimizeParameters_ );
  adv.loadAttribute( "noise_", noise_ );
}

END_NAMESPACE_OPENTURNS
