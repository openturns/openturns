//                                               -*- C++ -*-
/**
 *  @brief The result of a kriging estimation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/KrigingResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/DatabaseFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingResult)
static const Factory<KrigingResult> Factory_KrigingResult;

/* Default constructor */
KrigingResult::KrigingResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters & Cholesky factor */
KrigingResult::KrigingResult(const Sample & inputSample,
                             const Sample & outputSample,
                             const Function & metaModel,
                             const Point & residuals,
                             const Point & relativeErrors,
                             const BasisCollection & basis,
                             const PointCollection & trendCoefficients,
                             const CovarianceModel & covarianceModel,
                             const Sample & covarianceCoefficients)
  : MetaModelResult(DatabaseFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , basis_(basis)
  , trendCoefficients_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , covarianceCoefficients_(covarianceCoefficients)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In KrigingResult::KrigingResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
}


/* Constructor with parameters & Cholesky factor */
KrigingResult::KrigingResult(const Sample & inputSample,
                             const Sample & outputSample,
                             const Function & metaModel,
                             const Point & residuals,
                             const Point & relativeErrors,
                             const BasisCollection & basis,
                             const PointCollection & trendCoefficients,
                             const CovarianceModel & covarianceModel,
                             const Sample & covarianceCoefficients,
                             const TriangularMatrix & covarianceCholeskyFactor,
                             const HMatrix & covarianceHMatrix)
  : MetaModelResult(DatabaseFunction(inputSample, outputSample), metaModel, residuals, relativeErrors)
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , basis_(basis)
  , trendCoefficients_(trendCoefficients)
  , covarianceModel_(covarianceModel)
  , covarianceCoefficients_(covarianceCoefficients)
  , covarianceCholeskyFactor_(covarianceCholeskyFactor)
  , covarianceHMatrix_(covarianceHMatrix)
{
  const UnsignedInteger outputDimension = outputSample.getDimension();
  const UnsignedInteger size = inputSample.getSize();
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

String KrigingResult::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  oss << "covariance models=" << covarianceModel_;
  oss << ", covariance coefficients=" << covarianceCoefficients_;
  oss << ", basis=" << basis_;
  oss << ", trend coefficients=" << trendCoefficients_ << ")";
  return oss;
}


/* Design accessors */
Sample KrigingResult::getInputSample() const
{
  return inputSample_;
}

Sample KrigingResult::getOutputSample() const
{
  return outputSample_;
}


/* Basis accessor */
KrigingResult::BasisCollection KrigingResult::getBasisCollection() const
{
  return basis_;
}

/* Trend coefficients accessor */
KrigingResult::PointCollection KrigingResult::getTrendCoefficients() const
{
  return trendCoefficients_;
}

/* Covariance models accessor */
CovarianceModel KrigingResult::getCovarianceModel() const
{
  return covarianceModel_;
}

/* Covariance coefficients accessor */
Sample KrigingResult::getCovarianceCoefficients() const
{
  return covarianceCoefficients_;
}


/* Compute mean of new points conditionally to observations */
Point KrigingResult::getConditionalMean(const Sample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned matrix should have dimensions (p * s) x (p * s)
  const UnsignedInteger inputDimension = xi.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getMean, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMean, expected a non empty sample";
  // Use of metamodel to return result
  // Need to think if it is required to implement a specific method
  // in order to avoid data copy
  // sample is of size xi.getSize() * covarianceModel.getDimension()
  Sample output = metaModel_.operator()(xi);
  // Result is a Point ==> data are transformed form Sample to
  // Point by using a copy
  Point mean(output.getImplementation()->getData());
  return mean;
}

/* Compute mean of new points conditionally to observations */
Point KrigingResult::getConditionalMean(const Point & xi) const
{
  // Use of meta model evaluation
  // For Point, no problematic of copy
  const Point output = metaModel_.operator()(xi);
  return output;
}

struct KrigingResultCrossCovarianceFunctor
{
  const Sample & conditionnedPoints_;
  const Sample & input_;
  Matrix & output_;
  const CovarianceModel & model_;
  const UnsignedInteger dimension_;

  KrigingResultCrossCovarianceFunctor(const Sample & conditionnedPoints,
                                      const Sample & input,
                                      Matrix & output,
                                      const CovarianceModel & model)
    : conditionnedPoints_(conditionnedPoints)
    , input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getOutputDimension())
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
      const UnsignedInteger jLocal = i / conditionnedPoints_.getSize();
      const UnsignedInteger jBase = jLocal * dimension_;
      const UnsignedInteger iLocal = i - jLocal * conditionnedPoints_.getSize();
      const UnsignedInteger iBase = iLocal * dimension_;
      // Local covariance matrix
      const SquareMatrix localCovariance(model_(conditionnedPoints_[iLocal], input_[jLocal]));
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

struct KrigingResultCrossCovarianceFunctor1D
{
  const Sample & conditionnedPoints_;
  const Sample & input_;
  Matrix & output_;
  const CovarianceModel & model_;

  KrigingResultCrossCovarianceFunctor1D(const Sample &conditionnedPoints,
                                        const Sample &input,
                                        Matrix & output,
                                        const CovarianceModel & model)
    : conditionnedPoints_(conditionnedPoints)
    , input_(input)
    , output_(output)
    , model_(model)
  {
  }

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> &r) const
  {

    const UnsignedInteger inputDimension = input_.getDimension();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      for (UnsignedInteger j = 0; j < input_.getSize(); ++j)
        output_(i, j) = model_.getImplementation()->computeAsScalar(conditionnedPoints_.getImplementation()->data_begin() + (i * inputDimension),
                        input_.getImplementation()->data_begin() + (j * inputDimension));
    }
  } // operator()
};
/* end struct KrigingResultCrossCovarianceFunctor1D */

/* Compute cross matrix method ==> not necessary square matrix  */
Matrix KrigingResult::getCrossMatrix(const Sample & x) const
{
  // Use of TBB structures
  // The idea is that we work by blocks
  // the (i,j) block corresponds to the interaction x[i], inputData[j]
  // Each block is of size d x d
  // So we have trainingSize * sampleSize blocks
  // We fill the matrix by columns
  const UnsignedInteger dimension = covarianceModel_.getOutputDimension();
  if (dimension == 1)
  {
    const UnsignedInteger trainingSize = inputSample_.getSize();
    const UnsignedInteger sampleSize = x.getSize();
    Matrix result(trainingSize, sampleSize);
    const KrigingResultCrossCovarianceFunctor1D policy(inputSample_, x, result, covarianceModel_);
    // The loop is over the lower block-triangular part
    TBB::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize, policy);
    return result;
  }
  const UnsignedInteger trainingSize = inputSample_.getSize();
  const UnsignedInteger trainingFullSize = trainingSize * dimension;
  const UnsignedInteger sampleSize = x.getSize();
  const UnsignedInteger sampleFullSize = sampleSize * dimension;
  Matrix result(trainingFullSize, sampleFullSize);
  const KrigingResultCrossCovarianceFunctor policy(inputSample_, x, result, covarianceModel_);
  // The loop is over the lower block-triangular part
  TBB::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize * sampleSize, policy);
  return result;
}

struct KrigingResultCrossCovariancePointFunctor
{
  const Sample &conditionnedPoints_;
  const Point &input_;
  Matrix &output_;
  const CovarianceModel &model_;

  KrigingResultCrossCovariancePointFunctor(const Sample &conditionnedPoints,
      const Point &input,
      Matrix &output,
      const CovarianceModel &model)
    : conditionnedPoints_(conditionnedPoints)
    , input_(input)
    , output_(output)
    , model_(model)
  {
  }

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> &r) const
  {
    const UnsignedInteger dimension = model_.getOutputDimension();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      SquareMatrix localCovariance(model_(conditionnedPoints_[i], input_));
      for (UnsignedInteger columnIndex = 0; columnIndex < dimension; ++columnIndex)
        for (UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
          output_(i * dimension + rowIndex, columnIndex) = localCovariance(rowIndex, columnIndex);
    }
  } // operator()
};
/* end struct KrigingResultCrossCovariancePointFunctor */

struct KrigingResultCrossCovariancePointFunctor1D
{
  const Sample & conditionnedPoints_;
  const Point & input_;
  Matrix &output_;
  const CovarianceModel & model_;

  KrigingResultCrossCovariancePointFunctor1D(const Sample &conditionnedPoints,
      const Point &input,
      Matrix & output,
      const CovarianceModel &model)
    : conditionnedPoints_(conditionnedPoints)
    , input_(input)
    , output_(output)
    , model_(model)
  {
  }

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> &r) const
  {

    const UnsignedInteger inputDimension = input_.getDimension();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      output_(i, 0) = model_.getImplementation()->computeAsScalar(conditionnedPoints_.getImplementation()->data_begin() + (i * inputDimension), input_.begin() );
    }
  } // operator()
};
/* end struct KrigingResultCrossCovariancePointFunctor1D */

Matrix KrigingResult::getCrossMatrix(const Point & point) const
{
  const UnsignedInteger trainingSize = inputSample_.getSize();
  const UnsignedInteger outputDimension = covarianceModel_.getOutputDimension();
  if (outputDimension == 1)
  {
    Matrix result(trainingSize, 1);
    const KrigingResultCrossCovariancePointFunctor1D policy(inputSample_, point, result, covarianceModel_);
    // The loop is over the lower block-triangular part
    TBB::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize, policy);
    return result;
  }
  const UnsignedInteger trainingFullSize = trainingSize * covarianceModel_.getOutputDimension();
  Matrix result(trainingFullSize, outputDimension);
  const KrigingResultCrossCovariancePointFunctor policy(inputSample_, point, result, covarianceModel_);
  TBB::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize, policy);
  return result;
}

/* Compute cross matrix method ==> not necessary square matrix  */
void KrigingResult::computeF() const
{
  // Nothing to do if the design matrix has already been computed
  if (F_.getNbRows() != 0) return;
  const UnsignedInteger outputDimension = covarianceModel_.getOutputDimension();
  const UnsignedInteger sampleSize = inputSample_.getSize();
  const UnsignedInteger basisCollectionSize = basis_.getSize();
  UnsignedInteger totalSize = 0;
  for (UnsignedInteger i = 0; i < basisCollectionSize; ++ i ) totalSize += basis_[i].getSize();
  // basis_ is of size 0 or outputDimension
  if (totalSize == 0) return;
  UnsignedInteger index = 0;
  // Compute F
  F_ = Matrix(sampleSize * outputDimension, totalSize);
  for (UnsignedInteger outputMarginal = 0; outputMarginal < basisCollectionSize; ++ outputMarginal )
  {
    const Basis localBasis(basis_[outputMarginal]);
    const UnsignedInteger localBasisSize = localBasis.getSize();
    for (UnsignedInteger j = 0; j < localBasisSize; ++j, ++index )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const Sample basisSample(localBasis[j](inputSample_));
      for (UnsignedInteger i = 0; i < sampleSize; ++i) F_(outputMarginal + i * outputDimension, index) = basisSample(i, 0);
    }
  }
}

/* Compute cross matrix method ==> not necessary square matrix  */
void KrigingResult::computePhi() const
{
  // Nothing to do if the design matrix has already been computed
  if (Gt_.getNbRows() != 0) return;
  Matrix Q;
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

/* Compute covariance matrix conditionally to observations*/
CovarianceMatrix KrigingResult::getConditionalCovariance(const Sample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned matrix should have dimensions (p * s) x (p * s)
  const UnsignedInteger inputDimension = xi.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  const UnsignedInteger outputDimension = covarianceModel_.getOutputDimension();
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalCovariance, expected a non empty sample";

  const Sample sample(xi);
  // 1) compute \sigma_{x,x}
  LOGINFO("Compute interactions Sigma_xx");
  const CovarianceMatrix sigmaXX(covarianceModel_.discretize(sample));

  // 2) compute \sigma_{y,x}
  // compute r(x), the crossCovariance between the conditionned data & xi
  LOGINFO("Compute cross-interactions sigmaYX");
  const Matrix crossCovariance(getCrossMatrix(sample));
  // 3) Compute r^t R^{-1} r'(x)
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
  const CovarianceMatrix BtB(B.computeGram(true));

  // Interest is to compute sigma_xx -= BtB
  // However it is not trivial that A - B is a covariance matrix if A & B are covariance matrices
  // Symmetric : ok but not necessary definite. Here by definition it is!
  // So should we define  operator - & operator -= with covariances?
  LOGINFO("Compute Sigma_xx-BtB");
  CovarianceMatrix result(*sigmaXX.getImplementation() - *BtB.getImplementation() );

  // Case of simple Kriging
  if(basis_.getSize() == 0) return result;

  // Case of universal Kriging: compute the covariance due to the regression part
  // Additional information have to be computed
  // 1) compute F
  LOGINFO("Compute the regression matrix F");
  computeF();
  // 2) Interest is (F^t R^{-1} F)^{-1}
  // F^{t} R^{-1} F = F^{t} L^{-t} L^{-1} F
  // Solve first L phi = F
  computePhi();
  // 3) Compute u(x) = F^t *R^{-1} * r(x) - f(x)
  //                 = F^{t} * L^{-1}^t * L{-1} * r(x) - f(x)
  //                 = phiT_ * B - f(x)
  LOGINFO("Compute psi = phi^t * B");
  const Matrix psi(phiT_ * B);
  // compute f(x) & define u = psi - f(x)
  LOGINFO("Compute f(x)");
  // Note that fx = F^{T} for x in inputSample_
  Matrix fx(F_.getNbColumns(), sampleSize * covarianceModel_.getOutputDimension());
  // Fill fx => equivalent to F for the x data with transposition
  UnsignedInteger index = 0;
  for (UnsignedInteger basisMarginal = 0; basisMarginal < basis_.getSize(); ++ basisMarginal )
  {
    const Basis localBasis(basis_[basisMarginal]);
    const UnsignedInteger localBasisSize = localBasis.getSize();
    for (UnsignedInteger j = 0; j < localBasisSize; ++ j )
    {
      // Here we use potential parallelism in the evaluation of the basis functions
      const Sample basisSample(localBasis[j](sample));
      for (UnsignedInteger i = 0; i < sampleSize; ++ i) fx(j + index, basisMarginal + i * outputDimension) = basisSample(i, 0);
    }
    index = index + localBasisSize;
  }
  LOGINFO("Compute ux = psi - fx");
  const Matrix ux(psi - fx);

  // interest now is to solve  G rho = ux
  LOGINFO("Solve linear system G * rho = ux");
  const Matrix rho(Gt_.solveLinearSystem(ux));
  LOGINFO("Compute Sigma_xx-BtB + rho^{t}*rho");
  result = result + rho.computeGram(true);
  return result;
}

/* Compute covariance matrix conditionally to observations*/
CovarianceMatrix KrigingResult::getConditionalCovariance(const Point & point) const
{
  // For a process of output dimension p
  // returned p x p matrix
  const UnsignedInteger inputDimension = point.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";

  const Point data(point);
  // 1) compute \sigma_{x,x}
  LOGINFO("Compute interactions Sigma_xx");
  const SquareMatrix sigmaXX(covarianceModel_(Point(inputDimension, 0.0)));

  // 2) compute \sigma_{y,x}
  // compute r(x), the crossCovariance between the conditionned data & xi
  LOGINFO("Compute cross-interactions sigmaYX");
  const Matrix crossCovariance(getCrossMatrix(data));
  // 3) Compute r^t R^{-1} r'(x)
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
  const CovarianceMatrix BtB(B.computeGram(true));

  // Interest is to compute sigma_xx -= BtB
  // However it is not trivial that A - B is a covariance matrix if A & B are covariance matrices
  // Symmetric : ok but not necessary definite. Here by definition it is!
  // So should we define  operator - & operator -= with covariances?
  LOGINFO("Compute Sigma_xx-BtB");
  CovarianceMatrix result(*sigmaXX.getImplementation() - *BtB.getImplementation() );

  // Case of simple Kriging
  if(basis_.getSize() == 0) return result;

  // Case of universal Kriging: compute the covariance due to the regression part
  // Additional information have to be computed
  // 1) compute F
  LOGINFO("Compute the regression matrix F");
  computeF();
  // 2) Interest is (F^t R^{-1} F)^{-1}
  // F^{t} R^{-1} F = F^{t} L^{-t} L^{-1} F
  // Solve first L phi = F
  computePhi();
  // 3) Compute u(x) = F^t *R^{-1} * r(x) - f(x)
  //                 = F^{t} * L^{-1}^t * L{-1} * r(x) - f(x)
  //                 = phiT_ * B - f(x)
  LOGINFO("Compute psi = phi^t * B");
  const Matrix psi(phiT_ * B);
  // compute f(x) & define u = psi - f(x)
  LOGINFO("Compute f(x)");
  // Note that fx = F^{T} for x in inputSample_
  Matrix fx(F_.getNbColumns(), covarianceModel_.getOutputDimension());
  // Fill fx => equivalent to F for the x data with transposition
  UnsignedInteger index = 0;
  for (UnsignedInteger basisMarginal = 0; basisMarginal < basis_.getSize(); ++ basisMarginal )
  {
    const Basis localBasis(basis_[basisMarginal]);
    const UnsignedInteger localBasisSize = localBasis.getSize();
    for (UnsignedInteger j = 0; j < localBasisSize; ++ j )
    {
      fx(j + index, basisMarginal) = localBasis[j](data)[0];
    }
    index = index + localBasisSize;
  }
  LOGINFO("Compute ux = psi - fx");
  const Matrix ux(psi - fx);

  // interest now is to solve  G rho = ux
  LOGINFO("Solve linear system G * rho = ux");
  const Matrix rho(Gt_.solveLinearSystem(ux));
  LOGINFO("Compute Sigma_xx-BtB + rho^{t}*rho");
  result = result + rho.computeGram(true);
  return result;
}

/** Compute covariance matrices conditionally to observations (1 cov / point)*/
KrigingResult::CovarianceMatrixCollection KrigingResult::getConditionalMarginalCovariance(const Sample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned a s-collection of cov matrices (pxp)
  const UnsignedInteger inputDimension = xi.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalCovariance, expected a non empty sample";

  CovarianceMatrixCollection collection(sampleSize);
  Point data(inputDimension);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j] = xi(i, j);
    // Rely on getConditionalCovariance(Point&)
    collection[i] = getConditionalCovariance(data);
  }
  return collection;
}

/** Compute covariance matrix conditionally to observations (1 cov of size outdimension)*/
CovarianceMatrix KrigingResult::getConditionalMarginalCovariance(const Point & xi) const
{
  const UnsignedInteger inputDimension = xi.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  return getConditionalCovariance(xi);
}

/* Compute joint normal distribution conditionally to observations*/
Normal KrigingResult::operator()(const Sample & xi) const
{
  // The Normal distribution is defined by its mean & covariance
  LOGINFO("In KrigingResult::operator() : evaluating the mean");
  const Point mean = getConditionalMean(xi);
  LOGINFO("In KrigingResult::operator() : evaluating the covariance");
  const CovarianceMatrix covarianceMatrix = getConditionalCovariance(xi);
  // Check the covariance matrix. Indeed, if point is very similar to one of the learning points, covariance is null
  // Even if this check is done in Normal::Normal, we perform debugging
  LOGINFO("In KrigingResult::operator() : evaluating the Normal distribution");
  if (!covarianceMatrix.isPositiveDefinite()) throw InvalidArgumentException(HERE) << "In KrigingResult::operator(), the covariance matrix is not positive definite. The given points could be very close to the learning set. Could not build the Normal distribution";
  // Finally return the distribution
  return Normal(mean, covarianceMatrix);
}


/** Compute marginal variance conditionally to observations (1 cov of size outdimension)*/
Scalar KrigingResult::getConditionalMarginalVariance(const Point & point,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = point.getDimension();
  const UnsignedInteger outputDimension = covarianceModel_.getOutputDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  if ( !(marginalIndex < outputDimension))
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, marginalIndex should be in [0," << outputDimension << "]. Here, marginalIndex = " << marginalIndex ;
  // Compute the matrix & return only the marginalIndex diagonal element
  const CovarianceMatrix covarianceMatrix(getConditionalMarginalCovariance(point));
  return covarianceMatrix(marginalIndex, marginalIndex);
}

/** Compute marginal variance conditionally to observations (1 cov / point)*/
Point KrigingResult::getConditionalMarginalVariance(const Sample & xi,
    const UnsignedInteger marginalIndex) const
{

  const UnsignedInteger inputDimension = xi.getDimension();
  const UnsignedInteger outputDimension = covarianceModel_.getOutputDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  if ( !(marginalIndex < outputDimension))
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, marginalIndex should be in [0," << outputDimension << "]. Here, marginalIndex = " << marginalIndex ;
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, expected a non empty sample";

  if  (outputDimension == 1)
  {
    const Sample sample(xi);
    // 1) compute \sigma_{x,x}
    LOGINFO("Compute interactions Sigma_xx");
    // Only diagonal of the discretization Matrix
    // First set sigmaXX
    const Point tau(inputDimension);
    // There is no computeAsScalr(tau) method
    const Scalar sigma2 = covarianceModel_(tau)(0, 0);
    Point result(sampleSize, sigma2);


    // 2) compute \sigma_{y,x}
    // compute r(x), the crossCovariance between the conditionned data & xi
    LOGINFO("Compute cross-interactions sigmaYX");
    const Matrix crossCovariance(getCrossMatrix(sample));
    // 3) Compute r^t R^{-1} r'(x)
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
    // We compute diag(B^t B)
    // We can notice that it corresponds to the sum of elements
    // for each column
    LOGINFO("Compute B^tB & Sigma_xx-BtB");
    //const CovarianceMatrix BtB(B.computeGram(true));
    for (UnsignedInteger j = 0; j < B.getNbColumns(); ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < B.getNbRows(); ++i)
        sum += B(i, j) * B(i, j);
      result[j] -= sum;
    }

    // Case of simple Kriging
    if (basis_.getSize() == 0) return result;

    // Case of universal Kriging: compute the covariance due to the regression part
    // Additional information have to be computed
    // 1) compute F
    LOGINFO("Compute the regression matrix F");
    computeF();
    // 2) Interest is (F^t R^{-1} F)^{-1}
    // F^{t} R^{-1} F = F^{t} L^{-t} L^{-1} F
    // Solve first L phi = F
    computePhi();
    // 3) Compute u(x) = F^t *R^{-1} * r(x) - f(x)
    //                 = F^{t} * L^{-1}^t * L{-1} * r(x) - f(x)
    //                 = phiT_ * B - f(x)
    LOGINFO("Compute psi = phi^t * B");
    const Matrix psi(phiT_ * B);
    // compute f(x) & define u = psi - f(x)
    LOGINFO("Compute f(x)");
    // Note that fx = F^{T} for x in inputSample_
    Matrix fx(F_.getNbColumns(), sampleSize);
    // Fill fx => equivalent to F for the x data with transposition
    UnsignedInteger index = 0;
    for (UnsignedInteger basisMarginal = 0; basisMarginal < basis_.getSize(); ++basisMarginal)
    {
      const Basis localBasis(basis_[basisMarginal]);
      const UnsignedInteger localBasisSize = localBasis.getSize();
      for (UnsignedInteger j = 0; j < localBasisSize; ++j)
      {
        // Here we use potential parallelism in the evaluation of the basis functions
        const Sample basisSample(localBasis[j](sample));
        for (UnsignedInteger i = 0; i < sampleSize; ++i)
          fx(j + index, basisMarginal + i * outputDimension) = basisSample(i, 0);
      }
      index = index + localBasisSize;
    }
    LOGINFO("Compute ux = psi - fx");
    const Matrix ux(psi - fx);

    // interest now is to solve  G rho = ux
    LOGINFO("Solve linear system G * rho = ux");
    const Matrix rho(Gt_.solveLinearSystem(ux));
    LOGINFO("Compute Sigma_xx-BtB + rho^{t}*rho");
    for (UnsignedInteger j = 0; j < rho.getNbColumns(); ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < rho.getNbRows(); ++i)
        sum += rho(i, j) * rho(i, j);
      result[j] += sum;
    }
    return result;
  }

  Point marginalVariance(sampleSize);

  Point data(inputDimension);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j] = xi(i, j);
    marginalVariance[i] = getConditionalMarginalVariance(data, marginalIndex);
  }
  return marginalVariance;
}

Point KrigingResult::getConditionalMarginalVariance(const Point & point,
    const Indices &indices) const
{
  const UnsignedInteger inputDimension = point.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  if (!indices.check(covarianceModel_.getOutputDimension()))
    throw InvalidArgumentException(HERE) << "In KrigingResult::getConditionalMarginalVariance, the indices of a marginal sample must be in the range [0," << covarianceModel_.getOutputDimension()
                                         << " ] and must be different";
  // Compute the matrix & return only the marginalIndex diagonal element
  const CovarianceMatrix covarianceMatrix(getConditionalMarginalCovariance(point));
  Point result(indices.getSize());
  for (UnsignedInteger j = 0; j < indices.getSize(); ++j) result[j] = covarianceMatrix(indices[j], indices[j]);
  return result;
}

Point KrigingResult::getConditionalMarginalVariance(const Sample & xi,
    const Indices & indices) const
{

  const UnsignedInteger inputDimension = xi.getDimension();
  if (inputDimension != covarianceModel_.getInputDimension())
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel_.getInputDimension() << ")";
  if (!indices.check(covarianceModel_.getOutputDimension()))
    throw InvalidArgumentException(HERE) << "In KrigingResult::getConditionalMarginalVariance, the indices of a marginal sample must be in the range [0," << covarianceModel_.getOutputDimension()
                                         << " ] and must be different";
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In KrigingResult::getConditionalMarginalVariance, expected a non empty sample";
  // Compute the matrix & return only the marginalIndex diagonal element
  Point result(0);
  Point data(inputDimension);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j] = xi(i, j);
    result.add(getConditionalMarginalVariance(data, indices));
  }
  return result;
}


/* Compute joint normal distribution conditionally to observations*/
Normal KrigingResult::operator()(const Point & xi) const
{
  Sample sample(1, xi);
  return operator()(sample);
}

/* Method save() stores the object through the StorageManager */
void KrigingResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "trendCoefficients_", trendCoefficients_ );
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "covarianceCoefficients_", covarianceCoefficients_ );
  adv.saveAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
  adv.saveAttribute( "F_", F_);
  adv.saveAttribute( "phiT_", phiT_);
  adv.saveAttribute( "Gt_", Gt_);
}


/* Method load() reloads the object from the StorageManager */
void KrigingResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "trendCoefficients_", trendCoefficients_ );
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "covarianceCoefficients_", covarianceCoefficients_ );
  adv.loadAttribute( "covarianceCholeskyFactor_", covarianceCholeskyFactor_);
  adv.loadAttribute( "F_", F_);
  adv.loadAttribute( "phiT_", phiT_);
  adv.loadAttribute( "Gt_", Gt_);

}



END_NAMESPACE_OPENTURNS
