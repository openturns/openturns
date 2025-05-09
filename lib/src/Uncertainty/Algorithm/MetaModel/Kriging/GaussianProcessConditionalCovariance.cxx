//                                               -*- C++ -*-
/**
 *  @brief The postprocessing of a GPR result (conditional covariance)
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/AggregatedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessConditionalCovariance)
static const Factory<GaussianProcessConditionalCovariance> Factory_GaussianProcessConditionalCovariance;

/* Default constructor */
GaussianProcessConditionalCovariance::GaussianProcessConditionalCovariance()
  : PersistentObject()
  , result_()
{
  // Nothing to do
}


/* Constructor with parameters & Cholesky factor */
GaussianProcessConditionalCovariance::GaussianProcessConditionalCovariance(const GaussianProcessRegressionResult & result)
  : PersistentObject()
  , result_(result)
{
  computePhi();
}


/* Virtual constructor */
GaussianProcessConditionalCovariance * GaussianProcessConditionalCovariance::clone() const
{
  return new GaussianProcessConditionalCovariance(*this);
}


/* String converter */
String GaussianProcessConditionalCovariance::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", GPR Result=" << result_;
}

String GaussianProcessConditionalCovariance::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  oss << "gpr result=" << result_;
  return oss;
}

/* Compute cross matrix method ==> not necessary square matrix  */
void GaussianProcessConditionalCovariance::computePhi()
{
  // regression matrix F
  const Matrix F(result_.getRegressionMatrix());
  if (F.getNbColumns() == 0) return;
  // Nothing to do if the design matrix has already been computed
  LOGINFO("Solve linear system  L * phi= F");
  const Matrix phi(solveTriangularSystem(F));
  // Compute QR decomposition of Phi_
  LOGINFO("Compute the QR decomposition of phi");
  Matrix G;
  (void) phi.computeQR(G);
  Gt_ = G.transpose();
  phiT_ = phi.transpose();
}

Matrix GaussianProcessConditionalCovariance::solveTriangularSystem(const Matrix & rhs) const
{
  Matrix result;
  const GaussianProcessFitterResult::LinearAlgebra method = result_.getLinearAlgebraMethod();
  if (method == GaussianProcessFitterResult::LAPACK)
  {
    result = result_.getCholeskyFactor().solveLinearSystem(rhs);
  }
  else
  {
    result = result_.getHMatCholeskyFactor().solveLower(rhs);
  }
  return result;
}

/* Compute mean of new points conditionally to observations */
Sample GaussianProcessConditionalCovariance::getConditionalMean(const Sample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned matrix should have dimensions (p * s) x (p * s)
  const UnsignedInteger inputDimension = xi.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMean, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMean, expected a non empty sample";
  const Function metaModel(result_.getMetaModel());
  // Use of metamodel to return result
  // Need to think if it is required to implement a specific method
  // in order to avoid data copy
  // sample is of size xi.getSize() * covarianceModel.getDimension()
  return metaModel.operator()(xi);
}

/* Compute mean of new points conditionally to observations */
Point GaussianProcessConditionalCovariance::getConditionalMean(const Point & xi) const
{
  // Use of meta model evaluation
  const Point output(result_.getMetaModel().operator()(xi));
  return output;
}


/* Compute covariance matrix conditionally to observations*/
CovarianceMatrix GaussianProcessConditionalCovariance::getConditionalCovariance(const Sample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned matrix should have dimensions (p * s) x (p * s)
  const Basis basis(result_.getBasis());
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  const UnsignedInteger inputDimension = xi.getDimension();
  const UnsignedInteger outputDimension = covarianceModel.getOutputDimension();
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalCovariance, expected a non empty sample";
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";

  // 1) compute \sigma_{x,x}
  LOGINFO("Compute interactions Sigma_xx");
  const CovarianceMatrix sigmaXX(covarianceModel.discretize(xi));

  // 2) compute \sigma_{y,x}
  // compute r(x), the crossCovariance between the conditioned data & xi
  LOGINFO("Compute cross-interactions sigmaYX");
  const Matrix crossCovariance(covarianceModel.computeCrossCovariance(result_.getInputSample(), xi));
  // 3) Compute r^t R^{-1} r'(x)
  // As we get the Cholesky factor L, we can solve triangular linear system
  // We define B = L^{-1} * r(x)
  LOGINFO("Solve L.B = SigmaYX");
  const Matrix B(solveTriangularSystem(crossCovariance));
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
  CovarianceMatrix result((sigmaXX - BtB).getImplementation());

  if (basis.getSize() > 0)
    {
  // Case of universal Kriging: compute the covariance due to the regression part
  // Additional information have to be computed
  // 1) compute F
  LOGINFO("Compute the regression matrix F");
  // 2) Interest is (F^t R^{-1} F)^{-1}
  // F^{t} R^{-1} F = F^{t} L^{-t} L^{-1} F
  // Solve first L phi = F
  // 3) Compute u(x) = F^t *R^{-1} * r(x) - f(x)
  //                 = F^{t} * L^{-1}^t * L{-1} * r(x) - f(x)
  //                 = phiT_ * B - f(x)
  LOGINFO("Compute psi = phi^t * B");
  Matrix ux(phiT_ * B);
  // compute f(x) & define u = psi - f(x)
  LOGINFO("Compute f(x) & ux = psi - fx");
  const UnsignedInteger basisSize = basis.getSize();
  // Basis \Phi is a function from R^{inputDimension} to R^{outputDimension}
  // As we get B functions, total number of values is B * outputDimension
  // Compute fx & ux = ux - fx
  for (UnsignedInteger j = 0; j < basisSize; ++j)
  {
    // Compute phi_j (X)
    // Here we use potential parallelism in the evaluation of the basis functions
    // It generates a sample of shape (sampleSize, outputDimension)
    const Sample basisSample(basis[j](xi));
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
      for (UnsignedInteger outputMarginal = 0; outputMarginal < outputDimension; ++outputMarginal)
        ux(j * outputDimension + outputMarginal, outputMarginal + i * outputDimension) -= basisSample(i, outputMarginal);
  }

  // interest now is to solve  G rho = ux
  LOGINFO("Solve linear system G * rho = ux");
  const Matrix rho(Gt_.solveLinearSystem(ux));
  LOGINFO("Compute Sigma_xx-BtB + rho^{t}*rho");
  result = result + rho.computeGram(true);
    }
  // now check if the result has positive diagonal elements
  Scalar smallest = 0.0;
  for (UnsignedInteger i = 0; i < result.getDimension(); ++i)
    {
      const Scalar dII = result(i, i);
      if (dII < smallest) smallest = dII;
    }
  // If the smallest diagonal element is negative, shift the whole diagonal
  // in order to make it zero
  if (smallest < 0.0)
    {
      for (UnsignedInteger i = 0; i < result.getDimension(); ++i)
	result(i, i) -= smallest;
    }
  return result;
}

/* Compute covariance matrix conditionally to observations*/
CovarianceMatrix GaussianProcessConditionalCovariance::getConditionalCovariance(const Point & point) const
{
  const UnsignedInteger inputDimension = point.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  const Sample pointAsSample(1, point);
  return getConditionalCovariance(pointAsSample);
}

/** Compute covariance matrices conditionally to observations (1 cov / point)*/
GaussianProcessConditionalCovariance::CovarianceMatrixCollection GaussianProcessConditionalCovariance::getDiagonalCovarianceCollection(const Sample & xi) const
{
  // For a process of dimension p & xi's size=s,
  // returned a s-collection of cov matrices (pxp)
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  const UnsignedInteger inputDimension = xi.getDimension();
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getDiagonalCovarianceCollection, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getDiagonalCovarianceCollection, expected a non empty sample";

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
CovarianceMatrix GaussianProcessConditionalCovariance::getDiagonalCovariance(const Point & xi) const
{
  const UnsignedInteger inputDimension = xi.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getDiagonalCovariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  return getConditionalCovariance(xi);
}

/** Compute marginal variance conditionally to observations (1 cov of size outputDimension)*/
Scalar GaussianProcessConditionalCovariance::getConditionalMarginalVariance(const Point & point,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = point.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  const UnsignedInteger outputDimension = covarianceModel.getOutputDimension();
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  if ( !(marginalIndex < outputDimension))
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, marginalIndex should be in [0," << outputDimension << "]. Here, marginalIndex = " << marginalIndex ;
  // Compute the matrix & return only the marginalIndex diagonal element
  const CovarianceMatrix marginalCovarianceMatrix(getDiagonalCovariance(point));
  return marginalCovarianceMatrix(marginalIndex, marginalIndex);
}

/** Compute marginal variance conditionally to observations (1 cov / point)*/
Sample GaussianProcessConditionalCovariance::getConditionalMarginalVariance(const Sample & xi,
    const UnsignedInteger marginalIndex) const
{

  const UnsignedInteger inputDimension = xi.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  const UnsignedInteger outputDimension = covarianceModel.getOutputDimension();
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  if ( !(marginalIndex < outputDimension))
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, marginalIndex should be in [0," << outputDimension << "]. Here, marginalIndex = " << marginalIndex ;
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, expected a non empty sample";

  if  (outputDimension == 1)
  {
    // 1) compute \sigma_{x,x}
    LOGINFO("Compute interactions Sigma_xx");
    // Only diagonal of the discretization Matrix
    // First set sigmaXX
    const Point defaultPoint(inputDimension);
    const Point sigma2(1, covarianceModel.computeAsScalar(defaultPoint, defaultPoint));
    Sample result(sampleSize, sigma2);


    // 2) compute \sigma_{y,x}
    // compute r(x), the crossCovariance between the conditioned data & xi
    LOGINFO("Compute cross-interactions sigmaYX");
    const Matrix crossCovariance(covarianceModel.computeCrossCovariance(result_.getInputSample(), xi));
    // 3) Compute r^t R^{-1} r'(x)
    // As we get the Cholesky factor L, we can solve triangular linear system
    // We define B = L^{-1} * r(x)
    LOGINFO("Solve L.B = SigmaYX");
    const Matrix B(solveTriangularSystem(crossCovariance));
    // We compute diag(B^t B)
    // We can notice that it corresponds to the sum of elements
    // for each column
    LOGINFO("Compute B^tB & Sigma_xx-BtB");
    for (UnsignedInteger j = 0; j < B.getNbColumns(); ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < B.getNbRows(); ++i)
        sum += B(i, j) * B(i, j);
      result(j, 0) -= sum;
    }

    const Basis basis(result_.getBasis());
    if (basis.getSize() > 0)
      {
    // Case of universal Kriging: compute the covariance due to the regression part
    // Additional information have to be computed
    // 1) compute F
    LOGINFO("Compute the regression matrix F");
    // 2) Interest is (F^t R^{-1} F)^{-1}
    // F^{t} R^{-1} F = F^{t} L^{-t} L^{-1} F
    // Solve first L phi = F
    // 3) Compute u(x) = F^t *R^{-1} * r(x) - f(x)
    //                 = F^{t} * L^{-1}^t * L{-1} * r(x) - f(x)
    //                 = phiT_ * B - f(x)
    LOGINFO("Compute psi = phi^t * B");
    // ux playing the role of psi
    Matrix ux(phiT_ * B);
    // compute f(x) & define u = psi - f(x)
    LOGINFO("Compute f(x) & ux = psi - fx");
    for (UnsignedInteger j = 0; j < basis.getSize(); ++j)
    {
      // Compute phi_j (X)
      // Here we use potential parallelism in the evaluation of the basis functions
      // It generates a sample of shape (sampleSize, outputDimension)
      const Sample basisSample(basis[j](xi));
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
        ux(j,  i) -= basisSample(i, 0);
    }
    // interest now is to solve  G rho = ux
    LOGINFO("Solve linear system G * rho = ux");
    const Matrix rho(Gt_.solveLinearSystem(ux));
    LOGINFO("Compute Sigma_xx-BtB + rho^{t}*rho");
    for (UnsignedInteger j = 0; j < rho.getNbColumns(); ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < rho.getNbRows(); ++i)
        sum += rho(i, j) * rho(i, j);
      result(j, 0) += sum;
    }
      } // universal kriging
    // now check if the result has positive elements
    Scalar smallest = 0.0;
    for (UnsignedInteger i = 0; i < result.getSize(); ++i)
      {
	const Scalar varI = result(i, 0);
	if (varI < smallest) smallest = varI;
      }
    // If the smallest element is negative, shift the whole sample
    // in order to make it zero
    if (smallest < 0.0)
      {
	for (UnsignedInteger i = 0; i < result.getSize(); ++i)
	  result(i, 0) -= smallest;
      }
    return result;
  } // end for if  outputdim=1

  // Run sequentially over the sample
  Sample marginalVariance(sampleSize, 1);
  Point data(inputDimension);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j] = xi(i, j);
    marginalVariance(i, 0) = getConditionalMarginalVariance(data, marginalIndex);
  }
  return marginalVariance;
}

Point GaussianProcessConditionalCovariance::getConditionalMarginalVariance(const Point & point,
    const Indices &indices) const
{
  const UnsignedInteger inputDimension = point.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  if (!indices.check(covarianceModel.getOutputDimension()))
    throw InvalidArgumentException(HERE) << "In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, the indices of a marginal sample must be in the range [0," << covarianceModel.getOutputDimension()
                                         << " ] and must be different";
  // Compute the matrix & return only the marginalIndex diagonal element
  const CovarianceMatrix covarianceMatrix(getDiagonalCovariance(point));
  Point result(indices.getSize());
  for (UnsignedInteger j = 0; j < indices.getSize(); ++j) result[j] = covarianceMatrix(indices[j], indices[j]);
  return result;
}

Sample GaussianProcessConditionalCovariance::getConditionalMarginalVariance(const Sample & xi,
    const Indices & indices) const
{

  const UnsignedInteger inputDimension = xi.getDimension();
  const CovarianceModel covarianceModel(result_.getCovarianceModel());
  if (inputDimension != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, input data should have the same dimension as covariance model's input dimension. Here, (input dimension = " << inputDimension << ", covariance model spatial's dimension = " << covarianceModel.getInputDimension() << ")";
  if (!indices.check(covarianceModel.getOutputDimension()))
    throw InvalidArgumentException(HERE) << "In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, the indices of a marginal sample must be in the range [0," << covarianceModel.getOutputDimension()
                                         << " ] and must be different";
  const UnsignedInteger sampleSize = xi.getSize();
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << " In GaussianProcessConditionalCovariance::getConditionalMarginalVariance, expected a non empty sample";
  // Compute the matrix & return only the marginalIndex diagonal element
  Sample result(inputDimension, indices.getSize());
  Point data(inputDimension);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j] = xi(i, j);
    result[i] = getConditionalMarginalVariance(data, indices);
  }
  return result;
}


/* Compute joint normal distribution conditionally to observations*/
Normal GaussianProcessConditionalCovariance::operator()(const Point & xi) const
{
  const Sample sample(1, xi);
  return operator()(sample);
}

/* Compute joint normal distribution conditionally to observations*/
Normal GaussianProcessConditionalCovariance::operator()(const Sample & xi) const
{
  // The Normal distribution is defined by its mean & covariance
  LOGINFO("In GaussianProcessConditionalCovariance::operator() : evaluating the mean");
  const Sample meanAsSample(getConditionalMean(xi));
  // Mean should be a Point ==> data are copied form the Sample to a Point
  const Point mean(meanAsSample.getImplementation()->getData());
  LOGINFO("In GaussianProcessConditionalCovariance::operator() : evaluating the covariance");
  const CovarianceMatrix covarianceMatrix = getConditionalCovariance(xi);
  // Check the covariance matrix. Indeed, if point is very similar to one of the learning points, covariance is null
  // Even if this check is done in Normal::Normal, we perform debugging
  LOGINFO("In GaussianProcessConditionalCovariance::operator() : evaluating the Normal distribution");
  // Finally return the distribution
  return Normal(mean, covarianceMatrix);
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessConditionalCovariance::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "phiT_", phiT_);
  adv.saveAttribute( "Gt_", Gt_);
}


/* Method load() reloads the object from the StorageManager */
void GaussianProcessConditionalCovariance::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "phiT_", phiT_);
  adv.loadAttribute( "Gt_", Gt_);
}



END_NAMESPACE_OPENTURNS
