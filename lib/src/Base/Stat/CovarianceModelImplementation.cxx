//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/Contour.hxx"
#include "openturns/Curve.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModelImplementation
 */

CLASSNAMEINIT(CovarianceModelImplementation)

static const Factory<CovarianceModelImplementation> Factory_CovarianceModelImplementation;

/* Dimension-based constructor */
CovarianceModelImplementation::CovarianceModelImplementation(const UnsignedInteger inputDimension)
  : PersistentObject()
  , scale_(inputDimension, 1.0)
  , inputDimension_(inputDimension)
  , amplitude_(1, 1.0)
  , outputDimension_(1)
  , outputCorrelation_(0)
  , outputCovariance_(0)
  , outputCovarianceCholeskyFactor_(0)
  , isDiagonal_(true)
  , isStationary_(false)
  , nuggetFactor_(ResourceMap::GetAsScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(inputDimension_ + outputDimension_)
{
  activeParameter_.fill();
  updateOutputCovariance();
}

/* Standard constructor with scale and amplitude scale parameter parameter */
CovarianceModelImplementation::CovarianceModelImplementation(const Point & scale,
    const Point & amplitude)
  : PersistentObject()
  , scale_(0)
  , inputDimension_(scale.getDimension())
  , amplitude_(0)
  , outputDimension_(amplitude.getDimension())
  , outputCorrelation_(0)
  , outputCovariance_(0)
  , outputCovarianceCholeskyFactor_(0)
  , isDiagonal_(true)
  , isStationary_(false)
  , nuggetFactor_(ResourceMap::GetAsScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(inputDimension_ + outputDimension_)
{
  setAmplitude(amplitude);
  setScale(scale);
  activeParameter_.fill();
}

/* Standard constructor with scale, amplitude and spatial correlation parameter parameter */
CovarianceModelImplementation::CovarianceModelImplementation(const Point & scale,
    const Point & amplitude,
    const CorrelationMatrix & spatialCorrelation)
  : PersistentObject()
  , scale_(0)
  , inputDimension_(scale.getDimension())
  , amplitude_(0)
  , outputDimension_(amplitude.getDimension())
  , outputCorrelation_(0)
  , outputCovariance_(0)
  , outputCovarianceCholeskyFactor_(0)
  , isDiagonal_(true)
  , isStationary_(false)
  , nuggetFactor_(ResourceMap::GetAsScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(inputDimension_ + outputDimension_)
{
  setAmplitude(amplitude);
  setScale(scale);
  activeParameter_.fill();
  setOutputCorrelation(spatialCorrelation);
}

/* Standard constructor with scale and spatial covariance parameter parameter */
CovarianceModelImplementation::CovarianceModelImplementation(const Point & scale,
    const CovarianceMatrix & spatialCovariance)
  : PersistentObject()
  , scale_(0)
  , inputDimension_(scale.getDimension())
  , amplitude_(spatialCovariance.getDimension())
  , outputDimension_(spatialCovariance.getDimension())
  , outputCorrelation_(0)
  , outputCovariance_(spatialCovariance)
  , outputCovarianceCholeskyFactor_(0)
  , isDiagonal_(spatialCovariance.isDiagonal())
  , isStationary_(false)
  , nuggetFactor_(ResourceMap::GetAsScalar("CovarianceModel-DefaultNuggetFactor"))
  , activeParameter_(inputDimension_ + outputDimension_)
{
  setScale(scale);
  for (UnsignedInteger i = 0; i < outputDimension_; ++i) 
  {
    const Scalar amplitudeI = sqrt(spatialCovariance(i, i));
    if (!(amplitudeI > 0.0))
      throw InvalidArgumentException(HERE) << "The " << i << "-th diagonal component of the spatial covariance is non positive";
    amplitude_[i] = amplitudeI;
  }
  // Convert the spatial covariance into a spatial correlation
  if (!spatialCovariance.isDiagonal())
  {
    outputCorrelation_ = CorrelationMatrix(outputDimension_);
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = j + 1; i < outputDimension_; ++i)
        outputCorrelation_(i, j) = spatialCovariance(i, j) / (amplitude_[i] * amplitude_[j]);
  } // !isDiagonal
  activeParameter_.fill();
}

/* Virtual constructor */
CovarianceModelImplementation * CovarianceModelImplementation::clone() const
{
  return new CovarianceModelImplementation(*this);
}

/* Dimension accessors */
UnsignedInteger CovarianceModelImplementation::getOutputDimension() const
{
  return outputDimension_;
}

UnsignedInteger CovarianceModelImplementation::getInputDimension() const
{
  return inputDimension_;
}

SquareMatrix CovarianceModelImplementation::operator() (const Scalar s,
    const Scalar t) const
{
  return operator() (Point(1, s), Point(1, t));
}

SquareMatrix CovarianceModelImplementation::operator() (const Point & s,
    const Point & t) const
{
  if (outputDimension_ == 1)
  {
    SquareMatrix result(1);
    result(0, 0) = computeAsScalar(s, t);
    return result;
  }
  if (isStationary())
    return operator()(s - t);
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::operator()(const Point & s, const Point & t) const";
}

Scalar CovarianceModelImplementation::computeAsScalar (const Point & s,
    const Point & t) const
{
  if (outputDimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << outputDimension_ << ", expected dimension=1.";
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  // Return the scalar value
  // Even if model is stationary we do not create a new Point tau
  // We prefer relying on the iterator method
  return computeAsScalar(s.begin(), t.begin());
}

Scalar CovarianceModelImplementation::computeAsScalar(const Collection<Scalar>::const_iterator & ,
    const Collection<Scalar>::const_iterator & ) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin, const Collection<Scalar>::const_iterator & t_begin) const";
}

Scalar CovarianceModelImplementation::computeAsScalar(const Point &) const
{
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << outputDimension_ << ", expected dimension=1.";
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar (const Point & tau) const";
}

Scalar CovarianceModelImplementation::computeAsScalar(const Scalar s,
                                                      const Scalar t) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  if (isStationary())
    return computeAsScalar(s - t);
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const Scalar s, const Scalar t) const";
}

Scalar CovarianceModelImplementation::computeAsScalar(const Scalar) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const Scalar tau) const";
}

/* Computation of the covariance function */
SquareMatrix CovarianceModelImplementation::operator() (const Scalar tau) const
{
  return operator() (Point(1, tau));
}

SquareMatrix CovarianceModelImplementation::operator() (const Point & tau) const
{
  if (isStationary() && (getOutputDimension()==1))
  {
     SquareMatrix result(1);
     result(0, 0) = computeAsScalar(tau);
     return result;
    }

  if (isStationary())
    throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::operator()(const Point & tau) const";
  // Case not stationary ==> operator(tau, 0)
  return operator() (Point(tau.getDimension()), tau);
}

/* Gradient */
Matrix CovarianceModelImplementation::partialGradient (const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (outputDimension_ == 1)
  {
    Matrix gradient(inputDimension_, 1);
    Scalar centralValue = computeAsScalar(s, t);
    const Scalar epsilon = std::sqrt(SpecFunc::ScalarEpsilon);
    // Loop over the shifted points
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
    {
      Point currentPoint(s);
      currentPoint[i] += epsilon;
      Scalar currentValue = computeAsScalar(currentPoint, t);
      gradient(i, 0) = (currentValue - centralValue) / epsilon;
    }
    return gradient;
  }
  Matrix gradient(inputDimension_, outputDimension_ * outputDimension_);
  SquareMatrix covarianceST(operator()(s, t));
  // Convert result into MatrixImplementation to symmetrize & get the collection
  MatrixImplementation covarianceSTImplementation(*covarianceST.getImplementation());
  covarianceSTImplementation.symmetrize();
  const Point centralValue(covarianceSTImplementation);
  const Scalar epsilon = std::sqrt(SpecFunc::ScalarEpsilon);
  // Loop over the shifted points
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    Point currentPoint(s);
    currentPoint[i] += epsilon;
    SquareMatrix localCovariance(operator()(currentPoint, t));
    MatrixImplementation localCovarianceImplementation(*localCovariance.getImplementation());
    localCovarianceImplementation.symmetrize();
    const Point currentValue(localCovarianceImplementation);
    for (UnsignedInteger j = 0; j < centralValue.getDimension(); ++j)
      gradient(i, j) = (currentValue[j] - centralValue[j]) / epsilon;
  }
  return gradient;
}

/* Gradient */
Matrix CovarianceModelImplementation::parameterGradient(const Point & s,
    const Point & t) const
{
  const Point parameter(getParameter());
  const UnsignedInteger size = parameter.getSize();
  const Scalar epsilon = std::sqrt(SpecFunc::ScalarEpsilon);
  if (outputDimension_ == 1)
  {
    Matrix gradient(size, 1);
    const Scalar covRef = computeAsScalar(s, t);
    Pointer<CovarianceModelImplementation> p_implementation(clone());
    for (UnsignedInteger k = 0; k < size; ++ k)
    {
      Point parameterP(parameter);
      parameterP[k] += epsilon;
      p_implementation->setParameter(parameterP);
      const Scalar covP = p_implementation->computeAsScalar(s, t);
      gradient(k, 0) = (covP - covRef) / epsilon;
    }
    return gradient;
  }
  // Finite difference estimate
  // Care operator() yields SquareMatrix, which are not necessarly symmetric
  // Thus we should account for all elements of operator()(s,t)
  Matrix gradient(size, outputDimension_ * outputDimension_);
  const SquareMatrix covRef(operator()(s, t));
  Pointer<CovarianceModelImplementation> p_implementation(clone());
  for (UnsignedInteger k = 0; k < size; ++ k)
  {
    Point parameterP(parameter);
    parameterP[k] += epsilon;
    p_implementation->setParameter(parameterP);
    const SquareMatrix covP (p_implementation->operator()(s, t));
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension_; ++ j)
    {
      for (UnsignedInteger i = 0; i < outputDimension_; ++ i)
      {
        gradient(k, index) = (covP(i, j) - covRef(i, j)) / epsilon;
        ++ index;
      }
    }
  }
  return gradient;
}

/* Discretize the covariance function on a given TimeGrid/Mesh */
CovarianceMatrix CovarianceModelImplementation::discretize(const RegularGrid & timeGrid) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";

  if (isStationary())
  {
    const UnsignedInteger size = timeGrid.getN();
    const Scalar timeStep = timeGrid.getStep();
    const UnsignedInteger fullSize = size * outputDimension_;
    CovarianceMatrix covarianceMatrix(fullSize);

    if (outputDimension_ == 1)
    {
      // The stationary property of this model allows to optimize the discretize operation
      // over a regular time grid: the large covariance matrix is block-diagonal
      // Fill the matrix by block-diagonal
      // The main diagonal has a specific treatment as only its lower triangular part
      // has to be copied
      for (UnsignedInteger diag = 0; diag < size; ++diag)
      {
        const Scalar covTau = computeAsScalar(diag * timeStep);
        for (UnsignedInteger i = 0; i < size - diag; ++i)
          covarianceMatrix(i, i + diag) = covTau;
      }
      return covarianceMatrix;
    }
    // General multivariate (stationary) case
    // Fill-in the matrix by blocks
    for (UnsignedInteger diagonalOffset = 0; diagonalOffset < size; ++diagonalOffset)
    {
      const SquareMatrix localCovarianceMatrix(operator()( diagonalOffset * timeStep) );
      // Only the lower part has to be filled-in
      for (UnsignedInteger rowIndex = diagonalOffset; rowIndex < size; ++rowIndex)
      {
        const UnsignedInteger rowBase = rowIndex * outputDimension_;
        const UnsignedInteger columnIndex = rowIndex - diagonalOffset;
        const UnsignedInteger columnBase = columnIndex * outputDimension_;
        // We fill the covariance matrix using the previous local one
        // The full local covariance matrix has to be copied as it is
        // not copied on a symmetric position
        for (UnsignedInteger columnIndexLocal = 0; columnIndexLocal < outputDimension_; ++columnIndexLocal)
          for (UnsignedInteger rowIndexLocal = 0; rowIndexLocal < outputDimension_; ++rowIndexLocal)
            covarianceMatrix(rowBase + rowIndexLocal, columnBase + columnIndexLocal) = localCovarianceMatrix(rowIndexLocal, columnIndexLocal) ;
      } // column index of the block
    } // row index of the block
    return covarianceMatrix;
  }
  // Non stationary case
  return discretize(timeGrid.getVertices());
}

struct CovarianceModelDiscretizePolicy
{
  const SampleImplementation & input_;
  CovarianceMatrix & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger outputDimension_;

  CovarianceModelDiscretizePolicy(const Sample & input,
                                  CovarianceMatrix & output,
                                  const CovarianceModelImplementation & model)
    : input_(*input.getImplementation())
    , output_(output)
    , model_(model)
    , outputDimension_(model.getOutputDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jLocal = static_cast< UnsignedInteger >(sqrt(2.0 * i + 0.25) - 0.5);
      const UnsignedInteger jBase = jLocal * outputDimension_;
      const UnsignedInteger iLocal = i - (jLocal * (jLocal + 1)) / 2;
      const UnsignedInteger iBase = iLocal * outputDimension_;
      const SquareMatrix localCovariance(model_(input_[iLocal], input_[jLocal]));
      for (UnsignedInteger jj = 0; jj < outputDimension_; ++jj)
        for (UnsignedInteger ii = 0; ii < outputDimension_; ++ii)
          output_(iBase + ii, jBase + jj) = localCovariance(ii, jj);
    }
  }

}; /* end struct CovarianceModelDiscretizePolicy */

struct CovarianceModelDiscretizeScalarPolicy
{
  const SampleImplementation & input_;
  // output_ is a CovarianceMatrix, but since we fill only its half part,
  // we can directly store results in the underlying MatrixImplementation.
  // This avoids comparing row and column numbers.
  MatrixImplementation & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger inputDimension_;

  CovarianceModelDiscretizeScalarPolicy(const Sample & input,
      CovarianceMatrix & output,
      const CovarianceModelImplementation & model)
    : input_(*input.getImplementation())
    , output_(*output.getImplementation())
    , model_(model)
    , inputDimension_(input_.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jLocal = static_cast< UnsignedInteger >(sqrt(2.0 * i + 0.25) - 0.5);
      const UnsignedInteger iLocal = i - (jLocal * (jLocal + 1)) / 2;
      // By construction, iLocal <= jLocal
      output_(jLocal, iLocal) = model_.computeAsScalar(input_.data_begin() + iLocal * inputDimension_, input_.data_begin() + jLocal * inputDimension_);
    }
  }
}; /* end struct CovarianceModelDiscretizeScalarPolicy */

CovarianceMatrix CovarianceModelImplementation::discretize(const Sample & vertices) const
{
  if (vertices.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << inputDimension_;
  const UnsignedInteger size = vertices.getSize();
  if (getOutputDimension() == 1)
  {
    CovarianceMatrix covarianceMatrix(size);
    const CovarianceModelDiscretizeScalarPolicy policy(vertices, covarianceMatrix, *this);
    // The loop is over the lower block-triangular part
    TBB::ParallelForIf(isParallel(), 0, size * (size + 1) / 2, policy);
    return covarianceMatrix;
  }
  else
  {
    const UnsignedInteger fullSize = size * outputDimension_;
    CovarianceMatrix covarianceMatrix(fullSize);
    const CovarianceModelDiscretizePolicy policy( vertices, covarianceMatrix, *this );
    // The loop is over the lower block-triangular part
    TBB::ParallelForIf(isParallel(), 0, size * (size + 1) / 2, policy);
    return covarianceMatrix;
  }
}

CovarianceMatrix CovarianceModelImplementation::discretize(const Mesh & mesh) const
{
  return discretize(mesh.getVertices());
}

/* Discretize and factorize the covariance function on a given TimeGrid/Mesh */
TriangularMatrix CovarianceModelImplementation::discretizeAndFactorize(const RegularGrid & timeGrid) const
{
  return discretizeAndFactorize(timeGrid.getVertices());
}

TriangularMatrix CovarianceModelImplementation::discretizeAndFactorize(const Mesh & mesh) const
{
  return discretizeAndFactorize(mesh.getVertices());
}

TriangularMatrix CovarianceModelImplementation::discretizeAndFactorize(const Sample & vertices) const
{
  // We suppose that covariance matrix is symmetric positive definite
  if (getOutputDimension() == 1)
  {
    const UnsignedInteger size = vertices.getSize();
    CovarianceMatrix covarianceMatrix(size);
    const CovarianceModelDiscretizeScalarPolicy policy( vertices, covarianceMatrix, *this );
    // The loop is over the lower block-triangular part
    TBB::ParallelForIf(isParallel(), 0, size * (size + 1) / 2, policy);
    // Compute the Cholesky
    return covarianceMatrix.computeCholesky(false);
  }
  else
  {
    CovarianceMatrix covariance = discretize(vertices);
    TriangularMatrix choleskyFactor = covariance.computeCholesky();
    return choleskyFactor;
  }
}


struct CovarianceModelScalarDiscretizeRowPolicy
{
  const Sample & input_;
  const Point p_;
  Sample & output_;
  const CovarianceModelImplementation & model_;

  CovarianceModelScalarDiscretizeRowPolicy(const Sample & input,
      const UnsignedInteger p,
      Sample & output,
      const CovarianceModelImplementation & model)
    : input_(input)
    , p_(input[p])
    , output_(output)
    , model_(model)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_(i, 0) = model_.computeAsScalar(p_, input_[i]);
  }

}; /* end struct CovarianceModelScalarDiscretizeRowPolicy */

struct CovarianceModelDiscretizeRowPolicy
{
  const Sample & input_;
  const Point p_;
  Sample & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger outputDimension_;

  CovarianceModelDiscretizeRowPolicy(const Sample & input,
                                     const UnsignedInteger p,
                                     Sample & output,
                                     const CovarianceModelImplementation & model)
    : input_(input)
    , p_(input[p])
    , output_(output)
    , model_(model)
    , outputDimension_(model.getOutputDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const MatrixImplementation localCovariance(*(model_(p_, input_[i]).getImplementation()));
      const UnsignedInteger base = i * outputDimension_;
      for (UnsignedInteger ii = 0; ii < outputDimension_; ++ii)
        for (UnsignedInteger jj = 0; jj < outputDimension_; ++jj)
          output_(base + ii, jj) = localCovariance(ii, jj);
    }
  }

}; /* end struct CovarianceModelDiscretizeRowPolicy */

Sample CovarianceModelImplementation::discretizeRow(const Sample & vertices,
    const UnsignedInteger p) const
{
  if (vertices.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << inputDimension_;
  const UnsignedInteger size = vertices.getSize();
  Sample result(size * outputDimension_, outputDimension_);
  if (outputDimension_ == 1)
  {
    const CovarianceModelScalarDiscretizeRowPolicy policy( vertices, p, result, *this );
    TBB::ParallelForIf(isParallel(), 0, size, policy);
  }
  else
  {
    const CovarianceModelDiscretizeRowPolicy policy( vertices, p, result, *this );
    TBB::ParallelForIf(isParallel(), 0, size, policy);
  }
  return result;
}

/* Discretize the covariance function on a given TimeGrid/Mesh using HMatrix */
HMatrix CovarianceModelImplementation::discretizeHMatrix(const RegularGrid & timeGrid,
    const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(timeGrid.getVertices(), parameters);
}

HMatrix CovarianceModelImplementation::discretizeHMatrix(const Mesh & mesh,
    const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(mesh.getVertices(), parameters);
}

HMatrix CovarianceModelImplementation::discretizeHMatrix(const Sample & vertices,
    const HMatrixParameters & parameters) const
{
#ifdef OPENTURNS_HAVE_HMAT
  HMatrixFactory hmatrixFactory;
  HMatrix covarianceHMatrix = hmatrixFactory.build(vertices, outputDimension_, true, parameters);
  if (outputDimension_ == 1)
  {
    CovarianceAssemblyFunction simple(*this, vertices);
    covarianceHMatrix.assemble(simple, 'L');
  }
  else
  {
    CovarianceBlockAssemblyFunction block(*this, vertices);
    covarianceHMatrix.assemble(block, 'L');
  }
  return covarianceHMatrix;
#else
  (void) vertices;
  (void) parameters;
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::discretizeHMatrix, OpenTURNS had been compiled without HMat support";
#endif
}

/* Discretize and factorize the covariance function on a given TimeGrid/Mesh using HMatrix */
HMatrix CovarianceModelImplementation::discretizeAndFactorizeHMatrix(const RegularGrid & timeGrid,
    const HMatrixParameters & parameters) const
{
  return discretizeAndFactorizeHMatrix(timeGrid.getVertices(), parameters);
}

HMatrix CovarianceModelImplementation::discretizeAndFactorizeHMatrix(const Mesh & mesh,
    const HMatrixParameters & parameters) const
{
  return discretizeAndFactorizeHMatrix(mesh.getVertices(), parameters);
}

HMatrix CovarianceModelImplementation::discretizeAndFactorizeHMatrix(const Sample & vertices,
    const HMatrixParameters & parameters) const
{
  // We suppose that covariance matrix is symmetric positive definite
  // We do not catch InternalException
  // Incremeant nugget factor to make matrix positive definite
  // Maybe parameters need to be adapted.
  HMatrix covarianceFactor = discretizeHMatrix(vertices, parameters);
  covarianceFactor.factorize("LLt");
  return covarianceFactor;
}

/* Amplitude accessor */
Point CovarianceModelImplementation::getAmplitude() const
{
  return amplitude_;
}

void CovarianceModelImplementation::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != outputDimension_) throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setAmplitude: the given amplitude has a dimension=" << amplitude.getDimension() << " different from the dimension=" << outputDimension_;
  for (UnsignedInteger index = 0; index < outputDimension_; ++index)
    if (!(amplitude[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setAmplitude, the component " << index << " of amplitude is non positive" ;
  amplitude_ = amplitude;
  updateOutputCovariance();
}

/* Scale accessor */
Point CovarianceModelImplementation::getScale() const
{
  return scale_;
}

void CovarianceModelImplementation::setScale(const Point & scale)
{
  if (scale.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setScale: the given scale has a dimension=" << scale.getDimension() << " different from the input dimension=" << inputDimension_;
  for (UnsignedInteger index = 0; index < inputDimension_; ++index)
    if (!(scale[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setScale: the component " << index << " of scale is non positive" ;
  scale_ = scale;
}

/* Output correlation accessor */
CorrelationMatrix CovarianceModelImplementation::getOutputCorrelation() const
{
  if (!isDiagonal_) return outputCorrelation_;
  return CorrelationMatrix(outputDimension_);
}

void CovarianceModelImplementation::setOutputCorrelation(const CorrelationMatrix & outputCorrelation)
{
  if (outputCorrelation.getDimension() != outputDimension_)
    throw InvalidArgumentException(HERE) << "The given spatial correlation has a dimension different from the amplitudes.";
  isDiagonal_ = outputCorrelation.isDiagonal();
  outputCorrelation_ = isDiagonal_ ? CorrelationMatrix() : outputCorrelation;
  updateOutputCovariance();
}

void CovarianceModelImplementation::updateOutputCovariance()
{
  outputCovariance_ = CovarianceMatrix(outputDimension_);
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
  {
    outputCovariance_(j, j) = amplitude_[j] * amplitude_[j];
    if (!isDiagonal_)
    {
      for (UnsignedInteger i = j + 1; i < outputDimension_; ++i)
        outputCovariance_(i, j) = outputCorrelation_(i, j) * amplitude_[i] * amplitude_[j];
    }
  }
  outputCovarianceCholeskyFactor_.getImplementation().reset(new MatrixImplementation);
}

/* Nugget factor accessor */
Scalar CovarianceModelImplementation::getNuggetFactor() const
{
  return nuggetFactor_;
}

void CovarianceModelImplementation::setNuggetFactor(const Scalar nuggetFactor)
{
  if (!(nuggetFactor >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the nugget factor=" << nuggetFactor << " is negative";
  nuggetFactor_ = nuggetFactor;
}

void CovarianceModelImplementation::setFullParameter(const Point & parameter)
{
  // Here we manage only the generic parameters
  // First the scale parameter
  UnsignedInteger index = 0;
  // Check the size
  const UnsignedInteger totalSize = inputDimension_ + outputDimension_ * (outputDimension_ + 1) / 2;
  if (parameter.getSize() < totalSize)
    throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setFullParameter, points have incompatible size. Point size = " << parameter.getSize()
                                         << " whereas expected size = " << totalSize ;

  for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
  {
    if (!(parameter[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setFullParameter, the component " << index << " of scale is non positive";
    scale_[i] = parameter[index];
    ++ index;
  }
  // Second the amplitude parameter
  for (UnsignedInteger i = 0; i < outputDimension_; ++ i)
  {
    if (!(parameter[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In CovarianceModelImplementation::setFullParameter, the component " << index << " of amplitude is non positive";
    amplitude_[i] = parameter[index];
    ++ index;
  }
  CorrelationMatrix outputCorrelation(outputDimension_);
  // Third the output correlation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < outputDimension_; ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      outputCorrelation(i, j) = parameter[index];
      ++ index;
    }
  setOutputCorrelation(outputCorrelation);
}


Point CovarianceModelImplementation::getFullParameter() const
{
  // Here we manage only the generic parameters
  // First the scale parameter
  Point parameter(getScale());
  // Second the amplitude parameter
  parameter.add(getAmplitude());
  // Third the spatial correation parameter, only the lower triangle
  if (isDiagonal_)
  {
    if (outputDimension_ > 1)
      parameter.add(Point((outputDimension_ * (outputDimension_ - 1)) / 2));
  }
  else
    for (UnsignedInteger i = 0; i < outputDimension_; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        parameter.add(outputCorrelation_(i, j));
  return parameter;
}

Description CovarianceModelImplementation::getFullParameterDescription() const
{
  // Here we manage only the generic parameters
  // First the scale parameter
  Description description(0);
  // First the scale parameter
  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
    description.add(OSS() << "scale_" << j);
  // Second the amplitude parameter
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    description.add(OSS() << "amplitude_" << j);
  // Third the spatial correlation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      description.add(OSS() << "R_" << i << "_" << j);

  return description;
}

/* Indices of the active parameters */
void CovarianceModelImplementation::setActiveParameter(const Indices & active)
{
  if (!active.isIncreasing()) throw InvalidArgumentException(HERE) << "Error: the active parameter indices must be given in increasing order, here active=" << active;
  activeParameter_ = active;
}

Indices CovarianceModelImplementation::getActiveParameter() const
{
  return activeParameter_;
}


void CovarianceModelImplementation::setParameter(const Point & parameter)
{
  const UnsignedInteger activeSize = activeParameter_.getSize();
  if (activeSize == 0) return;
  // Brute-force approach: everything is updated
  Point fullParameter(getFullParameter());
  for (UnsignedInteger i = 0; i < activeSize; ++i)
    fullParameter[activeParameter_[i]] = parameter[i];
  setFullParameter(fullParameter);
}


Point CovarianceModelImplementation::getParameter() const
{
  // For now we use a brute-force approach: build all the parameters then extract the relevant ones
  Point result(0);
  const UnsignedInteger activeSize = activeParameter_.getSize();
  // Quick return if no parameter
  if (activeSize == 0) return result;
  const Point parameter(getFullParameter());
  if (activeSize == parameter.getSize()) return parameter;
  // Then filter only the active parameters
  for (UnsignedInteger i = 0; i < activeSize; ++ i)
    result.add(parameter[activeParameter_[i]]);
  return result;
}

Description CovarianceModelImplementation::getParameterDescription() const
{
  // For now we use a brute-force approach: build all the parameters then extract the relevant ones
  Description result(0);
  const UnsignedInteger activeSize = activeParameter_.getSize();
  // Quick return if no parameter
  if (activeSize == 0) return result;
  const Description description(getFullParameterDescription());
  if (activeSize == description.getSize()) return description;
  // Then filter only the active parameters
  for (UnsignedInteger i = 0; i < activeSize; ++i)
    result.add(description[activeParameter_[i]]);
  return result;
}


/* Is it a stationary model ? */
Bool CovarianceModelImplementation::isStationary() const
{
  return isStationary_;
}

/* Is it a diagonal model ? */
Bool CovarianceModelImplementation::isDiagonal() const
{
  return isDiagonal_;
}

/* Is it safe to compute discretize etc in parallel? */
Bool CovarianceModelImplementation::isParallel() const
{
  return true;
}

/* Marginal accessor */
CovarianceModel CovarianceModelImplementation::getMarginal(const UnsignedInteger index) const
{
  return getMarginal(Indices(1, index));
}

/* Marginal accessor */
CovarianceModel CovarianceModelImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(outputDimension_)) throw InvalidArgumentException(HERE) << "Error: index=" << indices << " must be less than output dimension=" << outputDimension_;
  CovarianceModel result(clone());
  if (outputDimension_ > 1)
  {
    result.getImplementation()->outputDimension_ = indices.getSize();
    Point newAmplitude;
    for (UnsignedInteger i = 0; i < indices.getSize(); ++ i)
    {
      newAmplitude.add(amplitude_[indices[i]]);
    }
    result.getImplementation()->setAmplitude(newAmplitude);
    if (!isDiagonal())
    {
      CorrelationMatrix newCorrelationMatrix(indices.getSize());
      for (UnsignedInteger i = 0; i < indices.getSize(); ++ i)
      {
        for (UnsignedInteger j = 0; j < i; ++ j)
        {
          newCorrelationMatrix(i, j) = result.getImplementation()->outputCorrelation_(indices[i], indices[j]);
        }
      }
      result.getImplementation()->setOutputCorrelation(newCorrelationMatrix);
    }

    // reset activeParameter_
    result.getImplementation()->activeParameter_ = Indices(inputDimension_ + indices.getSize());
    result.getImplementation()->activeParameter_.fill();
  }
  return result;
}

/* Drawing method */
Graph CovarianceModelImplementation::draw(const UnsignedInteger rowIndex,
    const UnsignedInteger columnIndex,
    const Scalar tMin,
    const Scalar tMax,
    const UnsignedInteger pointNumber,
    const Bool asStationary,
    const Bool correlationFlag) const
{
  if (inputDimension_ != 1) throw NotDefinedException(HERE) << "Error: can draw covariance models only if input dimension=1, here input dimension=" << inputDimension_;
  if (rowIndex >= outputDimension_) throw InvalidArgumentException(HERE) << "Error: the given row index must be less than " << outputDimension_ << ", here rowIndex=" << rowIndex;
  if (columnIndex >= outputDimension_) throw InvalidArgumentException(HERE) << "Error: the given column index must be less than " << outputDimension_ << ", here columnIndex=" << columnIndex;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw the model with pointNumber<2, here pointNumber=" << pointNumber;
  // Check if the model is stationary and if we want to draw it this way
  if (asStationary && isStationary())
  {
    // Here we compute the normalization for the correlation instead of using
    // the amplitude attribute for models for which it is not given
    Scalar ratio = 1.0;
    if (correlationFlag)
    {
      ratio = (*this)(0.0)(rowIndex, columnIndex);
      // If ratio == 0, the covariance is zero everywhere
      if (ratio == 0.0) ratio = 1.0;
    }
    Sample data(pointNumber, 2);
    for (UnsignedInteger i = 0; i < pointNumber; ++i)
    {
      const Scalar tau = (i * tMin + (pointNumber - i - 1.0) * tMax) / (pointNumber - 1.0);
      const Scalar value((*this)(tau)(rowIndex, columnIndex) / ratio);
      data(i, 0) = tau;
      data(i, 1) = value;
    }
    Graph graph(getName(), "tau", (correlationFlag ? "correlation" : "covariance"), true, "topright");
    Curve curve(data);
    curve.setLineWidth(2);
    curve.setColor("red");
    graph.add(curve);
    return graph;
  }
  // Here we draw a non-stationary model
  const Sample gridT = RegularGrid(tMin, (tMax - tMin) / (pointNumber - 1.0), pointNumber).getVertices();
  CovarianceMatrix matrix(discretize(gridT));
  const UnsignedInteger dimension = matrix.getDimension();
  // Normalize the data if needed
  if (correlationFlag)
  {
    Point sigma(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      sigma[i] = std::sqrt(matrix(i, i));
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      for (UnsignedInteger i = 0; i < j; ++i)
      {
        const Scalar scaling = sigma[i] * sigma[j];
        if (scaling == 0.0) matrix(i, j) = 0.0;
        else matrix(i, j) /= scaling;
      } // i
      matrix(j, j) = 1.0;
    } // j
  } // correlationFlag
  matrix.checkSymmetry();
  Sample data(pointNumber * pointNumber, 1);
  data.getImplementation()->setData(*matrix.getImplementation());
  Graph graph(getName() + (correlationFlag ? String(" correlation") : String (" covariance")), "s", "t", true, "bottomright");
  graph.setGrid(true);
  Contour contour(pointNumber, pointNumber, data);
  Contour isoValues(Contour(gridT, gridT, data, Point(0), Description(0), true, ""));
  isoValues.buildDefaultLevels();
  isoValues.buildDefaultLabels();
  const Point levels(isoValues.getLevels());
  const Description labels(isoValues.getLabels());
  for (UnsignedInteger i = 0; i < levels.getDimension(); ++i)
  {
    Contour current(isoValues);
    current.setLevels(Point(1, levels[i]));
    current.setLabels(Description(1, labels[i]));
    current.setDrawLabels(false);
    current.setLegend(labels[i]);
    current.setColor(Contour::ConvertFromHSV((360.0 * i / levels.getDimension()), 1.0, 1.0));
    graph.add(current);
  }
  return graph;
}

/* String converter */
String CovarianceModelImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << CovarianceModelImplementation::GetClassName();
  return oss;
}

/* String converter */
String CovarianceModelImplementation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class= " << CovarianceModelImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CovarianceModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("scale_", scale_);
  adv.saveAttribute("inputDimension_", inputDimension_);
  adv.saveAttribute("amplitude_", amplitude_);
  adv.saveAttribute("outputDimension_", outputDimension_);
  adv.saveAttribute("outputCorrelation_", outputCorrelation_);
  adv.saveAttribute("isDiagonal_", isDiagonal_);
  adv.saveAttribute("isStationary_", isStationary_);
  adv.saveAttribute("nuggetFactor_", nuggetFactor_);
  adv.saveAttribute("activeParameter_", activeParameter_);
}

/* Method load() reloads the object from the StorageManager */
void CovarianceModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("scale_", scale_);
  adv.loadAttribute("inputDimension_", inputDimension_);
  adv.loadAttribute("amplitude_", amplitude_);
  adv.loadAttribute("outputDimension_", outputDimension_);
  adv.loadAttribute("outputCorrelation_", outputCorrelation_);
  adv.loadAttribute("isDiagonal_", isDiagonal_);
  adv.loadAttribute("isStationary_", isStationary_);
  adv.loadAttribute("nuggetFactor_", nuggetFactor_);
  adv.loadAttribute("activeParameter_", activeParameter_);
  updateOutputCovariance();
}

END_NAMESPACE_OPENTURNS
