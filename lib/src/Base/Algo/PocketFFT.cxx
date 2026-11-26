//                                               -*- C++ -*-
/**
 *  @brief This class is an FFT implementation based on the PocketFFT library
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
#include "openturns/PocketFFT.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/TBBImplementation.hxx"
#include <algorithm>

// The vendored PocketFFT single-header implementation is included here and
// nowhere else: its API is a set of inline templates over std::complex<double>,
// so it must not leak into an installed header, following the same policy as
// the vendored kissfft.hh (see Base/Algo/CMakeLists.txt, where its installation
// is deliberately commented out).
// The library is
//   M. Reinecke and P. Bell, pocketfft, https://github.com/hayguen/pocketfft.
// Its small radix kernels follow the decomposition of the FFTPACK routine
// library:
//   P. N. Swarztrauber, "Vectorizing the FFTs", in Parallel Computations,
//   Academic Press, 1982, pp. 51-83.
//   P. N. Paul, "Fftpack - a subroutine library for computing real and
//   complex Fourier transforms", ACM Transactions on Mathematical Software
//   15(2), 1989, pp. 168-183.
// Sizes not factorable by those radices may be handled by Bluestein's
// algorithm, also known as the chirp-z transform, when the estimated cost
// of the latter is lower than the one of the factorization:
//   D. M. Bluestein, "A linear filtering approach to the computation of the
//   discrete Fourier transform", IEEE Transactions on Audio and
//   Electroacoustics 18(4), 1970, pp. 451-455.
//   L. R. Rabiner, R. W. Schafer and C. M. Rader, "The chirp z-transform
//   algorithm and its application", Bell System Technical Journal 48(5),
//   1969, pp. 1249-1292.
// POCKETFFT_NO_MULTITHREADING, read at include time, removes the internal
// std::thread pool of the header: OpenTURNS drives the parallelism with TBB in
// the policies below, and two schedulers competing for the same cores is a
// known anti-pattern.  With the macro defined the header reports a single
// thread for every call, hence the nthreads argument passed to pocketfft::c2c
// is always 1.
// POCKETFFT_CACHE_SIZE keeps its default of 0, i.e. no plan cache, so no state
// is shared between concurrent TBB tasks.  A plan is rebuilt for each
// pocketfft::c2c call, which is why a call must cover as many transforms as
// possible: see PocketFFT::fftND.
#define POCKETFFT_NO_MULTITHREADING
#include "pocketfft_hdronly.h"

BEGIN_NAMESPACE_OPENTURNS

namespace {

/** Byte stride of every axis of a column-major tensor of the given shape,
    as expected by pocketfft, whose API imposes the std::vector types
    (shape_t is std::vector<size_t> and stride_t is std::vector<ptrdiff_t>).
    The axis strides are the ones reproducing the column-major (Fortran)
    indexing used everywhere in OpenTURNS, where the first axis varies
    fastest. */
pocketfft::stride_t ComputeByteStrides(const pocketfft::shape_t & shape)
{
  const UnsignedInteger nDims = shape.size();
  const UnsignedInteger elementSize = sizeof(std::complex<Scalar>);
  pocketfft::stride_t strides(nDims);
  std::ptrdiff_t current = static_cast<std::ptrdiff_t>(elementSize);
  for (UnsignedInteger axis = 0; axis < nDims; ++axis)
  {
    strides[axis] = current;
    current *= static_cast<std::ptrdiff_t>(shape[axis]);
  }
  return strides;
}

} // namespace

/**
 * @class PocketFFT
 */

CLASSNAMEINIT(PocketFFT)

static const Factory<PocketFFT> Factory_PocketFFT;

/* Constructor with parameters */
PocketFFT::PocketFFT()
  : FFTImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
PocketFFT * PocketFFT::clone() const
{
  return new PocketFFT(*this);
}

/* String converter */
String PocketFFT::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PocketFFT::GetClassName();
  return oss;
}

/* String converter */
String PocketFFT::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << PocketFFT::GetClassName();
  return oss;
}

/* FFT transformation on complex */
PocketFFT::ComplexCollection PocketFFT::transform(const ComplexCollection & collection) const
{
  const UnsignedInteger size = collection.getSize();
  const UnsignedInteger elementSize = sizeof(std::complex<Scalar>);
  const pocketfft::shape_t shape(1, size);
  const pocketfft::stride_t stride(1, static_cast<std::ptrdiff_t>(elementSize));
  const pocketfft::shape_t axes(1, 0);
  // A Collection only exposes a const data() accessor, and the buffer below is
  // a local object whose ownership we take, hence the same const_cast idiom as
  // the LAPACK call sites in ComplexMatrixImplementation.cxx
  ComplexCollection transformationResult(size);
  pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD,
                 collection.data(),
                 const_cast<Complex *>(transformationResult.data()),
                 1.0, 1);
  return transformationResult;
}

/* FFT transformation on a block-part of the complex collection */
PocketFFT::ComplexCollection PocketFFT::transform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  const UnsignedInteger collectionSize = collection.getSize();
  if (first > collectionSize || size > collectionSize - first)
    throw InvalidArgumentException(HERE) << "In PocketFFT::transform: requested block ["
                                         << first << ", " << first + size << ") is out of bounds"
                                         << " for a collection of size " << collectionSize;
  const UnsignedInteger elementSize = sizeof(std::complex<Scalar>);
  const pocketfft::shape_t shape(1, size);
  const pocketfft::stride_t stride(1, static_cast<std::ptrdiff_t>(elementSize));
  const pocketfft::shape_t axes(1, 0);
  ComplexCollection transformationResult(size);
  pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD,
                 collection.data() + first,
                 const_cast<Complex *>(transformationResult.data()),
                 1.0, 1);
  return transformationResult;
}

/* FFT inverse transformation */
PocketFFT::ComplexCollection PocketFFT::inverseTransform(const ComplexCollection & collection) const
{
  const UnsignedInteger size = collection.getSize();
  // The 1 / N normalization is folded into the pocketfft forward factor,
  // which removes the explicit scalar loop needed with kissfft
  const Scalar alpha = (size == 0) ? 1.0 : 1.0 / size;
  const UnsignedInteger elementSize = sizeof(std::complex<Scalar>);
  const pocketfft::shape_t shape(1, size);
  const pocketfft::stride_t stride(1, static_cast<std::ptrdiff_t>(elementSize));
  const pocketfft::shape_t axes(1, 0);
  ComplexCollection transformationResult(size);
  pocketfft::c2c(shape, stride, stride, axes, pocketfft::BACKWARD,
                 collection.data(),
                 const_cast<Complex *>(transformationResult.data()),
                 alpha, 1);
  return transformationResult;
}

/* FFT inverse transformation on a block-part of the complex collection */
PocketFFT::ComplexCollection PocketFFT::inverseTransform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  const UnsignedInteger collectionSize = collection.getSize();
  if (first > collectionSize || size > collectionSize - first)
    throw InvalidArgumentException(HERE) << "In PocketFFT::inverseTransform: requested block ["
                                         << first << ", " << first + size << ") is out of bounds"
                                         << " for a collection of size " << collectionSize;
  const Scalar alpha = (size == 0) ? 1.0 : 1.0 / size;
  const UnsignedInteger elementSize = sizeof(std::complex<Scalar>);
  const pocketfft::shape_t shape(1, size);
  const pocketfft::stride_t stride(1, static_cast<std::ptrdiff_t>(elementSize));
  const pocketfft::shape_t axes(1, 0);
  ComplexCollection transformationResult(size);
  pocketfft::c2c(shape, stride, stride, axes, pocketfft::BACKWARD,
                 collection.data() + first,
                 const_cast<Complex *>(transformationResult.data()),
                 alpha, 1);
  return transformationResult;
}

/* ND transform in place, over every axis of the given shape */
void PocketFFT::fftND(ComplexCollection & data,
                      const Indices & dimensions,
                      const Bool isIFFT) const
{
  const UnsignedInteger nDims = dimensions.getSize();
  UnsignedInteger totalSize = 1;
  for (UnsignedInteger axis = 0; axis < nDims; ++axis)
  {
    if (dimensions[axis] == 0)
      throw InvalidArgumentException(HERE) << "In PocketFFT::fftND: dimension " << axis << " is zero";
    // Reject the shape before the product can exceed the data size and wrap
    // around the UnsignedInteger range, as [65536, 65536] would when it is a
    // 32-bit unsigned int
    if (totalSize > data.getSize() / dimensions[axis])
      throw InvalidArgumentException(HERE) << "In PocketFFT::fftND: data size (" << data.getSize()
                                           << ") does not match product of dimensions";
    totalSize *= dimensions[axis];
  }
  if (data.getSize() != totalSize)
    throw InvalidArgumentException(HERE) << "In PocketFFT::fftND: data size (" << data.getSize()
                                         << ") does not match product of dimensions (" << totalSize << ")";
  // An empty shape is the identity: the single value is returned unchanged
  if (nDims == 0)
    return;

  const pocketfft::shape_t shape(dimensions.begin(), dimensions.end());
  const pocketfft::stride_t strides(ComputeByteStrides(shape));
  const UnsignedInteger elementSize = sizeof(std::complex<Scalar>);
  // The 1 / totalSize normalization of the inverse transform must be applied
  // exactly once for the whole ND transform, hence on the last axis only,
  // after every axis has performed its transform unscaled, as KissFFT does
  // applying the factor after all the passes: applying it on the first axis
  // would flush subnormal values to zero before the following axes sum them.
  // Each axis is a separate pocketfft::c2c call
  const Scalar scale = isIFFT ? 1.0 / totalSize : 1.0;
  // At least one task, and never more tasks than transforms or more tasks than
  // index groups along the axis used for the partition
  const UnsignedInteger maxThreads = std::max(UnsignedInteger(1), TBBImplementation::GetThreadsNumber());

  for (UnsignedInteger axis = 0; axis < nDims; ++axis)
  {
    const UnsignedInteger axisLength = shape[axis];
    // Number of independent 1D transforms along the current axis
    const UnsignedInteger numFFTs = totalSize / axisLength;
    // The independent transforms are partitioned along the longest axis
    // different from the current one: each task handles a range of the index
    // values of that axis, which pocketfft expresses as a sub-shape keeping
    // the same strides, so that the tasks cover disjoint sets of lines.  One
    // pocketfft::c2c call per task, and not per line, is required because the
    // header rebuilds its plan on every call with POCKETFFT_CACHE_SIZE 0, so
    // one call per line would pay the plan construction numFFTs times.  With
    // a single dimension there is nothing to partition, and a single task
    // leaves the shape untouched
    UnsignedInteger batchAxis = 0;
    UnsignedInteger batchLength = shape[0];
    UnsignedInteger numTasks = 1;
    if (nDims > 1)
    {
      batchAxis = (axis == 0) ? 1 : 0;
      for (UnsignedInteger candidate = 0; candidate < nDims; ++candidate)
        if (candidate != axis && shape[candidate] > shape[batchAxis])
          batchAxis = candidate;
      batchLength = shape[batchAxis];
      // Never more tasks than transforms or index groups along the batch
      // axis; limit is taken when 4 * maxThreads would wrap the
      // UnsignedInteger range, i.e. for thread counts above 2^30
      const UnsignedInteger limit = std::min(numFFTs, batchLength);
      numTasks = (maxThreads > limit / UnsignedInteger(4)) ? limit : 4 * maxThreads;
    }
    // Every task gets chunkSize indices, the first remainder tasks one extra
    // index, so that no task ends up handling the whole remainder alone
    const UnsignedInteger chunkSize = batchLength / numTasks;
    const UnsignedInteger remainder = batchLength % numTasks;
    const std::ptrdiff_t batchElementStride = strides[batchAxis] / static_cast<std::ptrdiff_t>(elementSize);
    const pocketfft::shape_t axes(1, axis);

    struct TransformPolicyND
    {
      ComplexCollection & data_;
      const pocketfft::shape_t & shape_;
      const pocketfft::stride_t & strides_;
      const pocketfft::shape_t & axes_;
      const UnsignedInteger batchAxis_;
      const UnsignedInteger chunkSize_;
      const UnsignedInteger remainder_;
      const std::ptrdiff_t batchElementStride_;
      const bool forward_;
      const Scalar factor_;

      inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & range) const
      {
        for (UnsignedInteger task = range.begin(); task != range.end(); ++task)
        {
          const UnsignedInteger start = task * chunkSize_ + std::min(task, remainder_);
          const UnsignedInteger chunkLength = chunkSize_ + (task < remainder_ ? 1 : 0);
          pocketfft::shape_t chunkShape(shape_);
          chunkShape[batchAxis_] = chunkLength;
          Complex * lineBase = const_cast<Complex *>(data_.data()) + start * batchElementStride_;
          pocketfft::c2c(chunkShape, strides_, strides_, axes_, forward_,
                         lineBase, lineBase, factor_, 1);
        }
      }
    };

    const TransformPolicyND policy{data, shape, strides, axes, batchAxis,
                                   chunkSize, remainder, batchElementStride,
                                   isIFFT ? pocketfft::BACKWARD : pocketfft::FORWARD,
                                   (axis + 1 == nDims) ? scale : 1.0};
    TBBImplementation::ParallelFor(0, numTasks, policy);
  }
}

/* FFT ND transformation on real data */
PocketFFT::ComplexCollection PocketFFT::transform(const Point & data,
    const Indices & dimensions) const
{
  const UnsignedInteger totalSize = data.getSize();
  ComplexCollection complexData(totalSize);
  for (UnsignedInteger i = 0; i < totalSize; ++i)
    complexData[i] = Complex(data[i], 0.0);
  return transform(complexData, dimensions);
}

/* FFT ND transformation on complex data */
PocketFFT::ComplexCollection PocketFFT::transform(const ComplexCollection & data,
    const Indices & dimensions) const
{
  ComplexCollection result(data);
  fftND(result, dimensions, false);
  return result;
}

/* IFFT ND transformation on real data */
PocketFFT::ComplexCollection PocketFFT::inverseTransform(const Point & data,
    const Indices & dimensions) const
{
  const UnsignedInteger totalSize = data.getSize();
  ComplexCollection complexData(totalSize);
  for (UnsignedInteger i = 0; i < totalSize; ++i)
    complexData[i] = Complex(data[i], 0.0);
  return inverseTransform(complexData, dimensions);
}

/* IFFT ND transformation on complex data */
PocketFFT::ComplexCollection PocketFFT::inverseTransform(const ComplexCollection & data,
    const Indices & dimensions) const
{
  ComplexCollection result(data);
  fftND(result, dimensions, true);
  return result;
}

END_NAMESPACE_OPENTURNS
