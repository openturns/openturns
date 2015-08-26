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
  // todo
  return 0.0;
}


NumericalScalar GeneralizedLinearModelAlgorithm::computeLapackLogLikelihood(const NumericalPoint & parameters) const
{
  // todo
  return 0.0;
}

NumericalScalar GeneralizedLinearModelAlgorithm::computeHMatLogLikelihood(const NumericalPoint & parameters) const
{
  // todo
  return 0.0;
}

NumericalPoint GeneralizedLinearModelAlgorithm::optimizeLogLikelihood()
{
  // todo
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
  // todo
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
