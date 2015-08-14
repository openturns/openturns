//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "KrigingAlgorithm.hxx"
#include "PersistentObjectFactory.hxx"
#include "LinearNumericalMathFunction.hxx"
#include "SpecFunc.hxx"
#include "ProductCovarianceModel.hxx"
#include "KrigingEvaluation.hxx"
#include "KrigingGradient.hxx"
#include "CenteredFiniteDifferenceGradient.hxx"
#include "CenteredFiniteDifferenceHessian.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "HMatrixFactory.hxx"
#include "NonCenteredFiniteDifferenceGradient.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingAlgorithm);

static Factory<KrigingAlgorithm> RegisteredFactory("KrigingAlgorithm");


/* Default constructor */
KrigingAlgorithm::KrigingAlgorithm()
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , normalizedInputSample_(0, 0)
  , inputTransformation_()
  , normalize_(false)
  , outputSample_(0, 0)
  , covarianceModel_()
  , optimizer_()
  , optimizerProvided_(false)
  , beta_(0)
  , gamma_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , keepCovariance_()
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
  , sigma2_()
{
  // Nothing to do
}


/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const Basis & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool normalize,
                                   const Bool keepCovariance)
  : MetaModelAlgorithm()
  , inputSample_()
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(normalize)
  , outputSample_()
  , covarianceModel_()
  , optimizer_()
  , optimizerProvided_(false)
  , beta_(0)
  , gamma_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , keepCovariance_(keepCovariance)
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
  , sigma2_()
{
  // set data & covariance model
  setDataAndCovariance(inputSample, outputSample, covarianceModel);
  if (basis.getSize() > 0)
  {
    // Set basis
    basis_ = BasisCollection(outputSample.getDimension(), basis);
    if (basis[0].getOutputDimension() > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but first function has dimension " << basis[0].getOutputDimension() << ". Only the first output component will be taken into account.");
    if (outputSample.getDimension() > 1) LOGWARN(OSS() << "The basis of functions will be applied to all output marginals" );
  }

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension(inputSample.getDimension());
    const NumericalPoint mean(inputSample.computeMean());
    const NumericalPoint stdev(inputSample.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (fabs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearNumericalMathFunction(mean, zero, linear));
  }
}


/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalSample & outputSample,
                                   const BasisCollection & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool normalize,
                                   const Bool keepCovariance)
  : MetaModelAlgorithm()
  , inputSample_(inputSample)
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(normalize)
  , outputSample_(outputSample)
  , covarianceModel_(covarianceModel)
  , optimizer_()
  , optimizerProvided_(false)
  , beta_(0)
  , gamma_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , keepCovariance_(keepCovariance)
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
  , sigma2_()
{
  // set data & covariance model
  setDataAndCovariance(inputSample, outputSample, covarianceModel);
  // Set basis collection
  if (basis.getSize() > 0)  setBasisCollection(basis);

  // Build a normalization function if needed
  if (normalize_)
  {
    const UnsignedInteger dimension(inputSample.getDimension());
    const NumericalPoint mean(inputSample.computeMean());
    const NumericalPoint stdev(inputSample.computeStandardDeviationPerComponent());
    SquareMatrix linear(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      linear(j, j) = 1.0;
      if (fabs(stdev[j]) > SpecFunc::MinNumericalScalar) linear(j, j) /= stdev[j];
    }
    const NumericalPoint zero(dimension);
    setInputTransformation(LinearNumericalMathFunction(mean, zero, linear));
  }
}

/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalMathFunction & inputTransformation,
                                   const NumericalSample & outputSample,
                                   const Basis & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool keepCovariance)
  : MetaModelAlgorithm()
  , inputSample_()
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(true)
  , outputSample_()
  , covarianceModel_()
  , optimizer_()
  , optimizerProvided_(false)
  , beta_(0)
  , gamma_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , keepCovariance_(keepCovariance)
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
  , sigma2_()
{
  // set data & covariance model
  setDataAndCovariance(inputSample, outputSample, covarianceModel);
  // basis setter
  if (basis.getSize() > 0)
  {
    // Set basis
    basis_ = BasisCollection(outputSample.getDimension(), basis);
    if (basis[0].getOutputDimension() > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but first function has dimension" << basis[0].getOutputDimension() << ". Only the first output component will be taken into account.");
    if (outputSample.getDimension() > 1) LOGWARN(OSS() << "The basis of functions will be applied to all output marginals" );
  }

  // use the isoprobabilistic transformation
  setInputTransformation(inputTransformation);
}

/* Constructor */
KrigingAlgorithm::KrigingAlgorithm(const NumericalSample & inputSample,
                                   const NumericalMathFunction & inputTransformation,
                                   const NumericalSample & outputSample,
                                   const BasisCollection & basis,
                                   const CovarianceModel & covarianceModel,
                                   const Bool keepCovariance)
  : MetaModelAlgorithm()
  , inputSample_()
  , normalizedInputSample_(0, inputSample.getDimension())
  , inputTransformation_()
  , normalize_(true)
  , outputSample_()
  , covarianceModel_()
  , optimizer_()
  , optimizerProvided_(false)
  , beta_(0)
  , gamma_(0)
  , rho_(0)
  , F_(0, 0)
  , result_()
  , keepCovariance_(keepCovariance)
  , basis_()
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
  , sigma2_()
{
  // set data & covariance model
  setDataAndCovariance(inputSample, outputSample, covarianceModel);
  // Set basis collection
  if (basis.getSize() > 0)  setBasisCollection(basis);
  // use the isoprobabilistic transformation
  setInputTransformation(inputTransformation);
}


/* set sample & covariance method */
void KrigingAlgorithm::setDataAndCovariance(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const CovarianceModel & covarianceModel)
{
  // Check the sample sizes
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::KrigingAlgorithm, input sample size (" << inputSample.getSize() << ") does not match output sample size (" << outputSample.getSize() << ").";
  // Set samples
  inputSample_ = inputSample;
  outputSample_ = outputSample;
  // Check the covariance model
  const UnsignedInteger dimension(inputSample.getDimension());
  if (dimension != covarianceModel.getSpatialDimension())
  {
    if (covarianceModel.getSpatialDimension() != 1) throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::KrigingAlgorithm, input sample dimension (" << dimension << ") does not match covariance model spatial dimension (" << covarianceModel.getSpatialDimension() << ").";
    else covarianceModel_ = ProductCovarianceModel(ProductCovarianceModel::CovarianceModelCollection(dimension, covarianceModel));
  }
  else covarianceModel_ = covarianceModel;
  // Multivariate kriging: output sample should have same dimension as
  //    covariance model in order to get the right discretization.
  //    Otherwise user could try kriging per marginal
  if (covarianceModel_.getDimension() != outputSample.getDimension())
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::KrigingAlgorithm, output sample dimension (" << outputSample.getDimension()  << ") does not match covariance model dimension (" << covarianceModel_.getDimension() << ")";

}

/** Set basis collection method */
void KrigingAlgorithm::setBasisCollection(const BasisCollection & basis)
{
  // If basis given, then its size should be the same as the output dimension (each marginal of multibasis is a basis that will be used for trend of the corresponding marginal.
  if (basis.getSize() != outputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::KrigingAlgorithm, output sample dimension (" << outputSample_.getDimension()  << ") does not match multi-basis dimension (" << basis.getSize() << ")";
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
    catch (InvalidArgumentException)
    {
      index += 1;
      continuationCondition = continuationCondition && index < basis.getSize();
    }
  }
  if (outputDimension == 0)
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::KrigingAlgorithm, basisCollection argument contains basis with empty collection of functions";
  if (outputDimension > 1) LOGWARN(OSS() << "Expected a basis of scalar functions, but some function has dimension" << outputDimension << ". Only the first output component will be taken into account.");
  // Everything is ok, we set the basis
  basis_ = basis;
}


/* Normalize the input sample */
void KrigingAlgorithm::normalizeInputSample()
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
void KrigingAlgorithm::computeF()
{
  // Nothing to do if the design matrix has already been computed
  if (F_.getNbRows() != 0) return;
  // No early exit based on the sample/basis size as F_ must be initialized with the correct dimensions
  // With a multivariate basis of size similar to output dimension, each ith-basis should be applied to elements
  // of corresponding marginal
  const UnsignedInteger outputDimension(outputSample_.getDimension());
  const UnsignedInteger sampleSize(normalizedInputSample_.getSize());
  const UnsignedInteger basisCollectionSize(basis_.getSize());
  UnsignedInteger totalSize(0);
  for (UnsignedInteger i = 0; i < basisCollectionSize; ++ i ) totalSize += basis_[i].getSize();
  // basis_ is of size 0 or outputDimension
  if (totalSize == 0) return;
  UnsignedInteger index = 0;
  // Compute F
  F_ = Matrix(sampleSize * outputDimension, totalSize);
  // if totalSize > 0, then basis_.getSize() should be equal to outputDimension
  // This is checked in KrigingAlgorithm::KrigingAlgorithm
  for (UnsignedInteger outputMarginal = 0; outputMarginal < outputDimension; ++ outputMarginal )
  {
    const Basis localBasis(basis_[outputMarginal]);
    const UnsignedInteger localBasisSize(localBasis.getSize());
    for (UnsignedInteger j = 0; j < localBasisSize; ++j, ++index )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const NumericalSample basisSample(localBasis[j](normalizedInputSample_));
      for (UnsignedInteger i = 0; i < sampleSize; ++i) F_(outputMarginal + i * outputDimension, index) = basisSample[i][0];
    }
  }
}


void KrigingAlgorithm::computeGamma()
{
  LOGINFO("Solve C^t.gamma = rho");
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT")
  {
    gamma_ = covarianceHMatrix_.solveLower(rho_, true);
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
  LOGINFO("normalize the data");
  normalizeInputSample();
  LOGINFO("Compute the design matrix");
  computeF();
  const UnsignedInteger outputDimension(outputSample_.getDimension());
  NumericalPointWithDescription covarianceModelScale;
  // Here we compute both the meta-parameters and the kriging coefficients
  if (optimizerProvided_ && outputDimension == 1)
  {
    LOGINFO("Optimize the parameter of the marginal covariance model");
    covarianceModelScale = optimizeLogLikelihood();
  }
  else
  {
    LOGINFO("Extract the parameter of the marginal covariance model and compute the log-likelihood");
    // Here we simply read the meta-scales
    covarianceModelScale = covarianceModel_.getScale();
    // And we compute the kriging coefficients
    computeLogLikelihood(covarianceModelScale);
  }
  // Covariance coefficients are computed once, ever if optimiser is fixed
  computeGamma();
  LOGINFO("Store the estimates");
  Collection<NumericalPoint> trendCoefficients(basis_.getSize());
  UnsignedInteger cumulatedSize(0);
  for (UnsignedInteger outputIndex = 0; outputIndex < basis_.getSize(); ++ outputIndex)
  {
    const UnsignedInteger localBasisSize(basis_[outputIndex].getSize());
    NumericalPoint beta_i(localBasisSize);
    for(UnsignedInteger basisElement = 0; basisElement < localBasisSize; ++ basisElement)
    {
      beta_i[basisElement] = beta_[cumulatedSize];
      cumulatedSize += 1;
    }
    trendCoefficients[outputIndex] = beta_i;
  }
  CovarianceModel conditionalCovarianceModel(covarianceModel_);
  conditionalCovarianceModel.setScale(covarianceModelScale);
  NumericalSample covarianceCoefficients(inputSample_.getSize(), outputDimension);
  covarianceCoefficients.getImplementation()->setData(gamma_);
  LOGINFO("Build the output meta-model");
  NumericalMathFunction metaModel;
  // beta is not used as gamma, api have changed since OT 1.5
  // We use directly the collection of points
  metaModel.setEvaluation(new KrigingEvaluation(basis_, normalizedInputSample_, conditionalCovarianceModel, trendCoefficients, covarianceCoefficients));
  metaModel.setGradient(new KrigingGradient(basis_, normalizedInputSample_, conditionalCovarianceModel, trendCoefficients, covarianceCoefficients));
  metaModel.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), metaModel.getEvaluation()));
  // First build the meta-model on the transformed data
  // Then add the transformation if needed
  if (normalize_) metaModel = NumericalMathFunction(metaModel, inputTransformation_);
  // compute residual, relative error
  const NumericalPoint outputVariance(outputSample_.computeVariance());
  const NumericalSample mY(metaModel(inputSample_));
  const NumericalPoint squaredResiduals((outputSample_ - mY).computeRawMoment(2));
  NumericalPoint residuals(outputDimension);
  NumericalPoint relativeErrors(outputDimension);

  const UnsignedInteger size(inputSample_.getSize());
  for ( UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex )
  {
    residuals[outputIndex] = sqrt(squaredResiduals[outputIndex] / size);
    relativeErrors[outputIndex] = squaredResiduals[outputIndex] / outputVariance[outputIndex];
  }
  if (keepCovariance_)
    result_ = KrigingResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis_, trendCoefficients, conditionalCovarianceModel, covarianceCoefficients, covarianceCholeskyFactor_, covarianceHMatrix_);
  else
    result_ = KrigingResult(inputSample_, outputSample_, metaModel, residuals, relativeErrors, basis_, trendCoefficients, conditionalCovarianceModel, covarianceCoefficients);
  // If normalize, set input transformation
  if (normalize_) result_.setTransformation(inputTransformation_);
  // Set sigma2
  result_.setSigma2(sigma2_);
}

NumericalScalar KrigingAlgorithm::computeLogLikelihood(const NumericalPoint & theta) const
{
  if (theta.getSize() != covarianceModel_.getScale().getSize())
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::computeLogLikelihood, could not compute likelihood,"
                                         << " covariance model requires an argument of size " << covarianceModel_.getScale().getSize()
                                         << " but here we got " << theta.getSize();
  if (ResourceMap::Get("KrigingAlgorithm-LinearAlgebra") == "HMAT")
    return computeHMatLogLikelihood(theta);
  return computeLapackLogLikelihood(theta);
}


NumericalScalar KrigingAlgorithm::computeLapackLogLikelihood(const NumericalPoint & theta) const
{
  LOGINFO(OSS(false) << "Compute the LAPACK log-likelihood for theta=" << theta);
  CovarianceModel model(covarianceModel_);
  model.setScale(theta);

  const UnsignedInteger size(inputSample_.getSize());

  LOGINFO("Discretize the covariance model into R");
  CovarianceMatrix R(model.discretize(normalizedInputSample_));
  LOGINFO("Compute the Cholesky factor C of R");
  Bool continuationCondition(true);
  const NumericalScalar startingScaling(ResourceMap::GetAsNumericalScalar("KrigingAlgorithm-StartingScaling"));
  const NumericalScalar maximalScaling(ResourceMap::GetAsNumericalScalar("KrigingAlgorithm-MaximalScaling"));
  NumericalScalar cumulatedScaling(0.0);
  NumericalScalar scaling(startingScaling);
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
    throw InvalidArgumentException(HERE) << "In KrigingAlgorithm::computeLapackLogLikelihood, could not compute the Cholesky factor."
                                         << " Scaling up to "  << cumulatedScaling << " was not enough";
  if (cumulatedScaling > 0.0)
    LOGWARN(OSS() <<  "Warning! Scaling up to "  << cumulatedScaling << " was needed in order to get an admissible covariance. ");

  // y correspond to output data
  // The PersistentCollection is returned as NumericalPoint with the right memory map
  NumericalPoint y(outputSample_.getImplementation()->getData());
  LOGINFO("Solve C.psi = y");
  NumericalPoint psi(covarianceCholeskyFactor_.solveLinearSystem(y));
  rho_ = NumericalPoint(psi);
  // If trend to estimate
  if (basis_.getSize() > 0)
  {
    // Phi = C^{-1}F
    LOGINFO("Solve C.Phi = F");
    Matrix Phi(covarianceCholeskyFactor_.solveLinearSystem(F_));

    Matrix G;
    LOGINFO("Decompose Phi = Q.G with G triangular");
    Matrix Q(Phi.computeQR(G));
    LOGINFO("Solve Q.b = psi taking into account the orthogonality of Q");
    // Computing b = Q^t * psi
    NumericalPoint b(Q.getImplementation()->genVectProd(psi, true));
    LOGINFO("Solve G.beta = b");
    beta_ = G.solveLinearSystem(b);

    LOGINFO("Compute rho = psi - Phi.beta");
    rho_ -= Phi * beta_;
  }
  // Solving covarianceCholeskyFactor_^t * gamma_ = rho
  // Arguments are keepIntact=true, matrix_lower=true & solving_transposed=true
  //gamma_ = covarianceCholeskyFactor_.getImplementation()->solveLinearSystemTri(rho, true, true, true);
  LOGINFO("Compute log(|det(R)|)");
  // For restreint likelihood, sigma should be computed by taking into account
  // a penalty term
  const NumericalScalar penaltySigma(size * model.getDimension() - F_.getNbColumns());
  sigma2_ = rho_.normSquare() / penaltySigma;
  if (sigma2_ <= 0) return SpecFunc::MaxNumericalScalar;
  NumericalScalar logDetR(0.0);
  for ( UnsignedInteger i = 0; i < covarianceCholeskyFactor_.getDimension(); ++ i )
  {
    const NumericalScalar cii(covarianceCholeskyFactor_(i, i));
    if (cii <= 0.0) return SpecFunc::MaxNumericalScalar;
    logDetR += log(cii);
  }
  const NumericalScalar logLikelihood(-(penaltySigma * log(sigma2_) + 2.0 * logDetR));
  LOGINFO(OSS(false) << "Compute the unbiased log-likelihood=" << logLikelihood);

  return logLikelihood;
}

NumericalScalar KrigingAlgorithm::computeHMatLogLikelihood(const NumericalPoint & theta) const
{
  LOGINFO(OSS(false) << "Compute the HMAT log-likelihood for theta=" << theta);
  CovarianceModel model(covarianceModel_);
  model.setScale(theta);

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
  //gamma_ = covarianceHMatrix_.solveLower(rho, true);

  // For restreint likelihood, sigma should be computed by taking into account
  // a penalty term
  const NumericalScalar penaltySigma(size * model.getDimension() - F_.getNbColumns());
  sigma2_ = rho_.normSquare() / penaltySigma;
  if (sigma2_ <= 0) return SpecFunc::MaxNumericalScalar;
  LOGINFO("Compute log(|det(R)|)");
  NumericalScalar logDetR(0.0);
  NumericalPoint diagonal(covarianceHMatrix_.getDiagonal());
  for ( UnsignedInteger i = 0; i < rho_.getSize(); ++ i )
  {
    const NumericalScalar cii(diagonal[i]);
    if (cii <= 0.0) return SpecFunc::MaxNumericalScalar;
    logDetR += log(cii);
  }
  const NumericalScalar logLikelihood(-(penaltySigma * log(sigma2_) + 2.0 * logDetR));
  LOGINFO(OSS(false) << "Compute the unbiased log-likelihood=" << logLikelihood);

  return logLikelihood;
}

NumericalPoint KrigingAlgorithm::optimizeLogLikelihood()
{
  // initial guess
  const NumericalPoint initialTheta(covarianceModel_.getScale());
  // We use the functional form of the log-likelihood computation to benefit from the cache mechanism
  NumericalMathFunction logLikelihoodFunction(getLogLikelihoodFunction());
  const NumericalScalar initialLogLikelihood(logLikelihoodFunction(initialTheta)[0]);
  LOGINFO(OSS() << "Initial theta=" << initialTheta << ", log-likelihood=" << initialLogLikelihood);

  BoundConstrainedAlgorithm optimizer(optimizer_);
  optimizer.setObjectiveFunction(logLikelihoodFunction);
  optimizer.setOptimizationProblem(BoundConstrainedAlgorithmImplementationResult::MAXIMIZATION);
  optimizer.setStartingPoint(initialTheta);
  optimizer.run();

  // check result
  const NumericalScalar optimizedLogLikelihood(optimizer.getResult().getOptimalValue());
  const NumericalPoint optimizedTheta(optimizer.getResult().getOptimizer());
  LOGINFO(OSS() << "Optimized theta=" << optimizedTheta << ", log-likelihood=" << optimizedLogLikelihood);
  const NumericalPoint finalTheta(optimizedLogLikelihood > initialLogLikelihood ? optimizedTheta : initialTheta);
  // the last optimized point is not necessarily the last evaluated, so update intermediate results
  const NumericalScalar finalLogLikelihood(logLikelihoodFunction(finalTheta)[0]);
  LOGINFO(OSS() << "Final theta=" << finalTheta << ", log-likelihood=" << finalLogLikelihood);

  return finalTheta;
}


void KrigingAlgorithm::setOptimizer(const BoundConstrainedAlgorithm& optimizer)
{
  optimizer_ = optimizer;
  optimizerProvided_ = true;
}


BoundConstrainedAlgorithm KrigingAlgorithm::getOptimizer() const
{
  return optimizer_;
}


void KrigingAlgorithm::setInputTransformation(const NumericalMathFunction& inputTransformation)
{
  if (inputTransformation.getInputDimension() != inputSample_.getDimension()) throw InvalidDimensionException(HERE)
        << "In KrigingAlgorithm::setInputTransformation, input dimension of the transformation (" << inputTransformation.getInputDimension() << ") should match input sample dimension (" << inputSample_.getDimension() << ")";
  if (inputTransformation.getOutputDimension() != inputSample_.getDimension()) throw InvalidDimensionException(HERE)
        << "In KrigingAlgorithm::setInputTransformation, output dimension of the transformation (" << inputTransformation.getOutputDimension() << ") should match output sample dimension (" << inputSample_.getDimension() << ")";
  inputTransformation_ = inputTransformation;
}

NumericalMathFunction KrigingAlgorithm::getInputTransformation() const
{
  if (!normalize_)
  {
    const UnsignedInteger dimension(inputSample_.getDimension());
    return LinearNumericalMathFunction(NumericalPoint(dimension), NumericalPoint(dimension), IdentityMatrix(dimension));
  }
  return inputTransformation_;
}


/* Virtual constructor */
KrigingAlgorithm * KrigingAlgorithm::clone() const
{
  return new KrigingAlgorithm(*this);
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
  return outputSample_;;
}


KrigingResult KrigingAlgorithm::getResult()
{
  return result_;
}


NumericalMathFunction KrigingAlgorithm::getLogLikelihoodFunction()
{
  LOGINFO("normalize the data");
  normalizeInputSample();
  LOGINFO("Compute the design matrix");
  computeF();
  NumericalMathFunction logLikelihood(bindMethod <KrigingAlgorithm, NumericalScalar, NumericalPoint> ( *this, &KrigingAlgorithm::computeLogLikelihood, covarianceModel_.getScale().getSize(), 1 ));
  // Here we change the finite difference gradient for a non centered one in order to reduce the computational cost
  logLikelihood.setGradient(NonCenteredFiniteDifferenceGradient(ResourceMap::GetAsNumericalScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon" ), logLikelihood.getEvaluation()).clone());
  logLikelihood.enableCache();
  return logLikelihood;
}


/* Method save() stores the object through the StorageManager */
void KrigingAlgorithm::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "inputTransformation_", inputTransformation_ );
  adv.saveAttribute( "normalize_", normalize_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "optimizer_", optimizer_ );
  adv.saveAttribute( "optimizerProvided_", optimizerProvided_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "keepCovariance_", keepCovariance_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.saveAttribute( "sigma2_", sigma2_ );
}


/* Method load() reloads the object from the StorageManager */
void KrigingAlgorithm::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "inputTransformation_", inputTransformation_ );
  adv.loadAttribute( "normalize_", normalize_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "optimizer_", optimizer_ );
  adv.loadAttribute( "optimizerProvided_", optimizerProvided_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "keepCovariance_", keepCovariance_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_ );
  adv.loadAttribute( "sigma2_", sigma2_ );
}

END_NAMESPACE_OPENTURNS
