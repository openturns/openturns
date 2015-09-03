//                                               -*- C++ -*-
/**
 *  @brief The class builds generalized linear models
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "GeneralizedLinearModelAlgorithm.hxx"
#include "PersistentObjectFactory.hxx"
#include "HMatrixFactory.hxx"
#include "ProductCovarianceModel.hxx"
#include "TensorizedCovarianceModel.hxx"
#include "Log.hxx"
#include "SpecFunc.hxx"
#include "LinearNumericalMathFunction.hxx"
#include "CenteredFiniteDifferenceHessian.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "NonCenteredFiniteDifferenceGradient.hxx"
#include "TNC.hxx"
#include "NLopt.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedLinearModelAlgorithm);

static Factory<GeneralizedLinearModelAlgorithm> RegisteredFactory("GeneralizedLinearModelAlgorithm");

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
  , covarianceHMatrix_()
  , isEnabledKeepCovariance_(false)
  , method_(0)
  , hasRun_(false)
{
  // Nothing to do
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                                                  const NumericalSample & outputSample,
                                                                  const CovarianceModel & covarianceModel,
                                                                  const Bool normalize)
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
  , covarianceHMatrix_()
  , isEnabledKeepCovariance_(false)
  , method_(0)
  , hasRun_(false)
{
  // set data & covariance model
  setData(inputSample, outputSample);
  // If no basis then we suppose output sample centered
  checkYCentered(outputSample);
  setCovariance(covarianceModel);

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension(inputSample_.getDimension());
    const NumericalPoint mean(inputSample_.computeMean());
    const NumericalPoint stdev(inputSample_.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (fabs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearNumericalMathFunction(mean, zero, linear));
  }
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                                                  const NumericalSample & outputSample,
                                                                  const CovarianceModel & covarianceModel,
                                                                  const Basis & basis,
                                                                  const Bool normalize)
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
  , covarianceHMatrix_()
  , isEnabledKeepCovariance_(false)
  , method_(0)
  , hasRun_(false)
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
    const UnsignedInteger dimension(inputSample_.getDimension());
    const NumericalPoint mean(inputSample_.computeMean());
    const NumericalPoint stdev(inputSample_.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (fabs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearNumericalMathFunction(mean, zero, linear));
  }
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                                                  const NumericalMathFunction & inputTransformation,
                                                                  const NumericalSample & outputSample,
                                                                  const CovarianceModel & covarianceModel,
                                                                  const Basis & basis)
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
  , covarianceHMatrix_()
  , isEnabledKeepCovariance_(false)
  , method_(0)
  , hasRun_(false)
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
                                                                  const Bool normalize)
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
  , covarianceHMatrix_()
  , isEnabledKeepCovariance_(false)
  , method_(0)
  , hasRun_(false)
{
  // set data & covariance model
  setData(inputSample, outputSample);
  setCovariance(covarianceModel);

  // Set basis collection
  if (multivariateBasis.getSize() > 0) setBasis(multivariateBasis);

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension(inputSample_.getDimension());
    const NumericalPoint mean(inputSample_.computeMean());
    const NumericalPoint stdev(inputSample_.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (fabs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearNumericalMathFunction(mean, zero, linear));
  }
  initializeMethod();
  initializeDefaultOptimizationSolver();
}

/** Parameters constructor */
GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm (const NumericalSample & inputSample,
                                                                  const NumericalMathFunction & inputTransformation,
                                                                  const NumericalSample & outputSample,
                                                                  const CovarianceModel & covarianceModel,
                                                                  const BasisCollection & multivariateBasis)
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
  , covarianceHMatrix_()
  , isEnabledKeepCovariance_(false)
  , method_(0)
  , hasRun_(false)
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
  Bool continuationCondition(true);
  UnsignedInteger index(0);
  UnsignedInteger outputDimension(0);
  while(continuationCondition)
  {
    try
    {
      outputDimension =  basis[index][0].getOutputDimension();
      continuationCondition = false;
    }
    catch (InvalidArgumentException & ex)
    {
      index += 1;
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
  const NumericalPoint meanY = Y.computeMean();
  for (UnsignedInteger k = 0; k < meanY.getDimension(); ++k)
  {
    if (abs(meanY[k]) > meanEpsilon)
      throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm, basis is empty and output sample is not centered";
  }
}

void GeneralizedLinearModelAlgorithm::initializeDefaultOptimizationSolver()
{
  if (ResourceMap::Get("GeneralizedLinearModelAlgorithm-DefaultOptimizationSolver") == "TNC")
    solver_ = TNC();
  else if (ResourceMap::Get("GeneralizedLinearModelAlgorithm-DefaultOptimizationSolver") == "NELDER-MEAD")
    solver_ = NelderMead();
  else if (ResourceMap::Get("GeneralizedLinearModelAlgorithm-DefaultOptimizationSolver") == "LBFGS")
    solver_ = LBFGS();
  else
    throw InvalidArgumentException(HERE) << "Uknown optimization solver";
  // Define Optimization Problem
  // Default problem takes into account the bounds and thus determine the dimension
  OptimizationProblem problem;
  // Bounds should be of size spatialDimension + dimension
  const UnsignedInteger optimizationProblemSize = covarianceModel_.getParameter().getSize();
  const NumericalPoint lowerBound(optimizationProblemSize, ResourceMap::GetAsNumericalScalar( "GeneralizedLinearModelAlgorithm-DefaultOptimizationLowerBound"));
  const NumericalPoint upperBound(optimizationProblemSize, ResourceMap::GetAsNumericalScalar( "GeneralizedLinearModelAlgorithm-DefaultOptimizationUpperBound"));
  Interval bounds(lowerBound, upperBound);
  problem.setBounds(bounds);
  solver_.setProblem(problem);
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
  if (basis_.getSize() == 0) return;
  // No early exit based on the sample/basis size as F_ must be initialized with the correct dimensions
  // With a multivariate basis of size similar to output dimension, each ith-basis should be applied to elements
  // of corresponding marginal
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  const UnsignedInteger sampleSize = normalizedInputSample_.getSize();
  const UnsignedInteger basisCollectionSize = basis_.getSize();
  UnsignedInteger totalSize = 0;
  for (UnsignedInteger i = 0; i < basisCollectionSize; ++ i ) totalSize += basis_[i].getSize();
  UnsignedInteger index = 0;
  // Compute F
  F_ = Matrix(sampleSize * outputDimension, totalSize);
  // if totalSize > 0, then basis_.getSize() should be equal to outputDimension
  // This is checked in GeneralizedLinearModelAlgorithm::GeneralizedLinearModelAlgorithm
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
  // todo
}

NumericalScalar GeneralizedLinearModelAlgorithm::computeLogLikelihood(const NumericalPoint & parameters) const
{
  if (parameters.getSize() != covarianceModel_.getParameter().getSize())
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::computeLogLikelihood, could not compute likelihood,"
                                         << " covariance model requires an argument of size " << covarianceModel_.getParameter().getSize()
                                         << " but here we got " << parameters.getSize();
  NumericalScalar logLikelihood;
  if (method_ == 1)
    logLikelihood = computeHMatLogLikelihood(parameters);
  else
    logLikelihood = computeLapackLogLikelihood(parameters);
  // The lapack/hmat implementation computes :
  // 1) The log-determinant of the covariance matrix (log inverse). This is returned by the method
  // 2) rho_ : the point is updated
  // The next step is to compute the norm of this last one and added
  // rho is the residual choleskyFactor * Y-F*beta
  const NumericalScalar epsilon = rho_.normSquare();
  if (epsilon <= 0) return SpecFunc::MaxNumericalScalar;
  // For general case, we could not except a restricted log-likelihood
  // We use general expression of log-likelihood
  logLikelihood -= epsilon;
  LOGINFO(OSS(false) << "Compute the estimated log-likelihood=" << logLikelihood);
  return logLikelihood / outputSample_.getSize();
}


NumericalScalar GeneralizedLinearModelAlgorithm::computeLapackLogLikelihood(const NumericalPoint & parameters) const
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = diag(sigma) * R(s,t) diag(sigma) with R a correlation function
  LOGINFO(OSS(false) << "Compute the LAPACK log-likelihood for theta=" << parameters);
  CovarianceModel model(covarianceModel_);
  model.setParameter(parameters);

  const UnsignedInteger size = inputSample_.getSize();

  LOGINFO("Discretize the covariance model...");
  CovarianceMatrix R = model.discretize(normalizedInputSample_);
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
      covarianceCholeskyFactor_ = R.computeCholesky();
      continuationCondition = false;
    }
    // If it has not yet been computed, compute it and store it
    catch (InternalException &)
    {
      cumulatedScaling += scaling ;
      // Unroll the regularization to optimize the computation
      for (UnsignedInteger i = 0; i < R.getDimension(); ++i) R(i, i) += scaling;
      scaling *= 2.0;
    }
  }
  if (scaling >= maximalScaling)
    throw InvalidArgumentException(HERE) << "In GeneralizedLinearModelAlgorithm::computeLapackLogLikelihood, could not compute the Cholesky factor."
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");

  // y correspond to output data
  NumericalPoint y = outputSample_.getImplementation()->getData();
  LOGINFO("Solve C.psi = y");
  NumericalPoint psi = covarianceCholeskyFactor_.solveLinearSystem(y);
  rho_ = NumericalPoint(psi);
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = C^{-1}F
    LOGINFO("Solve C.Phi = F");
    Matrix Phi = covarianceCholeskyFactor_.solveLinearSystem(F_);

    Matrix G;
    LOGINFO("Decompose Phi = Q.G with G triangular");
    Matrix Q = Phi.computeQR(G);
    LOGINFO("Solve Q.b = psi taking into account the orthogonality of Q");
    // Computing b = Q^t * psi
    NumericalPoint b = Q.getImplementation()->genVectProd(psi, true);
    LOGINFO("Solve G.beta = b");
    beta_ = G.solveLinearSystem(b);

    LOGINFO("Compute rho = psi - Phi.beta");
    rho_ -= Phi * beta_;
  }
  LOGINFO("Compute log(|det(R)|)");
  NumericalScalar logDetR(0.0);
  for ( UnsignedInteger i = 0; i < covarianceCholeskyFactor_.getDimension(); ++i )
  {
    const NumericalScalar cii = covarianceCholeskyFactor_(i, i);
    if (cii <= 0.0) return SpecFunc::MaxNumericalScalar;
    logDetR += log(cii);
  }
  return -2.0 * logDetR;
}

NumericalScalar GeneralizedLinearModelAlgorithm::computeHMatLogLikelihood(const NumericalPoint & parameters) const
{
  // Using the hypothesis that parameters = scale & model writes : C(s,t) = \sigma^2 * R(s,t) with R a correlation function
  LOGINFO(OSS(false) << "Compute the HMAT log-likelihood for parameters=" << parameters);
  CovarianceModel model(covarianceModel_);
  model.setParameter(parameters);

  const UnsignedInteger size(inputSample_.getSize());

  Bool continuationCondition(true);
  const NumericalScalar startingScaling(ResourceMap::GetAsNumericalScalar("KrigingAlgorithm-StartingScaling"));
  const NumericalScalar maximalScaling(ResourceMap::GetAsNumericalScalar("KrigingAlgorithm-MaximalScaling"));
  NumericalScalar cumulatedScaling(0.0);
  NumericalScalar scaling(startingScaling);
  const UnsignedInteger covarianceDimension(model.getDimension());

  HMatrixFactory hmatrixFactory;
  NumericalScalar assemblyEpsilon = ResourceMap::GetAsNumericalScalar("HMatrix-AssemblyEpsilon");
  NumericalScalar recompressionEpsilon = ResourceMap::GetAsNumericalScalar("HMatrix-RecompressionEpsilon");

  while (continuationCondition && (cumulatedScaling < maximalScaling))
  {
    try
    {
      covarianceHMatrix_ = hmatrixFactory.build(normalizedInputSample_, covarianceDimension, true);
      covarianceHMatrix_.getImplementation()->setKey("assembly-epsilon", OSS() << assemblyEpsilon);
      covarianceHMatrix_.getImplementation()->setKey("recompression-epsilon", OSS() << recompressionEpsilon);
      if (covarianceDimension == 1)
      {
        CovarianceAssemblyFunction simple(model, normalizedInputSample_, cumulatedScaling);
        covarianceHMatrix_.assemble(simple, 'L');
      }
      else
      {
        CovarianceBlockAssemblyFunction block(model, normalizedInputSample_, cumulatedScaling);
        covarianceHMatrix_.assemble(block, 'L');
      }
      // Factorize
      covarianceHMatrix_.factorize("LLt");
      continuationCondition = false;
    }
    // If it has not yet been computed, compute it and store it
    catch (InternalException &)
    {
      cumulatedScaling += scaling ;
      scaling *= 2.0;
      assemblyEpsilon /= 10.0 ;
      recompressionEpsilon /= 10.0;
      LOGDEBUG(OSS() <<  "Currently, scaling up to "  << cumulatedScaling << " to get an admissible covariance. Maybe compression & recompression factors are not adapted.");
      LOGDEBUG(OSS() <<  "Currently, assembly espilon = "  << assemblyEpsilon );
      LOGDEBUG(OSS() <<  "Currently, recompression epsilon "  <<  recompressionEpsilon);
    }
  }
  if (scaling >= maximalScaling)
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::computeHMatLogLikelihood, could not compute the Cholesky factor"
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");

  // y correspond to output data
  // The PersistentCollection is returned as NumericalPoint with the right memory map
  NumericalPoint y(outputSample_.getImplementation()->getData());
  LOGINFO("Solve C.psi = y");
  NumericalPoint psi(covarianceHMatrix_.solveLower(y));
  rho_ = psi;
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = C^{-1}F
    LOGINFO("Solve C.Phi = F");
    Matrix Phi(covarianceHMatrix_.solveLower(F_));

    Matrix G;
    LOGINFO("Decompose Phi = Q.G with G triangular");
    Matrix Q(Phi.computeQR(G));
    LOGINFO("Solve Q.b = psi taking into account the orthogonality of Q");
    NumericalPoint b(Q.transpose() * psi);
    LOGINFO("Solve G.beta = b");
    beta_ = G.solveLinearSystem(b);

    LOGINFO("Compute rho = psi - Phi.beta");
    rho_ -= Phi * beta_;
  }
  LOGINFO("Compute log(|det(R)|)");
  NumericalScalar logDetR(0.0);
  NumericalPoint diagonal(covarianceHMatrix_.getDiagonal());
  for ( UnsignedInteger i = 0; i < rho_.getSize(); ++i )
  {
    const NumericalScalar cii(diagonal[i]);
    if (cii <= 0.0) return SpecFunc::MaxNumericalScalar;
    logDetR += log(cii);
  }
  return -2.0 * logDetR;
}

NumericalPoint GeneralizedLinearModelAlgorithm::optimizeLogLikelihood()
{
  // initial guess
  const NumericalPoint initialParameters = covarianceModel_.getParameter();
  // We use the functional form of the log-likelihood computation to benefit from the cache mechanism
  NumericalMathFunction logLikelihoodFunction = getObjectiveFunction();
  const NumericalScalar initialLogLikelihood = logLikelihoodFunction(initialParameters)[0];
  LOGINFO(OSS() << "Initial parameters=" << initialParameters << ", log-likelihood=" << initialLogLikelihood);

  // Define Optimization problem
  OptimizationProblem problem = solver_.getProblem();
  problem.setObjective(logLikelihoodFunction);
  problem.setMinimization(false);
  solver_.setStartingPoint(initialParameters);
  solver_.setProblem(problem);
  solver_.run();

  // check result
  const NumericalScalar optimizedLogLikelihood = solver_.getResult().getOptimalValue()[0];
  const NumericalPoint optimizedParameters = solver_.getResult().getOptimalPoint();
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
  Interval bounds = solver.getProblem().getBounds();
  const UnsignedInteger optimizationProblemSize = covarianceModel_.getParameter().getSize();
  OptimizationProblem problem;
  if (bounds.getDimension() != optimizationProblemSize)
  {
    problem = solver_.getProblem();
    solver_ = solver;
    solver_.setProblem(problem);
  }
  else
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
    const UnsignedInteger dimension(inputSample_.getDimension());
    return LinearNumericalMathFunction(NumericalPoint(dimension), NumericalPoint(dimension), IdentityMatrix(dimension));
  }
  return inputTransformation_;
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
      << ", solver=" << solver_;
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

Bool GeneralizedLinearModelAlgorithm::isEnabledKeepCovariance() const
{
  return isEnabledKeepCovariance_;
}

/** keep covariance */
void GeneralizedLinearModelAlgorithm::enableKeepCovariance() const
{
  isEnabledKeepCovariance_ = true;
}

void GeneralizedLinearModelAlgorithm::disableKeepCovariance() const
{
  isEnabledKeepCovariance_ = false;
}

void GeneralizedLinearModelAlgorithm::initializeMethod()
{

  if (ResourceMap::Get("GeneralizedLinearModelAlgorithm-LinearAlgebra") == "HMAT")
    method_ = 1;
}

/** Method accessor (lapack/hmat) - Protected but freind with KrigingAlgo class */
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
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "method", method_ );
  adv.saveAttribute( "isEnabledKeepCovariance_", isEnabledKeepCovariance_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
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
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "method", method_ );
  adv.loadAttribute( "isEnabledKeepCovariance_", isEnabledKeepCovariance_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
}

END_NAMESPACE_OPENTURNS
