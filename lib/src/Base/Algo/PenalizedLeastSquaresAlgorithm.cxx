//                                               -*- C++ -*-
/**
 *  @brief This Penalized Least Squares Algorithm as a functor class
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
#include <algorithm>
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PenalizedLeastSquaresAlgorithm)

static const Factory<PenalizedLeastSquaresAlgorithm> Factory_PenalizedLeastSquaresAlgorithm;


/* Default constructor */
PenalizedLeastSquaresAlgorithm::PenalizedLeastSquaresAlgorithm(const Bool useNormal)
  : ApproximationAlgorithmImplementation()
  , useNormal_(useNormal)
{
  // Nothing to do
}

/* Parameters constructor */
PenalizedLeastSquaresAlgorithm::PenalizedLeastSquaresAlgorithm(const Sample & x,
    const Sample & y,
    const FunctionCollection & psi,
    const Indices & indices,
    const Scalar penalizationFactor,
    const Bool useNormal)
  : ApproximationAlgorithmImplementation( x, y, psi, indices )
  , penalizationFactor_(penalizationFactor)
  , penalizationMatrix_(0)
  , useNormal_(useNormal)
{
  // If the penalization factor is strictly positive, use the identity matrix as a penalization term
  if (penalizationFactor > 0.0)
  {
    const UnsignedInteger basisSize = indices.getSize();
    penalizationMatrix_ = IdentityMatrix(basisSize);
  }
}


/* Parameters constructor */
PenalizedLeastSquaresAlgorithm::PenalizedLeastSquaresAlgorithm(const Sample & x,
    const Sample & y,
    const Point & weight,
    const FunctionCollection & psi,
    const Indices & indices,
    const Scalar penalizationFactor,
    const Bool useNormal)
  : ApproximationAlgorithmImplementation( x, y, weight, psi, indices )
  , penalizationFactor_(penalizationFactor)
  , penalizationMatrix_(0)
  , useNormal_(useNormal)
{
  // If the penalization factor is strictly positive, use the identity matrix as a penalization term
  if (penalizationFactor > 0.0)
  {
    const UnsignedInteger basisSize = indices.getSize();
    penalizationMatrix_ = IdentityMatrix(basisSize);
  }
}

/* Parameters constructor */
PenalizedLeastSquaresAlgorithm::PenalizedLeastSquaresAlgorithm(const Sample & x,
    const Sample & y,
    const Point & weight,
    const FunctionCollection & psi,
    const Indices & indices,
    const Scalar penalizationFactor,
    const CovarianceMatrix & penalizationMatrix,
    const Bool useNormal)
  : ApproximationAlgorithmImplementation( x, y, weight, psi, indices )
  , penalizationFactor_(penalizationFactor)
  , penalizationMatrix_(penalizationMatrix)
  , useNormal_(useNormal)
{
  const UnsignedInteger basisSize = indices.getSize();
  // Check if the penalization matrix has the proper dimension
  if (penalizationMatrix_.getDimension() != basisSize) throw InvalidArgumentException(HERE) << "Error: the given penalization matrix has an improper dimension: " << penalizationMatrix_.getDimension();
  if (!penalizationMatrix_.isPositiveDefinite()) throw NotSymmetricDefinitePositiveException(HERE) << "Error: the given penalization matrix is not positive definite.";
}

void PenalizedLeastSquaresAlgorithm::run()
{
  ApproximationAlgorithmImplementation::run();
}

/* Perform the optimization */
void PenalizedLeastSquaresAlgorithm::run(const DesignProxy & proxy)
{
  const UnsignedInteger basisDimension = currentIndices_.getSize();
  const UnsignedInteger sampleSize = x_.getSize();
  // Here we use directly a MatrixImplementation in order to have access to
  // the flat indexing of the data
  MatrixImplementation basisMatrix;
  Point rightHandSide;
  // We have four cases:
  // + no penalization and uniform weights
  // + no penalization and non-uniform weights
  // + penalization and uniform weights
  // + penalization and non-uniform weights
  // The penalization affects the size of the matrix and the right-hand side
  // The non-uniform weights impose additional computations

  // Starting from here, we build the matrix and the left-hand side of the least-squares problem
  // First, the most usual case: no penalization
  if (penalizationFactor_ == 0.0)
  {
    // First, get the data as if the weights were uniform
    basisMatrix = *proxy.computeDesign(currentIndices_).getImplementation();
    rightHandSide = y_.getImplementation()->getData();
    // If the weights are not uniform some additional work is needed
    if (!hasUniformWeight_)
    {
      // Now we take the weights into account. It is better to access the matrix in a column-wise fashion, so we precompute the square-roots of the weights as they act on a row-wise fashion on the design matrix. We scale the right-hand side in the same loop.
      Point weightSqrt(sampleSize);
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
      {
        weightSqrt[i] = std::sqrt(weight_[i]);
        rightHandSide[i] *= weightSqrt[i];
      }
      // Update the design matrix
      UnsignedInteger flatIndex = 0;
      for (UnsignedInteger j = 0; j < basisDimension; ++j)
        for (UnsignedInteger i = 0; i < sampleSize; ++i)
        {
          basisMatrix[flatIndex] *= weightSqrt[i];
          ++flatIndex;
        }
    } // (!hasUniformWeight_)
  } // (penalizationFactor_ == 0.0)
  else
    // (penalizationFactor_ > 0.0)
  {
    // Here we have a more complex situation: the penalized least-squares problem is reduced to a classical one by extending the matrix
    // First, get the data as if the weights were uniform
    const UnsignedInteger newNbRows = sampleSize + basisDimension;
    basisMatrix = MatrixImplementation(newNbRows, basisDimension);
    const MatrixImplementation Phi(*proxy.computeDesign(currentIndices_).getImplementation());
    rightHandSide = y_.getImplementation()->getData();
    rightHandSide.resize(newNbRows);
    // Extend the right-hand side with zeros
    std::fill(rightHandSide.begin() + sampleSize, rightHandSide.end(), 0.0);
    // Fill the upper part of the basisMatrix with the design matrix
    MatrixImplementation::const_iterator startPhi(Phi.begin());
    MatrixImplementation::iterator startBasisMatrix(basisMatrix.begin());
    for (UnsignedInteger j = 0; j < basisDimension; ++j)
    {
      MatrixImplementation::const_iterator stopPhi(startPhi + sampleSize);
      std::copy(startPhi, stopPhi, startBasisMatrix);
      startPhi = stopPhi;
      startBasisMatrix += newNbRows;
    }
    const TriangularMatrix penalizationCholesky(penalizationMatrix_.computeCholesky());
    // If the weights are uniform, they are taken into account by a change in the penalization factor
    if (hasUniformWeight_)
    {
      const Scalar rho = std::sqrt(penalizationFactor_ / weight_[0]);
      for (UnsignedInteger i = 0; i < basisDimension; ++i)
      {
        // The cholesky factor has to be transposed, thus we fill only the upper triangular part of the trailing block
        for (UnsignedInteger j = i; j < basisDimension; ++j)
          basisMatrix(sampleSize + i, j) = penalizationCholesky(j, i) * rho;
      }
    } // (hasUniformWeight_)
    else
    {
      const Scalar rho = std::sqrt(penalizationFactor_);
      // Here the upper part of the matrix and the right-hand side have to be changed to take into account the weights, the lower part to take into account the regularization
      Point weightSqrt(sampleSize);
      for (UnsignedInteger i = 0; i < sampleSize; ++i)
      {
        weightSqrt[i] = std::sqrt(weight_[i]);
        rightHandSide[i] *= weightSqrt[i];
      }
      // Update the design matrix
      UnsignedInteger flatIndex = 0;
      for (UnsignedInteger j = 0; j < basisDimension; ++j)
      {
        for (UnsignedInteger i = 0; i < sampleSize; ++i)
        {
          basisMatrix[flatIndex] *= weightSqrt[i];
          ++flatIndex;
        }
        for (UnsignedInteger i = 0; i <= j; ++i)
          basisMatrix(sampleSize + i, j) = penalizationCholesky(j, i) * rho;
        flatIndex += basisDimension;
      }
    } // !hasUniformWeight_
  } // penalizationFactor_ > 0.0
  // Solve the linear system (least squares solution)
  // If we can use the normal equation (fastest method, not very stable)
  Bool isSolved = false;
  // The normal matrix will be singular if not enough rows
  if (useNormal_ && (basisMatrix.getNbRows() >= basisMatrix.getNbColumns()))
  {
    try
    {
      LOGINFO("In PenalizedLeastSquaresAlgorithm::run(), use normal equation");
      CovarianceMatrix normalMatrix(basisMatrix.computeGram(true));
      setCoefficients(normalMatrix.solveLinearSystemInPlace(basisMatrix.genVectProd(rightHandSide, true)));
      isSolved = true;
    }
    catch (const NotDefinedException & ex)
    {
      LOGINFO(OSS() << ex);
      isSolved = false;
    }
  } // Use normal equation
  // Use the QR decomposition, slowest but more stable and able to deal with singular cases
  if (!(useNormal_ && isSolved))
  {
    LOGINFO("In PenalizedLeastSquaresAlgorithm::run(), use QR decomposition");
    setCoefficients(basisMatrix.solveLinearSystemRect(rightHandSide));
  }
  Scalar quadraticResidual = (basisMatrix.genVectProd(getCoefficients()) - rightHandSide).normSquare();
  // The residual is the mean L2 norm of the fitting
  setResidual(std::sqrt(quadraticResidual) / sampleSize);

  const Scalar empiricalError = quadraticResidual / sampleSize;

  // The relative error
  setRelativeError(empiricalError / y_.computeVariance()[0]);
}


/* Virtual constructor */
PenalizedLeastSquaresAlgorithm * PenalizedLeastSquaresAlgorithm::clone() const
{
  return new PenalizedLeastSquaresAlgorithm(*this);
}


/* String converter */
String PenalizedLeastSquaresAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << ApproximationAlgorithmImplementation::__repr__()
         << " penalization factor=" << penalizationFactor_
         << " penalization matrix=" << penalizationMatrix_;
}


/* Method save() stores the object through the StorageManager */
void PenalizedLeastSquaresAlgorithm::save(Advocate & adv) const
{
  ApproximationAlgorithmImplementation::save(adv);
  adv.saveAttribute( "penalizationFactor_", penalizationFactor_ );
  adv.saveAttribute( "penalizationMatrix_", penalizationMatrix_ );
}


/* Method load() reloads the object from the StorageManager */
void PenalizedLeastSquaresAlgorithm::load(Advocate & adv)
{
  ApproximationAlgorithmImplementation::load(adv);
  adv.loadAttribute( "penalizationFactor_", penalizationFactor_ );
  adv.loadAttribute( "penalizationMatrix_", penalizationMatrix_ );
}

Collection<Indices> PenalizedLeastSquaresAlgorithm::getSelectionHistory(Collection<Point> & coefficientsHistory) const
{
  coefficientsHistory = Collection<Point>(1, coefficients_);
  return Collection<Indices>(1, currentIndices_);
}

END_NAMESPACE_OPENTURNS
