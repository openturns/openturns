//                                               -*- C++ -*-
/**
 *  @brief SlicedInverseRegression
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
#include <algorithm>

#include "openturns/SlicedInverseRegression.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SlicedInverseRegression);

static Factory<SlicedInverseRegression> Factory_SlicedInverseRegression;


/* Default constructor */
SlicedInverseRegression::SlicedInverseRegression()
  : PersistentObject()
{
  // Nothing to do
}

SlicedInverseRegression::SlicedInverseRegression(const Sample & inputSample,
                                                 const Sample & outputSample,
                                                 const UnsignedInteger sliceNumber)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , nbModes_(inputSample.getDimension())
{
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Input and output samples must have the same size";
  if (outputSample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Supervision variable must be of dimension 1";
  setSliceNumber(sliceNumber);
}

/* Virtual constructor method */
SlicedInverseRegression * SlicedInverseRegression::clone() const
{
  return new SlicedInverseRegression(*this);
}

void SlicedInverseRegression::run()
{
  const UnsignedInteger size = inputSample_.getSize();
  const UnsignedInteger inputDimension = inputSample_.getDimension();

  // Validate slice count
  if (sliceNumber_ == 0 || sliceNumber_ > size)
    throw InvalidArgumentException(HERE) << "Slice number must be in [1, " << size << "]";

  // Step 1 -- Partition the sorted output into H slices of equal size
  // SIR relies on inverse regression: instead of regressing Y on X, we
  // regress X on Y by slicing the Y-range. Each slice contains roughly
  // the same number of observations. The remainder is spread over the
  // first slices so no observations are dropped.
  const Indices supervisionIndices = outputSample_.argsort();
  Collection<Indices> list_chunk(sliceNumber_);
  Indices chunk_population(sliceNumber_);
  const UnsignedInteger baseSize = size / sliceNumber_;
  const UnsignedInteger remainder = size % sliceNumber_;
  UnsignedInteger offset = 0;
  for (UnsignedInteger i = 0; i < sliceNumber_; ++i)
  {
    const UnsignedInteger localSize = baseSize + (i < remainder ? 1 : 0);
    Indices chunk(localSize);
    std::copy(supervisionIndices.begin() + offset, supervisionIndices.begin() + offset + localSize, chunk.begin());
    list_chunk[i] = chunk;
    chunk_population[i] = localSize;
    offset += localSize;
  }

  // Step 2  -- Center the input data
  const Point mean(inputSample_.computeMean());
  Sample X_centered(inputSample_);
  X_centered -= mean;

  // Step 3  -- QR decomposition: X_centered = Q * R
  // The Q matrix (size x inputDimension) provides a standardized
  // representation of X (equivalent to ZCA whitening). The R factor
  // encodes the rotation back to the original coordinate system.
  Matrix Q(size, inputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < inputDimension; ++j)
      Q(i, j) = X_centered(i, j);
  Matrix R;
  Q.computeQRInPlace(R);

  // Step 4  -- Compute the slice means in the Q-space
  // For each slice h, we accumulate the Q-vectors of its members and
  // scale by sqrt(n / n_h). The resulting matrix Z_mean is an
  // (H x inputDimension) matrix whose rows are the weighted slice means:
  //   Z_mean[h, :] = sqrt(n / n_h) * sum_{i in slice h} Q[i, :]
  const Scalar sqrtSize = std::sqrt(1.0 * size);
  Matrix zMeans(sliceNumber_, inputDimension);
  for (UnsignedInteger j = 0; j < sliceNumber_; ++j)
  {
    const UnsignedInteger localSize = chunk_population[j];
    const Scalar scale = sqrtSize / std::sqrt(1.0 * localSize);
    Point sumQ(inputDimension, 0.0);
    for (UnsignedInteger k = 0; k < localSize; ++k)
    {
      const UnsignedInteger index = list_chunk[j][k];
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        sumQ[i] += Q(index, i);
    }
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      zMeans(j, i) = sumQ[i] * scale;
  }

  // Step 5  -- SVD of the slice means matrix
  // zMeans = U * diag(S) * V^T. The columns of V^T are the
  // eigenvectors of zMeans^T * zMeans with eigenvalues =
  // S^2 / size. SVD is more numerically stable than forming
  // the cross-product matrix M explicitly.
  // zMeans is (H x d); SVD gives at most min(H, d) components.
  Matrix U, VT;
  const Point singularValues = zMeans.computeSVD(U, VT, false);
  // singularValues in descending order.
  // VT is (min(H, d) x d); rows are the right singular vectors.
  // Transpose to get eigenvectors as columns: (d x min(H, d)).
  const UnsignedInteger rank = std::min(sliceNumber_, inputDimension);
  Matrix eigenVectors(inputDimension, rank);
  for (UnsignedInteger j = 0; j < rank; ++j)
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      eigenVectors(i, j) = VT(j, i);
  Point eigenValues(rank);
  for (UnsignedInteger j = 0; j < rank; ++j)
    eigenValues[j] = singularValues[j] * singularValues[j] / size;

  // Step 6  -- Back-transform the EDR directions to the original space
  //   alpha = R^{-1} * beta  (where R2 = sqrt(n) * R)
  // This undoes the QR whitening so the directions are expressed in
  // terms of the original centered coordinates.
  TriangularMatrix R2((R * sqrtSize).getImplementation(), false);
  Matrix directions = R2.solveLinearSystemInPlace(eigenVectors);

  // Step 7  -- Keep only the requested number of modes
  const UnsignedInteger effectiveModes = std::min(nbModes_, rank);
  directions.getImplementation()->resize(inputDimension, effectiveModes);
  eigenValues.resize(effectiveModes);

  // Step 9  -- Normalize each direction to unit length
  for (UnsignedInteger j = 0; j < effectiveModes; ++j)
  {
    Scalar normJ = 0.0;
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      normJ += directions(i, j) * directions(i, j);
    const Scalar invNormJ = 1.0 / std::sqrt(normJ);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      directions(i, j) *= invNormJ;
  }
  result_ = SlicedInverseRegressionResult(directions, mean, eigenValues);
}

SlicedInverseRegressionResult SlicedInverseRegression::getResult() const
{
  return result_;
}

/* String converter */
String SlicedInverseRegression::__repr__() const
{
  OSS oss;
  oss << "class=" << SlicedInverseRegression::GetClassName();
  return oss;
}


/* Slice number accessor */
UnsignedInteger SlicedInverseRegression::getSliceNumber() const
{
  return sliceNumber_;
}

void SlicedInverseRegression::setSliceNumber(const UnsignedInteger sliceNumber)
{
  if (sliceNumber == 0)
    throw InvalidArgumentException(HERE) << "Slice number must be positive";
  sliceNumber_ = sliceNumber;
}

/* Modes number accessor */
void SlicedInverseRegression::setNbModes(const UnsignedInteger nbModes)
{
  if (nbModes == 0 || nbModes > inputSample_.getDimension())
    throw InvalidArgumentException(HERE) << "Modes number must be in [1, " << inputSample_.getDimension() << "]";
  nbModes_ = nbModes;
}

UnsignedInteger SlicedInverseRegression::getNbModes() const
{
  return nbModes_;
}

/* Method save() stores the object through the StorageManager */
void SlicedInverseRegression::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "sliceNumber_", sliceNumber_ );
  adv.saveAttribute( "nbModes_", nbModes_ );
  adv.saveAttribute( "result_", result_ );
}

/* Method load() reloads the object from the StorageManager */
void SlicedInverseRegression::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "sliceNumber_", sliceNumber_ );
  adv.loadAttribute( "nbModes_", nbModes_ );
  adv.loadAttribute( "result_", result_ );
}

END_NAMESPACE_OPENTURNS
