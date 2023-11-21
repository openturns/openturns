//                                               -*- C++ -*-
/**
 *  @brief ARMALikelihoodFactory makes an estimate of a multivariate ARMA model from data
 *  using a maximization of the likelihood function. We use here some articles of J.A.Mauricio (http://www.ucm.es/info/ecocuan/jam/)
 *  to use an efficient method
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ARMALikelihoodFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ARMACoefficients.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/Normal.hxx"
#include "openturns/WhittleFactory.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/MethodBoundEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ARMALikelihoodFactory)

static const Factory<ARMALikelihoodFactory> Factory_ARMALikelihoodFactory;

static int modifiedCholeskyDecomposition(SquareMatrix &matrix,
    Scalar epsilon);

/* Default constructor */
ARMALikelihoodFactory::ARMALikelihoodFactory()
  : ARMAFactoryImplementation()
  , solver_(new Cobyla())
{
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeCobylaSolverParameter();
}

/* Standard constructor */
ARMALikelihoodFactory::ARMALikelihoodFactory(const UnsignedInteger p,
    const UnsignedInteger q,
    const UnsignedInteger dimension,
    const Bool invertible)
  : ARMAFactoryImplementation(p, q, invertible)
  , dimension_(dimension)
  , solver_(new Cobyla())
{
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error : dimension could not be zero" ;
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeCobylaSolverParameter();
  if ((p != 0) || (q != 0))
  {
    currentP_ = p;
    currentQ_ = q;
  }
  else
  {
    throw InvalidArgumentException(HERE) << "Error : both AR and MA sizes are null";
  }
  // Initializing - With this constructor, no exploration of several configurations
  initialize();
}

/* Parameter constructor */
ARMALikelihoodFactory::ARMALikelihoodFactory(const Indices & p,
    const Indices & q,
    const UnsignedInteger dimension,
    const Bool invertible)
  : ARMAFactoryImplementation(p, q, invertible)
  , dimension_(dimension)
  , solver_(new Cobyla())
{
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error : dimension could not be zero" ;
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeCobylaSolverParameter();
}

void ARMALikelihoodFactory::initialize()
{
  // Initializing matrices depending on the couple (p, q) and dimension
  blockPhiTThetaTMatrix_ = Matrix(dimension_ * (currentP_ + currentQ_), dimension_);
  currentG_ = std::max(currentP_, currentQ_);
  covarianceMatrix_ = CovarianceMatrix(dimension_);

  // Set up the booleans to false since reallocation has been done
  hasInitializedARCoefficients_ = false;
  hasInitializedMACoefficients_ = false;
  hasInitializedCovarianceMatrix_ = false;
}

/* Virtual constructor */
ARMALikelihoodFactory * ARMALikelihoodFactory::clone() const
{
  return new ARMALikelihoodFactory(*this);
}

/* Compute the log-likelihood function */

Scalar ARMALikelihoodFactory::computeLogLikelihood(const Point & beta) const
{
  // beta regroups all unknown parameters :
  // p square matrices of dimension m (AR part) ==> m * m * p scalar coefficients
  // q square matrices of dimension m (MA part) ==> m * m * q scalar coefficients
  // 1 covariance matrix  ==> m * (m + 1) / 2 scalar coefficients
  // 1 scalar parameter sigma2
  // Total number of unknowns is m * (m * (p + q) + 0.5 * (m + 1)) + 1

  // Set the coefficients and covariance matrix from input
  // beta has for size n = (p + q) * dimension * dimension + dimension * (dimension + 1) /2 + 1
  // The first (p + q) * dimension * dimension elements correspond to the block phi and theta (blockPhiTThetaTMatrix_)
  // the next dimension * (dimension + 1) /2 correspond to the covariance matrix)
  // final scalar corresponds to sigma2
  UnsignedInteger currentIndex = 0;
  for (UnsignedInteger k = 0; k < currentP_; ++k)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        // \phi_{k}(i,j)
        blockPhiTThetaTMatrix_(k * dimension_ + j, i) = beta[currentIndex];
        currentIndex += 1;
      }
    }
  }
  for (UnsignedInteger k = 0; k < currentQ_; ++k)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        // \theta_{k}(i,j)
        blockPhiTThetaTMatrix_((k  + currentP_) * dimension_ + j, i) = beta[currentIndex];
        currentIndex += 1;
      }
    }
  }

  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_; ++i)
    {
      // cov(i,j)
      covarianceMatrix_(i, j) = beta[currentIndex];
      currentIndex += 1;
    }
  }

  // sigma2 parameters
  sigma2_ = beta[currentIndex];

  covarianceMatrixCholesky_ = covarianceMatrix_.computeCholesky();
  covarianceMatrixCholeskyInverse_ = SquareMatrix(dimension_);
  Point rhs(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      rhs[j] = 0.0;
      covarianceMatrixCholeskyInverse_(j, i) = 0.0;
    }
    rhs[i] = 1.0;
    // Solve the linear system
    rhs[0] /= covarianceMatrixCholesky_(0, 0);
    for (UnsignedInteger k = 1; k < dimension_; ++k)
    {
      Scalar value = 0.0;
      for (UnsignedInteger l = 0; l < k; ++l)
      {
        value += covarianceMatrixCholesky_(k, l) * rhs[l];
      }
      rhs[k] = (rhs[k] - value) / covarianceMatrixCholesky_(k, k);
    }
    for (UnsignedInteger j = i; j < dimension_; ++j)
    {
      covarianceMatrixCholeskyInverse_(j, i) = rhs[j];
    }
  }

  if (currentP_ > 0)
  {
    computeCrossCovarianceMatrix();
    computeAutocovarianceMatrix();
  }

  // Methods depending on both time series and coefficients
  // Assemble \Sigma, compute its cholesky factor and its determinant
  // In case of q = 0, computation of two factor matrices of size p * dimension_ then assemble them into the choleskyFactor matrix

  // Step (c): Compute the symmetric matrix of size (currentG_ * dimension_) V1 * \Omega * V1^{T}
  //           and perform its Cholesky decomposition
  SquareMatrix matV1_Omega_V1TCholesky(computeV1_Omega_V1T_Cholesky());
  // Step (d): Compute the \xi matrices and premultiply by R = Q1^{-1}
  Matrix rxi(computeRXi());
  // Step (e): Compute the n vectors \eta_{i} = R a_{0,i}, i = 1, 2, ..., n-1
  Matrix eta(computeEta());
  // Step (f): Compute h vectors and premultiply them by M^{T}
  Point vector_h(computeVectorh(rxi, eta, matV1_Omega_V1TCholesky));
  // Step (g): Compute the symmetric matrix of size (dimension_ * currentG_) H^{T} H
  SymmetricMatrix matrix_HTH(computeHTH(rxi));
  // Step (h): Compute the symmetric matrix (I + M^{T} H^{T} H M)
  CovarianceMatrix cov_I_MTHTHM(computeI_MTHTHM(matrix_HTH, matV1_Omega_V1TCholesky));
  // and its Cholesky decomposition
  SquareMatrix matrix_L(cov_I_MTHTHM.computeCholesky());
  // Step (i): Solve L \lambda = M^{T} h = vector_h
  vector_h[0] /= matrix_L(0, 0);
  for (UnsignedInteger i = 1; i < currentG_ * dimension_; ++i)
  {
    Scalar value = 0.0;
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      value += matrix_L(i, j) * vector_h[j];
    }
    vector_h[i] = (vector_h[i] - value) / matrix_L(i, i);
  }
  // Step (j): Compute \eta^{T} \eta - \lambda^{T} \lambda
  Scalar norm1 = 0.0;
  const UnsignedInteger size = w_.getSize();
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      norm1 += eta(i, j) * eta(i, j);
    }
  }
  Scalar normSquare = vector_h.normSquare();

  Scalar f1 = norm1 - normSquare;

  // Step (k): compute the exact log-likelihood
  Scalar determinantL = 1.0;
  for (UnsignedInteger i = 0; i < currentG_ * dimension_; ++i)
  {
    determinantL *= matrix_L(i, i);
  }

  Scalar logLikelihood = 0.0;
  const Scalar n = static_cast<double>(w_.getSize());
  const Scalar fullSize = static_cast<double>(dimension_ * n);
  const Scalar lg2pi = log(2.0 * M_PI);
  const Scalar detQ = covarianceMatrix_.computeDeterminant();
  logLikelihood = -0.5 * (fullSize * (lg2pi + log(sigma2_)) + n * log(detQ) + 2.0 * log(determinantL) + f1 / sigma2_);
  return logLikelihood;
}

/* Compute the log-likelihood constraint */

Point ARMALikelihoodFactory::computeLogLikelihoodInequalityConstraint(const Point & beta) const
{
  const Scalar epsilon = ResourceMap::GetAsScalar("ARMALikelihoodFactory-RootEpsilon");

  Point result(nbInequalityConstraint_, 0.0);

  UnsignedInteger constraintIndex = 0;
  UnsignedInteger currentIndex = 0;

  // If not pure MA, check the eigenValues of the AR polynom
  if (currentP_ > 0)
  {
    // Companion matrix - Matrix is of size (dimension * p_)
    SquareMatrix matrix(dimension_ * currentP_);
    for (UnsignedInteger coefficientIndex = 0; coefficientIndex < currentP_ ; ++coefficientIndex)
    {
      for  (UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++ rowIndex)
      {
        for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++ columnIndex)
        {
          matrix( dimension_ * (currentP_ - 1) +  rowIndex, coefficientIndex * dimension_ + columnIndex ) = beta[currentIndex] ;
          ++currentIndex;
        }
      }
    }

    // Incorporation into the previous for loop
    for (UnsignedInteger index = 0; index < dimension_ * (currentP_ - 1); ++index)
    {
      matrix(index, dimension_ + index) = 1.0;
    }

    // Computation of EigenValues without keeping intact (matrix not used after)
    const Collection<Complex> eigenValues(matrix.computeEigenValues(false));

    // Find the largest eigenvalue module
    Scalar s = std::norm(eigenValues[0]);
    for (UnsignedInteger i = 1; i < eigenValues.getSize() ; ++i) s = std::max(s, std::norm(eigenValues[i]));
    // If the largest eigenvalue is not in the interior of the unit circle, the ARMA process is not stable

    result[constraintIndex] = 1.0 - std::sqrt(s) - epsilon;
    ++constraintIndex;
  }
  // If invertible and not pure AR, check the eigenValues of the MA polynom
  if (invertible_ && currentQ_ > 0)
  {
    // Companion matrix - Matrix is of size (dimension * p_)
    SquareMatrix matrix(dimension_ * currentQ_);
    for (UnsignedInteger coefficientIndex = 0; coefficientIndex < currentQ_ ; ++coefficientIndex)
    {
      for  (UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++ rowIndex)
      {
        for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++ columnIndex)
        {
          matrix( dimension_ * (currentQ_ - 1) +  rowIndex, coefficientIndex * dimension_ + columnIndex ) = beta[currentIndex];
          ++currentIndex;
        }
      }
    }

    // Incorporation into the previous for loop
    for (UnsignedInteger index = 0; index < dimension_ * (currentQ_ - 1); ++index)
    {
      matrix(index, dimension_ + index) = 1.0;
    }

    // Computation of EigenValues without keeping intact (matrix not used after)
    const Collection<Complex> eigenValues(matrix.computeEigenValues(false));

    // Find the largest eigenvalue module
    Scalar s = std::norm(eigenValues[0]);
    for (UnsignedInteger i = 1; i < eigenValues.getSize() ; ++i) s = std::max(s, std::norm(eigenValues[i]));
    // If the largest eigenvalue is not in the interior of the unit circle, criteria is not respected
    result[constraintIndex] = 1.0 - std::sqrt(s) - epsilon;
    ++constraintIndex;
  }

  // Check the positive character of the matrix
  CovarianceMatrix covarianceMatrix(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_; ++i)
    {
      covarianceMatrix(i, j) = beta[currentIndex];
      currentIndex += 1;
    }
  }
  // Computation of EigenValues without keeping intact (matrix not used after)
  const Point eigenValues(covarianceMatrix.computeEigenValues(false));

  // Find the largest eigenvalue module
  Scalar s = eigenValues[0];
  for (UnsignedInteger i = 1; i < eigenValues.getSize() ; ++i) s = std::min(s, eigenValues[i]);
  // Constrain is that the min eigenvalue upper than 0
  result[constraintIndex] = s - epsilon;

  return result;
}

/* Compute the log-likelihood function accessor */
Function ARMALikelihoodFactory::getLogLikelihoodFunction() const
{
  return bindMethod <ARMALikelihoodFactory, Scalar, Point> ( *this, &ARMALikelihoodFactory::computeLogLikelihood, inputDimension_, 1);
}

/* Compute the log-likelihood constraint accessor */
Function ARMALikelihoodFactory::getLogLikelihoodInequalityConstraint() const
{
  return bindMethod <ARMALikelihoodFactory, Point, Point> ( *this, &ARMALikelihoodFactory::computeLogLikelihoodInequalityConstraint, inputDimension_, nbInequalityConstraint_);
}

/* Initialize optimization solver parameter using the ResourceMap */
void ARMALikelihoodFactory::initializeCobylaSolverParameter()
{
  Cobyla* cobyla = dynamic_cast<Cobyla *>(solver_.getImplementation().get());
  if (cobyla == NULL) throw InternalException(HERE);

  cobyla->setRhoBeg(ResourceMap::GetAsScalar("ARMALikelihoodFactory-DefaultRhoBeg"));
  cobyla->setCheckStatus(false);

  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("ARMALikelihoodFactory-DefaultRhoEnd"));
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("ARMALikelihoodFactory-DefaultMaximumEvaluationNumber"));
}

/* Optimization solver accessor */
OptimizationAlgorithm ARMALikelihoodFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void ARMALikelihoodFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

// in-place Cholesky decomposition
int modifiedCholeskyDecomposition(SquareMatrix &matrix,
                                  Scalar epsilon)
{
  const UnsignedInteger n = matrix.getDimension();
  Scalar maxSqrtDiag = std::sqrt(std::abs(matrix(0, 0)));
  for (UnsignedInteger j = 1; j < n; ++j)
  {
    maxSqrtDiag = std::max(maxSqrtDiag, std::sqrt(std::abs(matrix(j, j))));
  }

  // The diagonal elements are too small
  if (maxSqrtDiag * maxSqrtDiag < epsilon)
  {
    matrix = SquareMatrix(n);
    return 1;
  }

  const Scalar minValue = maxSqrtDiag * std::sqrt(epsilon);
  for (UnsignedInteger j = 0; j < n; ++j)
  {
    Scalar value = matrix(j, j);
    for (UnsignedInteger i = 0; i < j; ++i)
    {
      value -= matrix(j, i) * matrix(j, i);
    }
    if (value < - minValue) return 1;
    matrix(j, j) = value;

    Scalar minJ = 0.0;
    for (UnsignedInteger i = j + 1; i < n; ++i)
    {
      value = matrix(j, i);
      for (UnsignedInteger k = 0; k < j; ++k)
      {
        value -= matrix(i, k) * matrix(j, k);
      }
      matrix(i, j) = value;
      if (std::abs(matrix(i, j)) > minJ) minJ = std::abs(matrix(i, j));
    }
    if (minJ / maxSqrtDiag > 0.0)
    {
      minJ /= maxSqrtDiag;
    }
    else
    {
      minJ = 0.0;
    }
    if (matrix(j, j) > minJ * minJ)
    {
      matrix(j, j) = std::sqrt(matrix(j, j));
    }
    else
    {
      if (minJ < minValue) minJ = minValue;
      matrix(j, j) = minJ;
    }
    for (UnsignedInteger i = j + 1; i < n; ++i)
    {
      matrix(i, j) /= matrix(j, j);
    }
  }
  for (UnsignedInteger j = 1; j < n; ++j)
  {
    for (UnsignedInteger i = 0; i < j; ++i)
    {
      matrix(i, j) = 0.0;
    }
  }

  return 0;
}

/* String converter */
String ARMALikelihoodFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ARMALikelihoodFactory::GetClassName()
      << " implementation=" << ARMAFactoryImplementation::__repr__()
      << " dimension=" << dimension_ ;
  return oss;
}

/* String converter */
String ARMALikelihoodFactory::__str__(const String & ) const
{
  return this->__repr__();
}

/* Build method */
ARMA ARMALikelihoodFactory::build(const TimeSeries & timeSeries) const
{
  // Currently the implementation of the factory bases on m estimations of univariate models, m is the dimension of the
  // above time series

  // Checking the size of time series
  if (timeSeries.getSize() < currentG_)
    throw InvalidArgumentException(HERE) << "Error : expected time series of size greater than " << currentG_;

  // passing time series to log likelihood function
  w_ = timeSeries;
  const UnsignedInteger dimension = w_.getOutputDimension();
  if (dimension != dimension_)
    throw InvalidDimensionException(HERE) << "Error : expected time series of dimension " << dimension_
                                          << " here time series is of dimension " << dimension;
  if (dimension_ == 1)
    return WhittleFactory(currentP_, currentQ_).build(timeSeries);

  // Calling the default initialization if no coefficients have been initialized
  if ((!hasInitializedARCoefficients_) || (!hasInitializedMACoefficients_) || (!hasInitializedCovarianceMatrix_))
    defaultInitialize();

  int n((currentP_ + currentQ_) * dimension_ * dimension_ + dimension_ * (dimension_ + 1) / 2 + 1);

  // constraints count. At least, check the covariance character
  int m(1);
  // Check the minimum eigenvalue module of the AR polynom only if not pure MA process
  if (currentP_ > 0) ++m;
  // Check the minimum eigenvalue module of the MA polynom only if not pure AR process and if invertible
  if (invertible_ && currentQ_ > 0) ++m;

  // Current parameters vector \beta
  Point beta(n);
  UnsignedInteger currentIndex = 0;
  for (UnsignedInteger k = 0; k < currentP_; ++k)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        beta[currentIndex] = blockPhiTThetaTMatrix_(k * dimension_ + j, i);
        currentIndex += 1;
      }
    }
  }

  for (UnsignedInteger k = 0; k < currentQ_; ++k)
  {
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        beta[currentIndex] = blockPhiTThetaTMatrix_((k  + currentP_) * dimension_ + j, i);
        currentIndex += 1;
      }
    }
  }

  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_; ++i)
    {
      // Openturns conventions about ARMA are different from those in Mauricio's papers.
      // Moreover in order to improve matrix-matrix computation, transpose matrices are
      // stored in blockPhiTThetaTMatrix_.
      beta[currentIndex] = covarianceMatrix_(i, j);
      currentIndex += 1;
    }
  }
  // initial sigma2
  beta[currentIndex] = sigma2_;
  // use attributes to pass the data
  nbInequalityConstraint_ = m;
  inputDimension_ = n;
  // Define Objective and Constraint functions for Optimization problem
  OptimizationProblem problem(getLogLikelihoodFunction());
  problem.setMinimization(false);
  problem.setInequalityConstraint(getLogLikelihoodInequalityConstraint());
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(beta);

  // run Optimization problem
  solver.run();

  // optimal point
  const Point optpoint(solver.getResult().getOptimalPoint());
  beta = optpoint;

  // Return result
  currentIndex = 0;
  ARMACoefficients phi(currentP_, dimension_);
  for (UnsignedInteger k = 0; k < currentP_; ++k)
  {
    SquareMatrix phi_k(dimension_);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        phi_k(i, j) =  - beta[currentIndex];
        ++currentIndex;
      }
    }
    phi[k] = phi_k;
  }

  ARMACoefficients theta(currentQ_, dimension_);
  for (UnsignedInteger k = 0; k < currentQ_; ++k)
  {
    SquareMatrix theta_k(dimension_);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        theta_k(i, j) =  - beta[currentIndex];
        ++currentIndex;
      }
    }
    theta[k] = theta_k;
  }

  CovarianceMatrix cov(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_; ++i)
    {
      cov(i, j) = beta[currentIndex];
      ++currentIndex;
    }
  }

  sigma2_ = beta[currentIndex];
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_; ++i)
    {
      cov(i, j) *= sigma2_;
      ++currentIndex;
    }
  }

  const Normal distribution(Point(dimension_), cov);
  const RegularGrid timeGrid(timeSeries.getTimeGrid());
  const WhiteNoise whiteNoise(distribution, timeGrid);
  return ARMA(phi, theta, whiteNoise);
}

/* Build method */
ARMA ARMALikelihoodFactory::build(const ProcessSample & ) const
{
  throw NotYetImplementedException(HERE) << "In ARMALikelihoodFactory::build(const ProcessSample & sample) const";
}

// Set accessor for starting point of the optimization
void ARMALikelihoodFactory::setInitialARCoefficients(const ARMACoefficients & phi)
{
  if (phi.getDimension() != dimension_)
    throw InvalidDimensionException(HERE) << "Error; dimension of the AR coefficients is not coherent";
  if (phi.getSize() != currentP_)
    throw InvalidDimensionException(HERE) << "Error; size of the AR coefficients is not coherent";

  for (UnsignedInteger k = 0; k < currentP_; ++k)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        // Openturns conventions about ARMA are different from those in Mauricio's papers.
        // Moreover in order to improve matrix-matrix computation, transpose matrices are
        // stored in blockPhiTThetaTMatrix_.
        blockPhiTThetaTMatrix_(k * dimension_ + j, i) = - phi[k](i, j);
      }
    }
  }
  hasInitializedARCoefficients_ = true;
}

/* Run the default initialization of coefficients / covariance for the optimization */
void ARMALikelihoodFactory::defaultInitialize() const
{
  // Launch d times the WhittleFactory to get initial conditions for the optimization problem
  WhittleFactory factory(currentP_, currentQ_);
  const Sample values(w_.getValues());
  const RegularGrid timeGrid(w_.getTimeGrid());
  for (UnsignedInteger d = 0; d < dimension_; ++d)
  {
    // Univariate estimate
    const TimeSeries marginal_d(timeGrid, values.getMarginal(d));
    ARMA marginalARMAModel(factory.build(marginal_d));
    const WhiteNoise whiteNoise(marginalARMAModel.getWhiteNoise());
    const Scalar sigma = whiteNoise.getDistribution().getStandardDeviation()[0];
    covarianceMatrix_(d, d) = sigma * sigma;
    const ARMACoefficients arCoefficients(marginalARMAModel.getARCoefficients());
    const ARMACoefficients maCoefficients(marginalARMAModel.getMACoefficients());
    // fill the coefficients
    for (UnsignedInteger k = 0; k < currentP_; ++k)
    {
      const Scalar value = arCoefficients[k](0, 0);
      blockPhiTThetaTMatrix_(k * dimension_ + d, d) = -value;
    }

    for (UnsignedInteger k = 0; k < currentQ_; ++k)
    {
      const Scalar value = maCoefficients[k](0, 0);
      blockPhiTThetaTMatrix_((currentP_ + k) * dimension_ + d, d) = -value;
    }
  }

  // Init done
  hasInitializedARCoefficients_ = true;
  hasInitializedMACoefficients_ = true;
  hasInitializedCovarianceMatrix_ = true;
}

void ARMALikelihoodFactory::setInitialMACoefficients(const ARMACoefficients & theta)
{
  if (theta.getDimension() != dimension_)
    throw InvalidDimensionException(HERE) << "Error; dimension of the MA coefficients is not coherent";
  if (theta.getSize() != currentQ_)
    throw InvalidDimensionException(HERE) << "Error; size of the MA coefficients is not coherent";

  for (UnsignedInteger k = 0; k < currentQ_; ++k)
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        // Openturns conventions about ARMA are different from those in Mauricio's papers
        // Moreover in order to improve matrix-matrix computation, transpose matrices are
        // stored in blockPhiTThetaTMatrix_.
        blockPhiTThetaTMatrix_((currentP_ + k) * dimension_ + j, i) =  - theta[k](i, j);
      }
    }
  }
  hasInitializedMACoefficients_ = true;
}

void ARMALikelihoodFactory::setInitialCovarianceMatrix(const CovarianceMatrix & covarianceMatrix)
{
  if (covarianceMatrix.getDimension() != covarianceMatrix_.getDimension())
    throw InvalidDimensionException(HERE) << "Error; dimension of the covariance matrix is not coherent";
  covarianceMatrix_ = covarianceMatrix;
  hasInitializedCovarianceMatrix_ = true;
}

//  Global set initial starting point method
void ARMALikelihoodFactory::setInitialConditions(const ARMACoefficients & arCoefficients,
    const ARMACoefficients & maCoefficients,
    const CovarianceMatrix & covarianceMatrix)
{
  setInitialARCoefficients(arCoefficients);
  setInitialMACoefficients(maCoefficients);
  setInitialCovarianceMatrix(covarianceMatrix);
}

ARMACoefficients ARMALikelihoodFactory::getInitialARCoefficients() const
{
  // Openturns conventions about ARMA are different from those in Mauricio's papers
  // Moreover in order to improve matrix-matrix computation, transpose matrices are
  // stored in blockPhiTThetaTMatrix_.
  ARMACoefficients phi(currentP_, dimension_);
  for (UnsignedInteger k = 0; k < currentP_; ++k)
  {
    SquareMatrix phi_k(phi[k]);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        phi_k(i, j) =  - blockPhiTThetaTMatrix_(k * dimension_ + j, i);
      }
    }
    phi[k] = phi_k;
  }
  return phi;
}

ARMACoefficients ARMALikelihoodFactory::getInitialMACoefficients() const
{
  // Openturns conventions about ARMA are different from those in Mauricio's papers
  // Moreover in order to improve matrix-matrix computation, transpose matrices are
  // stored in blockPhiTThetaTMatrix_.
  ARMACoefficients theta(currentQ_, dimension_);
  for (UnsignedInteger k = 0; k < currentQ_; ++k)
  {
    SquareMatrix theta_k(dimension_);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        theta_k(i, j) = - blockPhiTThetaTMatrix_((currentP_ + k) * dimension_ + j, i);
      }
    }
    theta[k] = theta_k;
  }
  return theta;
}

CovarianceMatrix ARMALikelihoodFactory::getInitialCovarianceMatrix() const
{
  return covarianceMatrix_;
}

/* Compute the cross-covariance matrix - This method is public for validation purposes */
void ARMALikelihoodFactory::computeCrossCovarianceMatrix() const
{
  // This method aims at computing the theoretical cross covariance matrix
  // It is based on the article of J.A.Mauricio
  // The result matrix crossCovariance_ is a matrix of size (dimension, (max(1,q) * dimension))
  // It contains \Lambda_0, \Lambda_{-1},...,\Lambda_{1-q}

  // Allocate size for cross covariance matrix
  // \Lambda_{0}, \Lambda_{-1},..., \Lambda_{1-q}
  // If q = 0 or 1, the only matrix to compute is \Lambda_0
  const UnsignedInteger one = 1;
  const UnsignedInteger size = std::max(one, currentQ_);
  crossCovariance_ = Matrix(dimension_, dimension_ * size);

  // \Lambda_0 is \Sigma
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_; ++i)
    {
      crossCovariance_(i, j) = covarianceMatrix_(i, j);
      crossCovariance_(j, i) = covarianceMatrix_(i, j);
    }
  }

  // Computation of \Lambda_{-k} = - \theta_{k} * \Sigma + \sum_{l=1}^{k} \phi_{l} * \Lambda_{l-k}
  // for k = 1,2,...,q-1
  if (currentQ_ > 1)
  {
    for (UnsignedInteger k = 1; k <= currentQ_ - 1; ++k)
    {
      // Convention : \phi_{l} = 0 for l > p
      // So we compute \phi_{l} * \Lambda_{l-k} only for l <= k and l <= p
      const UnsignedInteger lMax = std::min(k, currentP_);

      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        for (UnsignedInteger i = 0; i < dimension_; ++i)
        {
          //  - \theta_{k} * \Sigma in the article
          Scalar value = 0.0;
          for (UnsignedInteger h = 0; h < dimension_; ++h)
          {
            value -= blockPhiTThetaTMatrix_((currentP_ + k - 1) * dimension_ + h, i) * covarianceMatrix_(h, j);
          }
          // + \sum_{l=1}^{k} \phi_{l} * \Lambda_{l-k}
          for (UnsignedInteger l = 1; l <= lMax; ++l)
          {
            for (UnsignedInteger h = 0; h < dimension_; ++h)
            {
              value += blockPhiTThetaTMatrix_((l - 1) * dimension_ + h, i) * crossCovariance_(h, (k - l) * dimension_ + j);
            }
          }
          crossCovariance_(i, k * dimension_ + j) = value;
        }
      }
    }
  }

}

/* Compute the autocovariance matrix - This method is public for validation purposes*/
void ARMALikelihoodFactory::computeAutocovarianceMatrix() const
{
  // This method aims at computing the autocovariance matrix from data
  // The method is based on the articles of J.A.Mauricio
  // The objective here is to compute the matrix which allows the resolution of linear system for \Gamma
  // The p unknown theoretical autocovariance matrices \Gamma_{0},..., \Gamma_{p-1} are given as follows:
  // \Gamma_0 - \sum_{r=1}^{p} \phi_{r} \Gamma_0 \phi_{r}^{T} - \sum_{r=1}^{p-1} \sum_{s=1}^{p-r} [ \phi_{r+s} \Gamma_{r} \phi_{s}^{T} +  \phi_{s} \Gamma_{r}^{T} \phi_{r+s}^{T}] = W0
  // \Gamma_{k} - \sum_{r=1}^{k-1} \Gamma_{r} \phi_{k-r}^{T} - \sum_{r=0}^{p-k} \Gamma_{r}^{T} \phi_{k+r}^{T}  = Wk, k=1,..,p-1
  // with Wk = -\sum_{s=k}^{q} \Lambda_{k-s} \theta_{s}^{T}, k > 0 (stored in matrix rhs)
  //
  // The previous equations are rewritten as system of the form A v = b with v = vec(\Gamma_0,....,Gamma_{p-1})
  // From a theoretical point of view, since W0 is symmetric, only the m(m+1)/2 first elements are useful (m = dimension for simplification)
  // Memory allocation
  const UnsignedInteger one = 1;
  const UnsignedInteger arSize = std::max(one, currentP_);
  // Initializing the autocovariance matrix
  autoCovariance_ = Matrix(dimension_, arSize * dimension_);

  // Compute the W0 matrix
  // This matrix is symmetric, only its upper part is filled up in computeW0Matrix()
  const SquareMatrix W0(computeW0Matrix());

  // Distinction between case p = 0 and p > 0
  // if p = 0 ==> No linear system needed but only use of W0 matrix
  if (currentP_ == 0)
  {
    // Assembling \Gamma_{0}
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i <= j; ++i)
      {
        autoCovariance_(i, j) = W0(i, j);
        // Using the symetry
        autoCovariance_(j, i) = W0(i, j);
      }
    }
  }
  else
  {
    // Memory allocate for the rhs and matA matrices
    const UnsignedInteger max1P = std::max(one, currentP_);
    const UnsignedInteger size = dimension_ * dimension_ * (max1P - 1) + dimension_ * (dimension_ + 1) / 2;
    SquareMatrix matA(size);
    Point rhs(size);

    // Fill the first dimension_ * (dimension_ + 1) / 2 rows
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      for (UnsignedInteger i = 0; i <= j; ++i)
      {
        // Row index differ from the expression of the article since indice start from 0
        const UnsignedInteger rowIndex = j * (j + 1) / 2 + i;

        // Fill the first dimension_ * (dimension_ + 1) / 2 columns
        for (UnsignedInteger l = 0; l < dimension_; ++l)
        {
          for (UnsignedInteger k = 0; k <= l ; ++k)
          {
            const UnsignedInteger columnIndex = l * (l + 1) / 2 + k;
            Scalar value = 0.0;
            if (k == l)
            {
              // Computation of - \sum_{r=1}^{p} \phi_{r}(i, k) \phi_{r}(j, l)
              for (UnsignedInteger r = 1; r <= currentP_; ++r)
              {
                value -= blockPhiTThetaTMatrix_((r - 1) * dimension_ + k, i) * blockPhiTThetaTMatrix_((r - 1) * dimension_ + l, j);
              }
            }
            else
            {
              // Computation of - \sum_{r=1}^{p} \phi_{r}(i, k) \phi_{r}(j, l) + \phi_{r}(i, l) \phi_{r}(j, k)
              for (UnsignedInteger r = 1; r <= currentP_; ++r)
              {
                value -= blockPhiTThetaTMatrix_((r - 1) * dimension_ + k, i) * blockPhiTThetaTMatrix_((r - 1) * dimension_ + l, j) ;
                value -= blockPhiTThetaTMatrix_((r - 1) * dimension_ + l, i) * blockPhiTThetaTMatrix_((r - 1) * dimension_ + k, j) ;
              }
            }
            matA(rowIndex, columnIndex) = value;
          }
        }

        // Fill the remaining dimension_ * dimension_ * (currentP_ - 1) columns
        for (UnsignedInteger s = 1; s < currentP_; ++s)
        {
          for (UnsignedInteger l = 0; l < dimension_; ++l)
          {
            const UnsignedInteger columnIndex = dimension_ * (dimension_ + 1) / 2 + dimension_ * dimension_ * (s - 1) + dimension_ * l;
            for (UnsignedInteger k = 0; k < dimension_; ++k)
            {
              Scalar value = 0.0;
              // Computation of - \sum_{r=1}^{p-s} \phi_{r+s}(i, k) \phi_{r}(j, l) + \phi_{r+s}(j, k) \phi_{r}(i, l)
              for (UnsignedInteger r = 1; r <= currentP_ - s; ++r)
              {
                value -= blockPhiTThetaTMatrix_((r + s - 1) * dimension_ + k, i) * blockPhiTThetaTMatrix_((r - 1) * dimension_ + l, j);
                value -= blockPhiTThetaTMatrix_((r + s - 1) * dimension_ + k, j) * blockPhiTThetaTMatrix_((r - 1) * dimension_ + l, i);
              }
              matA(rowIndex, columnIndex + k) = value;
            }
          }
        }
        // Set up the diagonal of A and right hand side
        matA(rowIndex, rowIndex) += 1.0;
        rhs[rowIndex] = W0(i, j);
      }
    }

    // compute the remaining dimension_ * dimension_ * (currentP_ - 1) rows
    for (UnsignedInteger s = 1; s < currentP_; ++s)
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        for (UnsignedInteger j = 0; j < dimension_; ++j)
        {
          const UnsignedInteger rowIndex = dimension_ * (dimension_ + 1) / 2 + dimension_ * dimension_ * (s - 1) + dimension_ * i + j;

          // compute the first dimension_ * (dimension_ + 1) / 2 columns
          // Get the value : -\phi_{s}(i, l)
          UnsignedInteger columnIndex = j * (j + 1) / 2;
          for (UnsignedInteger l = 0; l <= j; ++l)
          {
            matA(rowIndex, columnIndex + l) = - blockPhiTThetaTMatrix_((s - 1) * dimension_ + l, i);
          }
          for (UnsignedInteger l = j + 1; l < dimension_; ++l)
          {
            matA(rowIndex, l * (l + 1) / 2 + j) = - blockPhiTThetaTMatrix_((s - 1) * dimension_ + l, i);
          }

          // compute the remaining dimension_ * dimension_ * (currentP_ - 1) columns
          for (UnsignedInteger r = 1; r < currentP_; ++r)
          {
            columnIndex = dimension_ * (dimension_ + 1) / 2 + dimension_ * dimension_ * (r - 1);
            for (UnsignedInteger l = 0; l < dimension_; ++l)
            {
              if (r + s <= currentP_)
                matA(rowIndex, columnIndex + dimension_ * j + l) = - blockPhiTThetaTMatrix_((r + s - 1) * dimension_ + l, i);
              if (s > r)
                matA(rowIndex, columnIndex + dimension_ * l + j) -= blockPhiTThetaTMatrix_((s - r - 1) * dimension_ + l, i);
            }
          }
          // Set up the diagonal
          matA(rowIndex, rowIndex) += 1.0;

          // compute the right-hand-side
          // Ws = -\sum_{h=s}^{q} \Lambda_{s-h} \theta_{h}^{T}
          for (UnsignedInteger h = s; h <= currentQ_; ++h)
          {
            Scalar value = 0.0;
            for (UnsignedInteger k = 0; k < dimension_; ++k)
            {
              value += crossCovariance_(j, (h - s) * dimension_ + k) * blockPhiTThetaTMatrix_((h + currentP_ - 1) * dimension_ + k, i);
            }
            rhs[rowIndex] -= value;
          }
        }
      }
    }

    // Solve the linear system
    const Point x(matA.solveLinearSystem(rhs));
    // Now we assemble the autocovariance matrices
    // Assembling \Gamma_{0}
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      for (UnsignedInteger j = i; j < dimension_; ++j)
      {
        const UnsignedInteger index = j * (j + 1) / 2 + i;
        autoCovariance_(i, j) = x[index];
        // Using the symmetry
        autoCovariance_(j, i) = x[index];
      }
    }

    // Assembling Gamma_{k}, k=1,...,p-1
    for (UnsignedInteger k = 1; k < currentP_; ++k)
    {
      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        for (UnsignedInteger i = 0; i < dimension_; ++i)
        {
          const UnsignedInteger index = dimension_ * (dimension_ + 1) / 2 + dimension_ * dimension_ * (k - 1) + dimension_ * j + i;
          autoCovariance_(i,  k * dimension_ + j) = x[index];
        }
      }
    }
  } //end else

}

SquareMatrix ARMALikelihoodFactory::computeW0Matrix() const
{
  // Compute W0 matrix in J.A.Mauricio notations (JAM295)
  // W0 = \Sigma  - (B + B^{T}) + \sum_{j=1}^{q} \theta_{j} * \Sigma * \theta_{j}^{T}
  // with B = \sum_{i=1}^{p} \sum_{j=i}^{q} \phi_{i} * \Lambda_{i-j} * \theta_{j}^{T}
  //
  // W0 is symmetric, compute only its upper part

  SquareMatrix wzero(dimension_);
  SquareMatrix matrixB(dimension_);
  SquareMatrix productPhiLambdaT(dimension_);
  for (UnsignedInteger i = 1; i <= currentP_; ++i)
  {
    for (UnsignedInteger j = i; j <= currentQ_; ++j)
    {
      for (UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
      {
        for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++columnIndex)
        {
          Scalar value = 0.0;
          for(UnsignedInteger k = 0; k < dimension_; ++k)
          {
            value += blockPhiTThetaTMatrix_((i - 1) * dimension_ + k, rowIndex) * crossCovariance_(k, (j - i) * dimension_ + columnIndex);
          }
          productPhiLambdaT(columnIndex, rowIndex) =  value;
        }
      }

      for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++columnIndex)
      {
        for (UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
        {
          Scalar value = 0.0;
          for(UnsignedInteger k = 0; k < dimension_; ++k)
          {
            value += productPhiLambdaT(k, rowIndex) * blockPhiTThetaTMatrix_((j - 1 + currentP_) * dimension_ + k, columnIndex);
          }
          matrixB(rowIndex, columnIndex) += value;
        }
      }
    }
  }

  // Now compute \Sigma  - (B + B^{T})
  // As the final matrix is symmetric, we fill only the upper part
  // The lower part is never used
  for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++columnIndex)
  {
    for (UnsignedInteger rowIndex = 0; rowIndex <= columnIndex; ++rowIndex)
    {
      wzero(rowIndex, columnIndex) = covarianceMatrix_(rowIndex, columnIndex) - matrixB(rowIndex, columnIndex) - matrixB(columnIndex, rowIndex);
    }
  }

  // Computation of \sum_{j=1}^{q} \theta_{j} * \Sigma * \theta_{j}^{T}
  for (UnsignedInteger j = 1; j <= currentQ_; ++j)
  {
    // Reuse matrixB to store intermediate result \theta_{j} * \Sigma
    for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++columnIndex)
    {
      for (UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
      {
        Scalar value = 0.0;
        for(UnsignedInteger k = 0; k < dimension_; ++k)
        {
          value += blockPhiTThetaTMatrix_((j - 1 + currentP_) * dimension_ + k, rowIndex) * covarianceMatrix_(k, columnIndex);
        }
        matrixB(rowIndex, columnIndex) =  value;
      }
    }

    for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++columnIndex)
    {
      for (UnsignedInteger rowIndex = 0; rowIndex <= columnIndex; ++rowIndex)
      {
        Scalar value = 0.0;
        for(UnsignedInteger k = 0; k < dimension_; ++k)
        {
          value += matrixB(rowIndex, k) * blockPhiTThetaTMatrix_((j - 1 + currentP_) * dimension_ + k, columnIndex);
        }
        wzero(rowIndex, columnIndex) += value;
      }
    }
  }

  return wzero;
}

/** The methods hereafter depends both on coefficients and time series values and or size */

/*
 * Step (c) of AS311: Compute the symmetric currentG_ * dimension_ matrix V1 * \Omega * V1^{T}
 */
SquareMatrix ARMALikelihoodFactory::computeV1_Omega_V1T_Cholesky() const
{
  Matrix omegaV1T((currentP_ + currentQ_) * dimension_, currentG_ * dimension_);
  for (UnsignedInteger i = 1; i <= currentP_; ++i)
  {
    for (UnsignedInteger j = 1; j <= currentG_; ++j)
    {
      // (Omega * V1^{T})_{ij} = \sum_{k=j-i}^{p-i} \Gamma_{k}\phi_{p-k-i+j}^{T}
      //                           - \sum_{k=j-i}^{q-i} \Lambda_{q-p-k}\theta_{q-k-i+j}^{T}
      for (int k = j - i; k + i <= currentP_ ; ++k)
      {
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
        {
          for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
          {
            Scalar value = 0.0;
            if (k >= 0)
            {
              for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
              {
                value += autoCovariance_(ii, k * dimension_ + kk) * blockPhiTThetaTMatrix_((currentP_ - k - i + j - 1) * dimension_ + kk, jj);
              }
            }
            else
            {
              for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
              {
                value += autoCovariance_(kk, (-k) * dimension_ + ii) * blockPhiTThetaTMatrix_((currentP_ - k - i + j - 1) * dimension_ + kk, jj);
              }
            }
            omegaV1T((i - 1) * dimension_ + ii, (j - 1) * dimension_ + jj) += value;
          }
        }
      }
      for (int k = j - i; k + i <= currentQ_ && k + currentP_ <= currentQ_; ++k)
      {
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
        {
          for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
          {
            Scalar value = 0.0;
            for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
            {
              value += crossCovariance_(ii, (currentQ_ - currentP_ - k) * dimension_ + kk) * blockPhiTThetaTMatrix_((currentP_ + currentQ_ - k - i + j - 1) * dimension_ + kk, jj);
            }
            omegaV1T((i - 1) * dimension_ + ii, (j - 1) * dimension_ + jj) -= value;
          }
        }
      }
    }
  }
  for (UnsignedInteger i = currentP_ + 1; i <= currentP_ + currentQ_; ++i)
  {
    for (UnsignedInteger j = 1; j <= currentG_; ++j)
    {
      // (Omega * V1^{T})_{ij} = \sum_{k=p+j-i}^{2p-i} \Lambda_{q-p+k}^{T}\phi_{2p-k-i+j}^{T} - \Sigma \theta_{q+p-i+j}^{T}
      for (int k = currentP_ + j - i; k + i <= 2 * currentP_ ; ++k)
      {
        if (currentP_ > k + currentQ_) continue;
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
        {
          for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
          {
            Scalar value = 0.0;
            for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
            {
              value += crossCovariance_(kk, (currentQ_ - currentP_ + k) * dimension_ + ii) * blockPhiTThetaTMatrix_((2 * currentP_ - k - i + j - 1) * dimension_ + kk, jj);
            }
            omegaV1T((i - 1) * dimension_ + ii, (j - 1) * dimension_ + jj) += value;
          }
        }
      }
      if ((currentP_ + j <= i) )
      {
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
        {
          for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
          {
            Scalar value = 0.0;
            for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
            {
              value += covarianceMatrix_(ii, kk) * blockPhiTThetaTMatrix_((2 * currentP_ + currentQ_ - i + j - 1) * dimension_ + kk, jj);
            }
            omegaV1T((i - 1) * dimension_ + ii, (j - 1) * dimension_ + jj) -= value;
          }
        }
      }
    }
  }
  // Now compute (V1 * Omega * V1^{T})_{ij} = \sum_{k=0}^{p-i} \phi_{p-k} E_{k+i,j}
  //                           - \sum_{k=0}^{q-i} \theta_{q-k} E_{k+p+i,j}
  SquareMatrix V1_Omega_V1T(dimension_ * currentG_);

  for (UnsignedInteger i = 1; i <= currentG_; ++i)
  {
    for (UnsignedInteger j = i; j <= currentG_; ++j)
    {
      for (int k = 0; k + i <= currentP_ ; ++k)
      {
        for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        {
          for(UnsignedInteger jj = ((i == j) ? ii : 0); jj < dimension_; ++jj)
          {
            Scalar value = 0.0;
            for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
            {
              value += blockPhiTThetaTMatrix_((currentP_ - k - 1) * dimension_ + kk, ii) * omegaV1T((k + i - 1) * dimension_ + kk, (j - 1) * dimension_ + jj);
            }
            V1_Omega_V1T((i - 1) * dimension_ + ii, (j - 1) * dimension_ + jj) += value;
          }
        }
      }
      for (int k = 0; k + i <= currentQ_ ; ++k)
      {
        for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        {
          for(UnsignedInteger jj = ((i == j) ? ii : 0); jj < dimension_; ++jj)
          {
            Scalar value = 0.0;
            for(UnsignedInteger kk = 0; kk < dimension_; ++kk)
            {
              value += blockPhiTThetaTMatrix_((currentP_ + currentQ_ - k - 1) * dimension_ + kk, ii) * omegaV1T((k + currentP_ + i - 1) * dimension_ + kk, (j - 1) * dimension_ + jj);
            }
            V1_Omega_V1T((i - 1) * dimension_ + ii, (j - 1) * dimension_ + jj) -= value;
          }
        }
      }
    }
  }

  // And compute the Cholesky decomposition
  Scalar smallestValue = 1.0;
  while (smallestValue + 1.0 > 1.0)
  {
    smallestValue *= 0.5;
  }
  smallestValue *= 2.0;
  modifiedCholeskyDecomposition(V1_Omega_V1T, smallestValue);
  return V1_Omega_V1T;
}

/*
 * Step (d) of AS311: Compute the \xi matrices:
 *   \xi_{k}(i,j), i = 1, 2, ..., dimension_, j = 1, 2, ... , dimension_, k = 0, 1, ... n-1
 * defined by \xi_{0} = Id and the relation
 *   \xi_{k} = \sum_{j=1}^{q} \theta_j \xi_{k-j}
 * Premultiply \xi matrices by R = Q1^{-1}
 * In this initial version, there is no truncation, see Mauricio's paper to see how
 * to implement it.
 */
Matrix ARMALikelihoodFactory::computeRXi() const
{
  UnsignedInteger size = w_.getSize();
  Matrix xi(dimension_, size * dimension_);

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    xi(i, i) = 1.0;
  }

  for (UnsignedInteger k = 1; k < size; ++k)
  {
    for (UnsignedInteger j = 1; j <= currentQ_ && j <= k; ++j)
    {
      for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
      {
        for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
        {
          Scalar value = 0.0;
          for(UnsignedInteger h = 0; h < dimension_; ++h)
          {
            value += blockPhiTThetaTMatrix_((currentP_ + j - 1) * dimension_ + h, ii) * xi(h, (k - j) * dimension_ + jj);
          }
          xi(ii, k * dimension_ + jj) += value;
        }
      }
    }
  }

  // Premultiply \xi by R
  return Matrix(covarianceMatrixCholeskyInverse_ * xi);
}

/*
 * Step (e) of AS311: Compute the n vectors (stored in a matrix for convenience)
 *   \eta_{i} = R a_{0,i}, i = 1, 2, ..., n-1
 * where a0 is defined by recurrence:
 *   a_{0,i} = w_{i} - \sum_{j=1}^{p} \phi_{j} w_{i-j} + \sum_{j=1}^{q} \theta_{j} a0_{i-j}
 */
Matrix ARMALikelihoodFactory::computeEta() const
{
  UnsignedInteger size = w_.getSize();
  Matrix a0(dimension_, size);

  for (UnsignedInteger i = 1; i <= size; ++i)
  {
    // Compute \sum_{j=1}^{p} \phi_{j} w_{i-j}
    Point phiW(dimension_);
    for (UnsignedInteger j = 1; j <= currentP_ && j < i; ++j)
    {
      Point point(w_.getValueAtIndex(i - j - 1));
      for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
      {
        Scalar value = 0.0;
        for(UnsignedInteger h = 0; h < dimension_; ++h)
        {
          value += blockPhiTThetaTMatrix_((j - 1) * dimension_ + h, ii) * point[h];
        }
        phiW[ii] += value;
      }
    }
    // Compute \sum_{j=1}^{q} \theta_{j} a0_{i-j}
    Point thetaA0(dimension_);
    for (UnsignedInteger j = 1; j <= currentQ_ && j < i; ++j)
    {
      for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
      {
        Scalar value = 0.0;
        for(UnsignedInteger h = 0; h < dimension_; ++h)
        {
          value += blockPhiTThetaTMatrix_((currentP_ + j - 1) * dimension_ + h, ii) * a0(h, i - j - 1);
        }
        thetaA0[ii] += value;
      }
    }
    // Compute a0_{i} = w_{i} - phiW + thetaA0;
    Point point(w_.getValueAtIndex(i - 1));
    for(UnsignedInteger ii = 0; ii < dimension_; ++ii)
    {
      for(UnsignedInteger h = 0; h < dimension_; ++h)
      {
        a0(h, i - 1) = point[h] - phiW[h] + thetaA0[h];
      }
    }
  }

  // Premultiply a0 by R
  return Matrix(covarianceMatrixCholeskyInverse_ * a0);
}

/*
 * Step (f) of AS311: Compute the g vectors (stored in a matrix for convenience)
 *   h_{j} = \sum_{i=0}^{n-j} \xi_{i}^{T} R^{T} \eta_{i+j}
 *         = \sum_{i=0}^{n-j} (R \xi_{i})^{T} \eta_{i+j}
 * and premultiply by M^{T}
 */
Point ARMALikelihoodFactory::computeVectorh(const Matrix & rxi,
    const Matrix & eta,
    const Matrix & matV1_Omega_V1TCholesky) const
{
  UnsignedInteger size = w_.getSize();
  Point vector_h(dimension_ * currentG_);

  // size condition in the loop is due to possible truncation
  for (UnsignedInteger j = 1; j <= currentG_ && j <= size; ++j)
  {
    for (UnsignedInteger i = 0; i <= size - j; ++i)
    {
      for(UnsignedInteger jj = 0; jj < dimension_; ++jj)
      {
        Scalar value = 0.0;
        for(UnsignedInteger k = 0; k < dimension_; ++k)
        {
          value += rxi(k, i * dimension_ + jj) * eta(k, i + j - 1);
        }
        vector_h[jj + (j - 1) * dimension_] += value;
      }
    }
  }

  // Premultiply vector_h by M^{T}
  Point vector_MTh(dimension_ * currentG_);
  for (UnsignedInteger i = 0; i < dimension_ * currentG_; ++i)
  {
    Scalar value = 0.0;
    for (UnsignedInteger k = 0; k < dimension_ * currentG_; ++k)
    {
      value += matV1_Omega_V1TCholesky(k, i) * vector_h[k];
    }
    vector_MTh[i] = value;
  }
  return vector_MTh;
}

/*
 * Step (g) of AS311: Compute the symmetric matrix dimension_ * currentG_ H^{T} H
 * This is a block matrix, defined by:
 *  (H^{T} H)_{i,1} = \sum_{k=0}^{n-i} (R \xi_{k})^{T} R \xi_{k+i-1} for i=1, 2, ..., g
 * and by the recurrence relation
 *  (H^{T} H)_{i,j} = (H^{T} H)_{i-1,j-1} - (R \xi_{n-i+1})^{T} R \xi_{n-j+1} for i=2, 3, ..., g and j=2, ", ..., i
 */
SymmetricMatrix ARMALikelihoodFactory::computeHTH(const Matrix & rxi) const
{
  UnsignedInteger size = w_.getSize();
  SymmetricMatrix matrix_HTH(dimension_ * currentG_);
  // First block
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
    {
      for (UnsignedInteger ii = jj; ii < dimension_; ++ii)
      {
        Scalar value = 0.0;
        for (UnsignedInteger kk = 0; kk < dimension_; ++kk)
        {
          value += rxi(kk, k * dimension_ + ii) * rxi(kk, k * dimension_ + jj);
        }
        matrix_HTH(ii, jj) += value;
      }
    }
  }
  // Other blocks on the first column
  // size condition in the loop is due to possible truncation of rxi
  for (UnsignedInteger i = 2; i <= currentG_ && i <= size; ++i)
  {
    for (UnsignedInteger k = 0; k <= size - i; ++k)
    {
      for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
      {
        for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        {
          Scalar value = 0.0;
          for (UnsignedInteger kk = 0; kk < dimension_; ++kk)
          {
            value += rxi(kk, k * dimension_ + ii) * rxi(kk, (k + i - 1) * dimension_ + jj);
          }
          matrix_HTH(ii + (i - 1) * dimension_, jj) += value;
        }
      }
    }
  }
  // Other diagonal blocks
  // size condition in the loop is due to possible truncation of rxi
  for (UnsignedInteger i = 2; i <= currentG_ && i <= size + 1; ++i)
  {
    for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
    {
      for (UnsignedInteger ii = jj; ii < dimension_; ++ii)
      {
        Scalar value = 0.0;
        for (UnsignedInteger kk = 0; kk < dimension_; ++kk)
        {
          value += rxi(kk, (size - i + 1) * dimension_ + ii) * rxi(kk, (size - i + 1) * dimension_ + jj);
        }
        matrix_HTH(ii + (i - 1) * dimension_, jj + (i - 1) * dimension_) =
          matrix_HTH(ii + (i - 2) * dimension_, jj + (i - 2) * dimension_) - value;
      }
    }
  }
  // Other subdiagonal blocks
  // size condition in the loop is due to possible truncation of rxi
  for (UnsignedInteger i = 2; i <= currentG_ && i <= size + 1; ++i)
  {
    for (UnsignedInteger j = 2; j < i; ++j)
    {
      for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
      {
        for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        {
          Scalar value = 0.0;
          for (UnsignedInteger kk = 0; kk < dimension_; ++kk)
          {
            value += rxi(kk, (size - i + 1) * dimension_ + ii) * rxi(kk, (size - j + 1) * dimension_ + jj);
          }
          matrix_HTH(ii + (i - 1) * dimension_, jj + (j - 1) * dimension_) =
            matrix_HTH(ii + (i - 2) * dimension_, jj + (j - 2) * dimension_) - value;
        }
      }
    }
  }

  return matrix_HTH;
}

/*
 * Step (h) of AS311: Compute the symmetric matrix (dimension_ * currentG_) (I + M^{T} H^{T} H M)
 */
CovarianceMatrix ARMALikelihoodFactory::computeI_MTHTHM(const SymmetricMatrix & matrix_HTH,
    const Matrix & matV1_Omega_V1TCholesky) const
{
  CovarianceMatrix matrixI_MTHTHM(dimension_ * currentG_);
  // Compute M^{T} H^{T} H
  Matrix matrix_MTHTH(matV1_Omega_V1TCholesky.transpose() * matrix_HTH);
  Matrix matrix_MTHTHM(matrix_MTHTH * matV1_Omega_V1TCholesky);
  for (UnsignedInteger j = 0; j < dimension_ * currentG_; ++j)
  {
    for (UnsignedInteger i = j; i < dimension_ * currentG_; ++i)
    {
      matrixI_MTHTHM(i, j) += matrix_MTHTHM(i, j);
    }
  }
  return matrixI_MTHTHM;
}

/* Method save() stores the object through the StorageManager */
void ARMALikelihoodFactory::save(Advocate & adv) const
{
  ARMAFactoryImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_);
}

/* Method load() reloads the object from the StorageManager */
void ARMALikelihoodFactory::load(Advocate & adv)
{
  ARMAFactoryImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_);
}

END_NAMESPACE_OPENTURNS
