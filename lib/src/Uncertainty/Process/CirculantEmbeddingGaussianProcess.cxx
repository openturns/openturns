//                                               -*- C++ -*-
/**
 *  @brief A class which implements the CirculantEmbeddingGaussianProcess process
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/CirculantEmbeddingGaussianProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/IntervalMesher.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/TriangularComplexMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CirculantEmbeddingGaussianProcess)

static const Factory<CirculantEmbeddingGaussianProcess> Factory_CirculantEmbeddingGaussianProcess;

CirculantEmbeddingGaussianProcess::CirculantEmbeddingGaussianProcess()
  : ProcessImplementation()
{
  setOutputDimension(covarianceModel_.getOutputDimension());
  setDescription(Description::BuildDefault(getOutputDimension(), "y"));
}

CirculantEmbeddingGaussianProcess::CirculantEmbeddingGaussianProcess(const CovarianceModel & covarianceModel,
    const Interval & interval,
    const Indices & discretization)
  : ProcessImplementation()
  , covarianceModel_(covarianceModel)
  , interval_(interval)
  , discretization_(discretization)
{
  dimension_ = interval.getDimension();
  if (dimension_ != covarianceModel.getInputDimension())
    throw InvalidArgumentException(HERE) << "Error: the interval dimension (" << dimension_
                                         << ") must match the covariance model input dimension ("
                                         << covarianceModel.getInputDimension() << ").";
  if (dimension_ < 1)
    throw InvalidArgumentException(HERE) << "Error: the spatial dimension must be at least 1 (got " << dimension_ << ").";
  if (discretization.getSize() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the discretization size (" << discretization.getSize()
                                         << ") must match the dimension (" << dimension_ << ").";
  for (UnsignedInteger d = 0; d < dimension_; ++d)
  {
    if (discretization[d] < 1)
      throw InvalidArgumentException(HERE) << "Error: each discretization component must be at least 1.";
  }

  IntervalMesher mesher(discretization);
  Mesh mesh(mesher.build(interval));
  setMesh(mesh);
  setOutputDimension(covarianceModel.getOutputDimension());
  setDescription(Description::BuildDefault(getOutputDimension(), "y"));
}

CirculantEmbeddingGaussianProcess * CirculantEmbeddingGaussianProcess::clone() const
{
  return new CirculantEmbeddingGaussianProcess(*this);
}

void CirculantEmbeddingGaussianProcess::initialize() const
{
  initializeND();
  isInitialized_ = true;
}

void CirculantEmbeddingGaussianProcess::initializeND() const
{
  const UnsignedInteger d = dimension_;
  const UnsignedInteger outputDim = getOutputDimension();

  // Minimum size per dimension
  Indices Nmin(d);
  for (UnsignedInteger dim = 0; dim < d; ++dim)
  {
    const UnsignedInteger n = discretization_[dim] + 1;
    if (n < 2)
      throw InvalidArgumentException(HERE) << "Error: each discretization must be at least 1.";
    Nmin[dim] = 2 * (n - 1);
  }

  Indices N(d);
  for (UnsignedInteger dim = 0; dim < d; ++dim)
    N[dim] = SpecFunc::NextPowerOfTwo(Nmin[dim]);

  const Bool userOverride = (circularSize_.getSize() == d);
  if (userOverride)
  {
    for (UnsignedInteger dim = 0; dim < d; ++dim)
    {
      if (circularSize_[dim] > 0)
      {
        if ((circularSize_[dim] & (circularSize_[dim] - 1)) != 0)
          throw InvalidArgumentException(HERE) << "Error: the circular size must be a power of two.";
        if (circularSize_[dim] < Nmin[dim])
          throw InvalidArgumentException(HERE) << "Error: the circular size is too small.";
        N[dim] = circularSize_[dim];
      }
    }
  }

  Point step(d);
  for (UnsignedInteger dim = 0; dim < d; ++dim)
    step[dim] = (interval_.getUpperBound()[dim] - interval_.getLowerBound()[dim]) / discretization_[dim];

  UnsignedInteger totalSize = 1;
  static const Scalar negativeThreshold = -1.0e-13;
  static const UnsignedInteger maxIterations = 20;

  if (outputDim == 1)
  {
    // ---- Scalar output: eigenvalue approach (simpler, avoids Cholesky overhead) ----
    Bool converged = false;
    for (UnsignedInteger iter = 0; iter < maxIterations; ++iter)
    {
      totalSize = 1;
      for (UnsignedInteger dim = 0; dim < d; ++dim)
        totalSize *= N[dim];

      // Build first row of the circulant matrix
      Collection<Complex> firstRow(totalSize);
      Indices index(d, 0);
      for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
      {
        Point tau(d);
        for (UnsignedInteger dim = 0; dim < d; ++dim)
        {
          const UnsignedInteger lag = std::min(index[dim], N[dim] - index[dim]);
          tau[dim] = lag * step[dim];
        }
        firstRow[flat] = Complex(covarianceModel_(tau)(0, 0), 0.0);

        // Advance multi-index (dim 0 fastest)
        UnsignedInteger dim = 0;
        while (dim < d && ++index[dim] == N[dim])
        {
          index[dim] = 0;
          ++dim;
        }
      }

      const Collection<Complex> eigenvaluesFFT(fftAlgorithm_.transform(firstRow, N));
      eigenvalues_ = Point(totalSize);
      Bool hasNegativeEigenvalues = false;
      Scalar minEigenvalue = SpecFunc::MaxScalar;
      for (UnsignedInteger k = 0; k < totalSize; ++k)
      {
        const Scalar lambda = std::real(eigenvaluesFFT[k]);
        eigenvalues_[k] = lambda;
        minEigenvalue = std::min(minEigenvalue, lambda);
        if (lambda < 0.0) hasNegativeEigenvalues = true;
        if (std::abs(std::imag(eigenvaluesFFT[k])) > 1e-12)
        {
          LOGWARN(OSS() << "Non-zero imaginary part in eigenvalue " << k << ": " << std::imag(eigenvaluesFFT[k]));
        }
      }

      if (!hasNegativeEigenvalues)
      {
        converged = true;
        break;
      }

      if (userOverride)
      {
        for (UnsignedInteger k = 0; k < totalSize; ++k)
          if (eigenvalues_[k] < 0.0) eigenvalues_[k] = 0.0;
        LOGWARN(OSS() << "The user-defined circular size leads to negative eigenvalues (min=" << minEigenvalue << "). Some eigenvalues have been truncated to zero.");
        converged = true;
        break;
      }

      if (minEigenvalue >= negativeThreshold)
      {
        for (UnsignedInteger k = 0; k < totalSize; ++k)
          if (eigenvalues_[k] < 0.0) eigenvalues_[k] = 0.0;
        LOGWARN(OSS() << "The smallest eigenvalue (" << minEigenvalue << ") is only slightly negative, likely due to floating-point noise. Clamping to zero.");
        converged = true;
        break;
      }

      // Double the circulant size per dimension
      for (UnsignedInteger dim = 0; dim < d; ++dim)
        N[dim] *= 2;
      LOGINFO(OSS() << "Negative eigenvalues detected (min=" << minEigenvalue << "). Increasing circular size.");
    }

    if (!converged)
      throw InternalException(HERE) << "CirculantEmbeddingGaussianProcess: maximum iterations reached without convergence. Try increasing the circular size.";
  }
  else
  {
    // ---- Vector output: Cholesky approach ----
    Bool converged = false;
    for (UnsignedInteger iter = 0; iter < maxIterations; ++iter)
    {
      totalSize = 1;
      for (UnsignedInteger dim = 0; dim < d; ++dim)
        totalSize *= N[dim];

      // Build first row of the block circulant matrix
      // Storage: [spatial_flat][p][q]
      Collection<Complex> firstRow(totalSize * outputDim * outputDim);
      Indices index(d, 0);
      for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
      {
        Point tau(d);
        for (UnsignedInteger dim = 0; dim < d; ++dim)
        {
          const UnsignedInteger lag = std::min(index[dim], N[dim] - index[dim]);
          tau[dim] = lag * step[dim];
        }

        const SquareMatrix covMatrix(covarianceModel_(tau));
        for (UnsignedInteger p = 0; p < outputDim; ++p)
        {
          for (UnsignedInteger q = 0; q < outputDim; ++q)
          {
            firstRow[flat * outputDim * outputDim + p * outputDim + q] = Complex(covMatrix(p, q), 0.0);
          }
        }

        // Advance multi-index (dim 0 fastest)
        UnsignedInteger dim = 0;
        while (dim < d && ++index[dim] == N[dim])
        {
          index[dim] = 0;
          ++dim;
        }
      }

      // FFT each (p,q) channel
      Collection<Complex> fftResult(totalSize * outputDim * outputDim);
      Collection<Complex> channel(totalSize);
      for (UnsignedInteger p = 0; p < outputDim; ++p)
      {
        for (UnsignedInteger q = 0; q < outputDim; ++q)
        {
          for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
            channel[flat] = firstRow[flat * outputDim * outputDim + p * outputDim + q];

          const Collection<Complex> fftChannel(fftAlgorithm_.transform(channel, N));

          for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
            fftResult[flat * outputDim * outputDim + p * outputDim + q] = fftChannel[flat];
        }
      }

      // Compute Cholesky factor of S(k) at each frequency
      cholFactors_.resize(totalSize);
      Bool hasNegativeEigenvalues = false;
      Scalar minEigenvalue = SpecFunc::MaxScalar;
      for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
      {
        SquareComplexMatrix spectralDensity(outputDim);
        for (UnsignedInteger p = 0; p < outputDim; ++p)
          for (UnsignedInteger q = 0; q < outputDim; ++q)
            spectralDensity(p, q) = fftResult[flat * outputDim * outputDim + p * outputDim + q];

        HermitianMatrix H(outputDim);
        for (UnsignedInteger i = 0; i < outputDim; ++i)
        {
          H(i, i) = std::real(spectralDensity(i, i));
          for (UnsignedInteger j = 0; j < i; ++j)
            H(i, j) = spectralDensity(i, j);
        }

        TriangularComplexMatrix L;
        try
        {
          L = H.computeCholesky();
        }
        catch (...)
        {
          hasNegativeEigenvalues = true;
          for (UnsignedInteger p = 0; p < outputDim; ++p)
            if (std::real(spectralDensity(p, p)) < minEigenvalue)
              minEigenvalue = std::real(spectralDensity(p, p));
        }

        if (hasNegativeEigenvalues) break;

        cholFactors_[flat] = SquareComplexMatrix(L);
      }

      if (!hasNegativeEigenvalues)
      {
        converged = true;
        break;
      }

      if (userOverride)
      {
        LOGWARN(OSS() << "The user-defined circular size leads to negative eigenvalues (min=" << minEigenvalue << "). Some eigenvalues have been truncated to zero.");
        for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
        {
          SquareComplexMatrix spectralDensity(outputDim);
          for (UnsignedInteger p = 0; p < outputDim; ++p)
            for (UnsignedInteger q = 0; q < outputDim; ++q)
              spectralDensity(p, q) = fftResult[flat * outputDim * outputDim + p * outputDim + q];

          for (UnsignedInteger i = 0; i < outputDim; ++i)
          {
            Scalar diagVal = std::real(spectralDensity(i, i));
            if (diagVal <= 0.0)
            {
              diagVal = std::abs(minEigenvalue) + 1.0e-10;
              spectralDensity(i, i) = Complex(diagVal, 0.0);
            }
          }

          HermitianMatrix H(outputDim);
          for (UnsignedInteger i = 0; i < outputDim; ++i)
          {
            H(i, i) = std::real(spectralDensity(i, i));
            for (UnsignedInteger j = 0; j < i; ++j)
              H(i, j) = spectralDensity(i, j);
          }
          TriangularComplexMatrix L = H.computeCholesky();
          cholFactors_[flat] = SquareComplexMatrix(L);
        }
        converged = true;
        break;
      }

      if (minEigenvalue >= negativeThreshold)
      {
        LOGWARN(OSS() << "The smallest eigenvalue (" << minEigenvalue << ") is only slightly negative, likely due to floating-point noise. Clamping to zero.");
        for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
        {
          SquareComplexMatrix spectralDensity(outputDim);
          for (UnsignedInteger p = 0; p < outputDim; ++p)
            for (UnsignedInteger q = 0; q < outputDim; ++q)
              spectralDensity(p, q) = fftResult[flat * outputDim * outputDim + p * outputDim + q];

          for (UnsignedInteger i = 0; i < outputDim; ++i)
          {
            Scalar diagVal = std::real(spectralDensity(i, i));
            if (diagVal < 0.0)
              spectralDensity(i, i) = Complex(1.0e-10, 0.0);
          }

          HermitianMatrix H(outputDim);
          for (UnsignedInteger i = 0; i < outputDim; ++i)
          {
            H(i, i) = std::real(spectralDensity(i, i));
            for (UnsignedInteger j = 0; j < i; ++j)
              H(i, j) = spectralDensity(i, j);
          }
          TriangularComplexMatrix L = H.computeCholesky();
          cholFactors_[flat] = SquareComplexMatrix(L);
        }
        converged = true;
        break;
      }

      // Double the circulant size per dimension
      for (UnsignedInteger dim = 0; dim < d; ++dim)
        N[dim] *= 2;
      LOGINFO(OSS() << "Negative eigenvalues detected (min=" << minEigenvalue << "). Increasing circular size.");
    }

    if (!converged)
      throw InternalException(HERE) << "CirculantEmbeddingGaussianProcess: maximum iterations reached without convergence. Try increasing the circular size.";
  }

  circularSize_ = N;
}

/* String converter */
String CirculantEmbeddingGaussianProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CirculantEmbeddingGaussianProcess::GetClassName()
      << " interval=" << interval_
      << " discretization=" << discretization_
      << " covarianceModel=" << covarianceModel_
      << " circularSize=" << circularSize_
      << " dimension=" << dimension_
      << " isInitialized=" << isInitialized_;
  return oss;
}

String CirculantEmbeddingGaussianProcess::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "CirculantEmbeddingGaussianProcess("
      << "covariance=" << covarianceModel_.__str__(offset)
      << ")";
  return oss;
}

/* Mesh accessor */
void CirculantEmbeddingGaussianProcess::setMesh(const Mesh & mesh)
{
  const UnsignedInteger n = mesh.getVerticesNumber();
  dimension_ = mesh.getDimension();
  const Sample vertices(mesh.getVertices());

  // Deduce nVerts per dimension from lexicographic vertex ordering (dim 0 fastest)
  Indices nVerts(dimension_);

  // dim 0: count consecutive vertices with same coordinates in dims 1..d-1
  {
    const Point & v0 = vertices[0];
    UnsignedInteger count = 1;
    for (UnsignedInteger i = 1; i < n; ++i)
    {
      Bool same = true;
      for (UnsignedInteger dim = 1; dim < dimension_; ++dim)
      {
        if (std::abs(vertices[i][dim] - v0[dim]) >= 1e-14)
        {
          same = false;
          break;
        }
      }
      if (!same) break;
      ++count;
    }
    nVerts[0] = count;
  }

  // dims 1..d-2
  {
    UnsignedInteger stride = nVerts[0];
    for (UnsignedInteger d = 1; d < dimension_ - 1; ++d)
    {
      const Point & v0 = vertices[0];
      UnsignedInteger count = 1;
      for (UnsignedInteger i = stride; i < n; i += stride)
      {
        Bool same = true;
        for (UnsignedInteger dim = d + 1; dim < dimension_; ++dim)
        {
          if (std::abs(vertices[i][dim] - v0[dim]) >= 1e-14)
          {
            same = false;
            break;
          }
        }
        if (!same) break;
        ++count;
      }
      nVerts[d] = count;
      stride *= count;
    }
  }

  // last dim: remaining
  {
    UnsignedInteger product = 1;
    for (UnsignedInteger dim = 0; dim < dimension_ - 1; ++dim)
      product *= nVerts[dim];
    nVerts[dimension_ - 1] = n / product;
  }

  // Validate
  UnsignedInteger check = 1;
  for (UnsignedInteger dim = 0; dim < dimension_; ++dim)
    check *= nVerts[dim];
  if (check != n)
    throw InvalidArgumentException(HERE) << "Error: cannot deduce regular mesh discretization from input mesh with " << n << " vertices.";

  discretization_.resize(dimension_);
  for (UnsignedInteger dim = 0; dim < dimension_; ++dim)
    discretization_[dim] = nVerts[dim] - 1;

  // Infer interval from first and last vertex
  const Point lowerBound(vertices[0]);
  const Point upperBound(vertices[n - 1]);
  interval_ = Interval(lowerBound, upperBound);

  ProcessImplementation::setMesh(mesh);
  isInitialized_ = false;
}

/* TimeGrid accessor */
void CirculantEmbeddingGaussianProcess::setTimeGrid(const RegularGrid & timeGrid)
{
  if (dimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: setTimeGrid is only supported for 1D processes.";
  setMesh(timeGrid);
}

/* Circular size accessors */
void CirculantEmbeddingGaussianProcess::setCircularSize(const Indices & circularSize)
{
  circularSize_ = circularSize;
  isInitialized_ = false;
}

Indices CirculantEmbeddingGaussianProcess::getCircularSize() const
{
  return circularSize_;
}

/* Realization generator */
Field CirculantEmbeddingGaussianProcess::getRealization() const
{
  if (!isInitialized_) initialize();
  Field field;
  getRealizationND(field);
  return field;
}

void CirculantEmbeddingGaussianProcess::getRealizationND(Field & field) const
{
  const UnsignedInteger d = dimension_;
  const UnsignedInteger outputDim = getOutputDimension();
  const Indices & N = circularSize_;

  UnsignedInteger totalSize = 1;
  for (UnsignedInteger dim = 0; dim < d; ++dim)
    totalSize *= N[dim];

  const Indices strides(computeStrides(N));
  const Scalar normFactor = 1.0 / std::sqrt(totalSize);

    if (outputDim == 1)
    {
      // ---- Scalar output: eigenvalue approach ----
      Collection<Complex> W(totalSize);
      Indices index(d, 0);
      for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
      {
        // Compute conjugate multi-index
        Indices conjIndex(d);
        for (UnsignedInteger dim = 0; dim < d; ++dim)
          conjIndex[dim] = (N[dim] - index[dim]) % N[dim];

        const UnsignedInteger conjFlat = flatIndex(conjIndex, strides);

        if (flat < conjFlat)
        {
          const Scalar lambda = eigenvalues_[flat];
          const Scalar factor = normFactor * std::sqrt(lambda / 2.0);
          const Scalar realPart = factor * DistFunc::rNormal();
          const Scalar imagPart = factor * DistFunc::rNormal();
          W[flat] = Complex(realPart, imagPart);
          W[conjFlat] = Complex(realPart, -imagPart);
        }
        else if (flat == conjFlat)
        {
          const Scalar lambda = eigenvalues_[flat];
          W[flat] = Complex(normFactor * std::sqrt(lambda) * DistFunc::rNormal(), 0.0);
        }
        // else: flat > conjFlat, already set as conjugate

        // Advance multi-index (dim 0 fastest)
        UnsignedInteger dim = 0;
        while (dim < d && ++index[dim] == N[dim])
        {
          index[dim] = 0;
          ++dim;
        }
      }

    // Forward FFT
    const Collection<Complex> Y(fftAlgorithm_.transform(W, N));

    // Extract on original grid
    const UnsignedInteger nVertices = mesh_.getVerticesNumber();
    Sample values(nVertices, 1);
    for (UnsignedInteger i = 0; i < nVertices; ++i)
    {
      UnsignedInteger remainder = i;
      UnsignedInteger yFlat = 0;
      for (UnsignedInteger dim = 0; dim < d; ++dim)
      {
        const UnsignedInteger coord = remainder % (discretization_[dim] + 1);
        yFlat += coord * strides[dim];
        remainder /= (discretization_[dim] + 1);
      }
      values(i, 0) = std::real(Y[yFlat]);
    }

    values.setDescription(getDescription());
    field = Field(mesh_, values);
    return;
  }

  // ---- Vector output: Cholesky approach ----
  // Generate frequency-domain vector W (outputDim channels x totalSize spatial)
  Collection<Complex> W(totalSize * outputDim);

  Indices index(d, 0);
  for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
  {
    // Compute conjugate multi-index
    Indices conjIndex(d);
    for (UnsignedInteger dim = 0; dim < d; ++dim)
      conjIndex[dim] = (N[dim] - index[dim]) % N[dim];

    const UnsignedInteger conjFlat = flatIndex(conjIndex, strides);

    if (flat < conjFlat)
    {
      // Non-self-conjugate: generate complex vector and set conjugate
      const SquareComplexMatrix & L = cholFactors_[flat];

      // Generate complex standard normal vector Z (E[Z * Z^H] = I)
      Collection<Complex> Z(outputDim);
      for (UnsignedInteger p = 0; p < outputDim; ++p)
      {
        const Scalar realPart = DistFunc::rNormal() * std::sqrt(0.5);
        const Scalar imagPart = DistFunc::rNormal() * std::sqrt(0.5);
        Z[p] = Complex(realPart, imagPart);
      }

      // W_k = cholFactors_[k] * Z_k (unscaled)
      const Collection<Complex> Wk(L * Z);

      // Apply normFactor = 1/sqrt(totalSize)
      for (UnsignedInteger p = 0; p < outputDim; ++p)
      {
        W[flat * outputDim + p] = Wk[p] * Complex(normFactor, 0.0);
        W[conjFlat * outputDim + p] = Complex(std::real(Wk[p]) * normFactor, -std::imag(Wk[p]) * normFactor);
      }
    }
    else if (flat == conjFlat)
    {
      // Self-conjugate: real-valued vector
      const SquareComplexMatrix & L = cholFactors_[flat];

      // Generate real standard normal vector Z
      Point Z(outputDim);
      for (UnsignedInteger p = 0; p < outputDim; ++p)
        Z[p] = DistFunc::rNormal();

      // W_k = cholFactors_[k] * Z_k (should be real)
      const Collection<Complex> Wk(L * Z);

      // Apply normFactor
      for (UnsignedInteger p = 0; p < outputDim; ++p)
        W[flat * outputDim + p] = Complex(std::real(Wk[p]) * normFactor, 0.0);
    }
    // else: flat > conjFlat, already set as conjugate

    // Advance multi-index (dim 0 fastest)
    UnsignedInteger dim = 0;
    while (dim < d && ++index[dim] == N[dim])
    {
      index[dim] = 0;
      ++dim;
    }
  }

  // Forward FFT each output dimension independently
  const UnsignedInteger nVertices = mesh_.getVerticesNumber();
  Sample values(nVertices, outputDim);
  Collection<Complex> channel(totalSize);
  for (UnsignedInteger p = 0; p < outputDim; ++p)
  {
    // Extract channel p
    for (UnsignedInteger flat = 0; flat < totalSize; ++flat)
      channel[flat] = W[flat * outputDim + p];

    // ND FFT
    const Collection<Complex> Y(fftAlgorithm_.transform(channel, N));

    // Gather mesh vertex values from FFT output using proper flat index mapping
    for (UnsignedInteger i = 0; i < nVertices; ++i)
    {
      UnsignedInteger remainder = i;
      UnsignedInteger yFlat = 0;
      for (UnsignedInteger dim = 0; dim < d; ++dim)
      {
        const UnsignedInteger coord = remainder % (discretization_[dim] + 1);
        yFlat += coord * strides[dim];
        remainder /= (discretization_[dim] + 1);
      }
      values(i, p) = std::real(Y[yFlat]);
    }
  }

  values.setDescription(getDescription());
  field = Field(mesh_, values);
}

/* Covariance model accessor */
CovarianceModel CirculantEmbeddingGaussianProcess::getCovarianceModel() const
{
  return covarianceModel_;
}

/* FFT algorithm accessors */
FFT CirculantEmbeddingGaussianProcess::getFFTAlgorithm() const
{
  return fftAlgorithm_;
}

void CirculantEmbeddingGaussianProcess::setFFTAlgorithm(const FFT & fft)
{
  fftAlgorithm_ = fft;
  isInitialized_ = false;
}

/* Interval accessor */
Interval CirculantEmbeddingGaussianProcess::getInterval() const
{
  return interval_;
}

/* Discretization accessor */
Indices CirculantEmbeddingGaussianProcess::getDiscretization() const
{
  return discretization_;
}

/* Check if the process is stationary */
Bool CirculantEmbeddingGaussianProcess::isStationary() const
{
  return covarianceModel_.isStationary();
}

/* Check if the process is Normal */
Bool CirculantEmbeddingGaussianProcess::isNormal() const
{
  return true;
}

Indices CirculantEmbeddingGaussianProcess::computeStrides(const Indices & dimensions) const
{
  const UnsignedInteger d = dimensions.getSize();
  Indices strides(d, 1);
  for (UnsignedInteger dim = 1; dim < d; ++dim)
    strides[dim] = strides[dim - 1] * dimensions[dim - 1];
  return strides;
}

UnsignedInteger CirculantEmbeddingGaussianProcess::flatIndex(const Indices & index,
    const Indices & strides) const
{
  UnsignedInteger f = index[0];
  for (UnsignedInteger dim = 1; dim < index.getSize(); ++dim)
    f += strides[dim] * index[dim];
  return f;
}

/* Method save() stores the object through the StorageManager */
void CirculantEmbeddingGaussianProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("cholFactors_", cholFactors_);
  adv.saveAttribute("eigenvalues_", eigenvalues_);
  adv.saveAttribute("circularSize_", circularSize_);
  adv.saveAttribute("isInitialized_", isInitialized_);
  adv.saveAttribute("fftAlgorithm_", fftAlgorithm_);
  adv.saveAttribute("interval_", interval_);
  adv.saveAttribute("discretization_", discretization_);
  adv.saveAttribute("dimension_", dimension_);
}

/* Method load() reloads the object from the StorageManager */
void CirculantEmbeddingGaussianProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("cholFactors_", cholFactors_);
  adv.loadAttribute("eigenvalues_", eigenvalues_);
  adv.loadAttribute("circularSize_", circularSize_);
  adv.loadAttribute("isInitialized_", isInitialized_);
  adv.loadAttribute("fftAlgorithm_", fftAlgorithm_);
  adv.loadAttribute("interval_", interval_);
  adv.loadAttribute("discretization_", discretization_);
  adv.loadAttribute("dimension_", dimension_);
}

END_NAMESPACE_OPENTURNS
