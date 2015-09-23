//                                               -*- C++ -*-
/**
 *  @brief The result of a kriging estimation
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
#include "KrigingResult.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingResult);
static const Factory<KrigingResult> RegisteredFactory;

/* Default constructor */
KrigingResult::KrigingResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters & Cholesky factor */
KrigingResult::KrigingResult(const NumericalSample & inputSample,
                             const NumericalSample & outputSample,
                             const NumericalMathFunction & metaModel,
                             const NumericalPoint & residuals,
                             const NumericalPoint & relativeErrors,
                             const BasisCollection & basis,
                             const NumericalPointCollection & trendCoefficients,
                             const CovarianceModel & covarianceModel,
                             const NumericalSample & covarianceCoefficients)
  : MetaModelResult(NumericalMathFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputData_(inputSample)
  , inputTransformedData_(inputSample)
  , inputTransformation_()
  , hasTransformation_(false)
  , basis_(basis)
  , trendCoefficients_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , covarianceCoefficients_(covarianceCoefficients)
  , hasCholeskyFactor_(false)
  , covarianceCholeskyFactor_()
  , covarianceHMatrix_()
  , sigma2_(1.0)
  , F_()
  , phiT_()
  , Gt_()
{
  const UnsignedInteger size(inputSample.getSize());
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In KrigingResult::KrigingResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
}


/* Constructor with parameters & Cholesky factor */
KrigingResult::KrigingResult(const NumericalSample & inputSample,
                             const NumericalSample & outputSample,
                             const NumericalMathFunction & metaModel,
                             const NumericalPoint & residuals,
                             const NumericalPoint & relativeErrors,
                             const BasisCollection & basis,
                             const NumericalPointCollection & trendCoefficients,
                             const CovarianceModel & covarianceModel,
                             const NumericalSample & covarianceCoefficients,
                             const TriangularMatrix & covarianceCholeskyFactor,
                             const HMatrix & covarianceHMatrix)
  : MetaModelResult(NumericalMathFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputData_(inputSample)
  , inputTransformedData_(inputSample)
  , inputTransformation_()
  , hasTransformation_(false)
  , basis_(basis)
  , trendCoefficients_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , covarianceCoefficients_(covarianceCoefficients)
  , hasCholeskyFactor_(true)
  , covarianceCholeskyFactor_(covarianceCholeskyFactor)
  , covarianceHMatrix_(covarianceHMatrix)
  , sigma2_(1.0)
  , F_()
  , phiT_()
  , Gt_()
{
  const UnsignedInteger outputDimension(outputSample.getDimension());
  const UnsignedInteger size(inputSample.getSize());
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In KrigingResult::KrigingResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
  if (covarianceCholeskyFactor_.getDimension() != 0 && covarianceCholeskyFactor_.getDimension() != size * outputDimension)
    throw InvalidArgumentException(HERE) << "In KrigingResult::KrigingResult, Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceCholeskyFactor_.getDimension();
  if (covarianceHMatrix_.getNbRows() != 0)
  {
    if (covarianceHMatrix_.getNbRows() != covarianceHMatrix_.getNbColumns())
      throw InvalidArgumentException(HERE) << "In KrigingResult::KrigingResult, HMAT Cholesky factor is not square. Its dimension is " << covarianceHMatrix_.getNbRows() << "x" << covarianceHMatrix_.getNbColumns();
    if (covarianceHMatrix_.getNbRows() != size * outputDimension)
      throw InvalidArgumentException(HERE) << "In KrigingResult::KrigingResult, HMAT Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceHMatrix_.getNbRows();
  }
}


/* Virtual constructor */
KrigingResult * KrigingResult::clone() const
{
  return new KrigingResult(*this);
}


/* String converter */
String KrigingResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", basis=" << basis_
         << ", trend coefficients=" << trendCoefficients_
         << ", covariance models=" << covarianceModel_
         << ", covariance coefficients=" << covarianceCoefficients_;
}

String KrigingResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  oss << "covariance models=" << covarianceModel_;
  oss << ", covariance coefficients=" << covarianceCoefficients_;
  oss << ", basis=" << basis_;
  oss << ", trend coefficients=" << trendCoefficients_ << ")";
  return oss;
}

/* Basis accessor */
KrigingResult::BasisCollection KrigingResult::getBasisCollection() const
{
  return basis_;
}

/* Trend coefficients accessor */
KrigingResult::NumericalPointCollection KrigingResult::getTrendCoefficients() const
{
  return trendCoefficients_;
}

/* Covariance models accessor */
CovarianceModel KrigingResult::getCovarianceModel() const
{
  return covarianceModel_;
}

/* Covariance coefficients accessor */
NumericalSample KrigingResult::getCovarianceCoefficients() const
{
  return covarianceCoefficients_;
}

NumericalMathFunction KrigingResult::getTransformation() const
{
  return inputTransformation_;
}

void KrigingResult::setTransformation(const NumericalMathFunction & transformation)
{
  if (transformation.getInputDimension() != inputData_.getDimension())
    throw InvalidArgumentException(HERE) << "In KrigingResult::setTransformation, incompatible function dimension. Function should have input dimension = " << inputData_.getDimension() << ". Here, function's input dimension = " << transformation.getInputDimension();
  inputTransformation_ = transformation;
  // Map inputData using the transformation
  inputTransformedData_ = transformation(inputData_);
  hasTransformation_ = true;
}

/** Sigma2 accessor */
NumericalScalar KrigingResult::getSigma2() const
{
  return sigma2_;
}

void KrigingResult::setSigma2(const NumericalScalar & sigma2)
{
  if (sigma2_ < 0)
    throw InvalidArgumentException(HERE) << "in KrigingResult::setSigma2, sigma2 should be > 0";
  sigma2_ = sigma2;
}

/* Compute mean of new points conditionnaly to observations */
NumericalPoint KrigingResult::getConditionalMean(const NumericalSample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned matrix should have dimensions (p * s) x (p * s)
  const UnsignedInteger spatialDimension(xi.getDimension());
  if (spatialDimension != covarianceModel_.getSpatialDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getMean, input data should have the same dimension as covariance model's spatial dimension. Here, (input dimension = " << spatialDimension << ", covariance model spatial's dimension = " << covarianceModel_.getSpatialDimension() << ")";
  const UnsignedInteger sampleSize(xi.getSize());
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMean, expected a non empty sample";
  // Use of metamodel to return result
  // Need to think if it is required to reimplement a specific method
  // in order to avoid data copy
  // sample is of size xi.getSize() * covarianceModel.getDimension()
  NumericalSample output = metaModel_.operator()(xi);
  // Result is a NumericalPoint ==> data are transformed form NumericalSample to
  // NumericalPoint by using a copy
  NumericalPoint mean(output.getImplementation()->getData());
  return mean;
}

/* Compute mean of new points conditionnaly to observations */
NumericalPoint KrigingResult::getConditionalMean(const NumericalPoint & xi) const
{
  // Use of meta model evaluation
  // For NumericalPoint, no problematic of copy
  const NumericalPoint output = metaModel_.operator()(xi);
  return output;
}

struct KrigingResultCrossCovarianceFunctor
{
  const NumericalSample & conditionnedPoints_;
  const NumericalSample & input_;
  Matrix & output_;
  const CovarianceModel & model_;
  const UnsignedInteger dimension_;

  KrigingResultCrossCovarianceFunctor(const NumericalSample & conditionnedPoints,
                                      const NumericalSample & input,
                                      Matrix & output,
                                      const CovarianceModel & model)
    : conditionnedPoints_(conditionnedPoints)
    , input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      // Fill by column
      // jLocal ==> which column to fill
      // jBase : use of block size to determine first element of matrix
      // iLocal : for a fixed jLocal row, which iLocal-th element to fill
      // iBase : same as jBase but for rows
      const UnsignedInteger jLocal(i / conditionnedPoints_.getSize());
      const UnsignedInteger jBase(jLocal * dimension_);
      const UnsignedInteger iLocal(i - jLocal * conditionnedPoints_.getSize());
      const UnsignedInteger iBase(iLocal * dimension_);
      // Local covariance matrix
      const CovarianceMatrix localCovariance(model_(conditionnedPoints_[iLocal], input_[jLocal]));
      for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
      {
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
        {
          output_(iBase + ii, jBase + jj) = localCovariance(ii, jj);
        }
      }
    }
  }
};
/* end struct KrigingResultCrossCovarianceFunctor */


/* Compute cross matrix method ==> not necessary square matrix  */
Matrix KrigingResult::getCrossMatrix(const NumericalSample & x) const
{
  // Use of TBB structures
  // The idea is that we work by blocks
  // the (i,j) block corresponds to the interaction x[i], inputData[j]
  // Each block is of size d x d
  // So we have trainingSize * sampleSize blocks
  // We fill the matrix by columns
  const UnsignedInteger trainingSize(inputData_.getSize());
  const UnsignedInteger trainingFullSize(trainingSize * covarianceModel_.getDimension());
  const UnsignedInteger sampleSize(x.getSize());
  const UnsignedInteger sampleFullSize(sampleSize * covarianceModel_.getDimension());
  Matrix result(trainingFullSize, sampleFullSize);
  const KrigingResultCrossCovarianceFunctor policy( inputTransformedData_, x, result, covarianceModel_);
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, trainingSize * sampleSize, policy );
  return result;
}

/* Compute cross matrix method ==> not necessary square matrix  */
void KrigingResult::computeF() const
{
  // Nothing to do if the design matrix has already been computed
  if (F_.getNbRows() != 0) return;
  const UnsignedInteger outputDimension(covarianceModel_.getDimension());
  const UnsignedInteger sampleSize(inputData_.getSize());
  const UnsignedInteger basisCollectionSize(basis_.getSize());
  UnsignedInteger totalSize(0);
  for (UnsignedInteger i = 0; i < basisCollectionSize; ++ i ) totalSize += basis_[i].getSize();
  // basis_ is of size 0 or outputDimension
  if (totalSize == 0) return;
  UnsignedInteger index = 0;
  // Compute F
  F_ = Matrix(sampleSize * outputDimension, totalSize);
  for (UnsignedInteger outputMarginal = 0; outputMarginal < basisCollectionSize; ++ outputMarginal )
  {
    const Basis localBasis(basis_[outputMarginal]);
    const UnsignedInteger localBasisSize(localBasis.getSize());
    for (UnsignedInteger j = 0; j < localBasisSize; ++j, ++index )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const NumericalSample basisSample(localBasis[j](inputTransformedData_));
      for (UnsignedInteger i = 0; i < sampleSize; ++i) F_(outputMarginal + i * outputDimension, index) = basisSample[i][0];
    }
  }
}

/* Compute covariance matrix conditionnaly to observations*/
CovarianceMatrix KrigingResult::getConditionalCovariance(const NumericalSample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned matrix should have dimensions (p * s) x (p * s)
  if (!hasCholeskyFactor_)
    throw InvalidArgumentException(HERE) << "In KrigingResult::getConditionalCovariance, Cholesky factor was not provided. This last one is mandatory to compute the covariance";
  const UnsignedInteger spatialDimension(xi.getDimension());
  if (spatialDimension != covarianceModel_.getSpatialDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalCovariance, input data should have the same dimension as covariance model's spatial dimension. Here, (input dimension = " << spatialDimension << ", covariance model spatial's dimension = " << covarianceModel_.getSpatialDimension() << ")";
  const UnsignedInteger outputDimension(covarianceModel_.getDimension());
  const UnsignedInteger sampleSize(xi.getSize());
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalCovariance, expected a non empty sample";
  // 0) Take into account transformation
  NumericalSample sample;
  // Transform data if necessary
  if (hasTransformation_)
    sample = inputTransformation_(xi);
  else
    sample = xi;
  // 1) compute \sigma_{x,x}
  LOGINFO("Compute interactions Sigma_xx");
  CovarianceMatrix sigmaXX(covarianceModel_.discretize(sample));

  // 2) compute \sigma_{y,x}
  // compute r(x), the crossCovariance between the conditionned data & xi
  LOGINFO("Compute cross-interactions sigmaYX");
  const Matrix crossCovariance(getCrossMatrix(sample));
  // 3) Compute r^t R^{-1} r'x)
  // As we get the Cholesky factor L, we can solve triangular linear system
  // We define B = L^{-1} * r(x)
  Matrix B;
  if (0 != covarianceCholeskyFactor_.getNbRows())
  {
    LOGINFO("Solve L.B = SigmaYX");
    B = covarianceCholeskyFactor_.solveLinearSystem(crossCovariance);
  }
  else
  {
    LOGINFO("Solve L.B = SigmaYX (h-mat version)");
    B = covarianceHMatrix_.solveLower(crossCovariance);
  }
  // Use of gram to compute B^{t} * B
  // Default gram computes B*B^t
  // With transpose argument=true, it performs B^t*B
  // With full argument=false, lower triangular matrix B^t*B is not symmetrized
  LOGINFO("Compute B^tB");
  CovarianceMatrix BtB(B.computeGram(true));

  // Interest is to compute sigma_xx -= BtB
  // However it is not trivial that A - B is a covariance matrix if A & B are covariance matrices
  // Symmeric : ok but not necessary definite. Here by definition it is!
  // So should we define  operator - & operator -= with covariances?
  LOGINFO("Compute Sigma_xx-BtB");
  CovarianceMatrix result(*sigmaXX.getImplementation() * sigma2_ - *BtB.getImplementation() * sigma2_);

  // Case of simple Kriging
  if(basis_.getSize() == 0) return result;

  // Case of universal Kriging: compute the covariance due to the regression part
  // Additionnal information have to be computed
  // 1) compute F
  LOGINFO("Compute the regression matrix F");
  computeF();
  // 2) Interest is (F^t R^{-1} F)^{-1}
  // F^{t} R^{-1} F = F^{t} L^{-t} L^{-1} F
  // Solve first L phi = F
  Matrix Q;
  if (Gt_.getNbRows() == 0)
  {
    LOGINFO("Solve linear system  L * phi= F");
    Matrix phi;
    if (0 != covarianceCholeskyFactor_.getNbRows())
      phi = covarianceCholeskyFactor_.solveLinearSystem(F_);
    else
      phi = covarianceHMatrix_.solveLower(F_);
    // Compute QR decomposition of Phi_
    LOGINFO("Compute the QR decomposition of phi");
    Matrix G;
    Q = phi.computeQR(G);
    Gt_ = G.transpose();
    phiT_ = phi.transpose();
  }
  // 3) Compute u(x) = F^t *R^{-1} * r(x) - f(x)
  //                 = F^{t} * L^{-1}^t * L{-1} * r(x) - f(x)
  //                 = phiT_ * B - f(x)
  LOGINFO("Compute psi = phi^t * B");
  Matrix psi(phiT_ * B);
  // compute f(x) & define u = psi - f(x)
  LOGINFO("Compute f(x)");
  // Note that fx = F^{T} for x in inputData_
  Matrix fx(F_.getNbColumns(), sampleSize * covarianceModel_.getDimension());
  // Fill fx => equivalent to F for the x data with transposition
  UnsignedInteger index = 0;
  for (UnsignedInteger basisMarginal = 0; basisMarginal < basis_.getSize(); ++ basisMarginal )
  {
    const Basis localBasis(basis_[basisMarginal]);
    const UnsignedInteger localBasisSize(localBasis.getSize());
    for (UnsignedInteger j = 0; j < localBasisSize; ++ j )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const NumericalSample basisSample(localBasis[j](sample));
      for (UnsignedInteger i = 0; i < sampleSize; ++ i) fx(j + index, basisMarginal + i * outputDimension) = basisSample[i][0];
    }
    index = index + localBasisSize;
  }
  LOGINFO("Compute ux = psi - fx");
  Matrix ux(psi - fx);

  // interest now is to solve  G rho = ux
  LOGINFO("Solve linear system G * rho = ux");
  Matrix rho(Gt_.solveLinearSystem(ux * std::sqrt(sigma2_)));
  LOGINFO("Compute Sigma_xx-BtB + rho^{t}*rho");
  result = result + rho.computeGram(true);
  return result;
}

/* Compute covariance matrix conditionnaly to observations*/
CovarianceMatrix KrigingResult::getConditionalCovariance(const NumericalPoint & xi) const
{
  const NumericalSample sample(1, xi);
  return getConditionalCovariance(sample);
}

/* Compute joint normal distribution conditionnaly to observations*/
Normal KrigingResult::operator()(const NumericalSample & xi) const
{
  // The Normal distribution is defined by its mean & covariance
  LOGINFO("In KrigingResult::operator() : evaluationg the mean");
  const NumericalPoint mean = getConditionalMean(xi);
  LOGINFO("In KrigingResult::operator() : evaluationg the covariance");
  const CovarianceMatrix covarianceMatrix = getConditionalCovariance(xi);
  // Check the covariance matrix. Indeed, if point is very similar to one of the learning points, covariance is null
  // Even if this check is done in Normal::Normal, we perform debugging
  LOGINFO("In KrigingResult::operator() : evaluationg the Normal distribution");
  if (!const_cast<CovarianceMatrix*>(&covarianceMatrix)->isPositiveDefinite()) throw InvalidArgumentException(HERE) << "In KrigingResult::operator(), the covariance matrix is not positive definite. The given points could be very close to the learning set. Could not build the Normal distribution";
  // Finally return the distribution
  return Normal(mean, covarianceMatrix);
}

/* Compute joint normal distribution conditionnaly to observations*/
Normal KrigingResult::operator()(const NumericalPoint & xi) const
{
  NumericalSample sample(1, xi);
  return operator()(sample);
}

/* Method save() stores the object through the StorageManager */
void KrigingResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "inputData_", inputData_ );
  adv.saveAttribute( "inputTransformedData_", inputTransformedData_ );
  adv.saveAttribute( "inputTransformation_", inputTransformation_ );
  adv.saveAttribute( "hasTransformation_", hasTransformation_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "trendCoefficients_", trendCoefficients_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "covarianceCoefficients_", covarianceCoefficients_ );
  adv.saveAttribute( "hasCholeskyFactor_", hasCholeskyFactor_);
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
  adv.saveAttribute( "sigma2_", sigma2_ );
  adv.saveAttribute( "F_", F_);
  adv.saveAttribute( "phiT_", phiT_);
  adv.saveAttribute( "Gt_", Gt_);
}


/* Method load() reloads the object from the StorageManager */
void KrigingResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "inputData_", inputData_ );
  adv.loadAttribute( "inputTransformedData_", inputTransformedData_ );
  adv.loadAttribute( "inputTransformation_", inputTransformation_ );
  adv.loadAttribute( "hasTransformation_", hasTransformation_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "trendCoefficients_", trendCoefficients_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "covarianceCoefficients_", covarianceCoefficients_ );
  adv.loadAttribute( "hasCholeskyFactor_", hasCholeskyFactor_);
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
  adv.loadAttribute( "sigma2_", sigma2_ );
  adv.loadAttribute( "F_", F_);
  adv.loadAttribute( "phiT_", phiT_);
  adv.loadAttribute( "Gt_", Gt_);

}



END_NAMESPACE_OPENTURNS
