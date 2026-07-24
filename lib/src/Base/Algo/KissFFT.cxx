//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/KissFFT.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "kissfft.hh"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef kissfft<Scalar> KISSFFTScalar;

struct FFTPolicy
{
  const KissFFT::ComplexCollection & input_;
  KissFFT::ComplexCollection  & output_;
  KISSFFTScalar & fft_;
  const UnsignedInteger fftSize_;

  FFTPolicy(const KissFFT::ComplexCollection & input,
            KissFFT::ComplexCollection & output,
            KISSFFTScalar & fft,
            UnsignedInteger fftSize)
    : input_(input)
    , output_(output)
    , fft_(fft)
    , fftSize_(fftSize)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger first = fftSize_ * i;
      fft_.transform( &input_[first], &output_[first] );
    }
  }
}; /* end struct FFTPolicy */

/**
 * @class KissFFT
 */

CLASSNAMEINIT(KissFFT)

static const Factory<KissFFT> Factory_KissFFT;

/* Constructor with parameters */
KissFFT::KissFFT()
  : FFTImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
KissFFT * KissFFT::clone() const
{
  return new KissFFT(*this);
}

/* String converter */
String KissFFT::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KissFFT::GetClassName();
  return oss;
}

/* String converter */
String KissFFT::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << KissFFT::GetClassName();
  return oss;
}


/* FFT transformation on complex */
KissFFT::ComplexCollection KissFFT::transform(const ComplexCollection & collection) const
{
  // Get the size of the collection
  const UnsignedInteger fftSize = collection.getSize();

  // Init of kissfft  - Boolean argument is to tell that we perform a direct transformation
  KISSFFTScalar fft(fftSize, false);

  // Allocation of memory for final result
  ComplexCollection transformationResult(fftSize);
  fft.transform( &collection[0], &transformationResult[0] );
  return transformationResult;
}

/* FFT transformation on a regular complex sequence of the collection (between first and last, with elements separated by step)*/
KissFFT::ComplexCollection KissFFT::transform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  // Init of kissfft  - Boolean argument is to tell that we perform a direct transformation
  KISSFFTScalar fft(size, false);

  // Allocation of memory for final result
  ComplexCollection transformationResult(size);
  fft.transform( &collection[first], &transformationResult[0] );
  return transformationResult;
}

/* FFT T transformation */
KissFFT::ComplexCollection KissFFT::inverseTransform(const ComplexCollection & collection) const
{
  // Get the size of the collection
  const UnsignedInteger size = collection.getSize();

  // Init of kissfft  - Boolean argument is to tell that we perform an inverse transformation
  KISSFFTScalar fft(size, true);

  // Allocation of memory for final result
  ComplexCollection transformationResult(size);

  // Inverse transform = transform with the opposite sign in the exponent and a 1/N factor
  // Call inverse transformation
  fft.transform( &collection[0], &transformationResult[0] );
  const Scalar alpha = 1.0 / size;
  for (UnsignedInteger index = 0; index < size; ++index) transformationResult[index] *= alpha;
  return transformationResult;
}

/* FFT transformation on a regular complex sequence of the collection (between first and last, with elements separated by step)*/
KissFFT::ComplexCollection KissFFT::inverseTransform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  // Init of kissfft  - Boolean argument is true
  KISSFFTScalar fft(size, true);

  // Allocation of memory for final result
  ComplexCollection transformationResult(size);
  fft.transform( &collection[first], &transformationResult[0] );
  const Scalar alpha = 1.0 / size;
  for (UnsignedInteger index = 0; index < size; ++index) transformationResult[index] *= alpha;
  return transformationResult;
}

KissFFT::ComplexCollection KissFFT::fftND(const ComplexCollection & data,
                                const Indices & dimensions,
                                const Bool isIFFT) const
{
  const UnsignedInteger nDims = dimensions.getSize();
  UnsignedInteger totalSize = 1;
  for (UnsignedInteger i = 0; i < nDims; ++i)
    totalSize *= dimensions[i];
  if (data.getSize() != totalSize)
    throw InvalidArgumentException(HERE) << "In KissFFT::fftND: data size (" << data.getSize()
                                         << ") does not match product of dimensions (" << totalSize << ")";

  ComplexCollection work(data);
  ComplexCollection result(totalSize);
  UnsignedInteger stride = 1;
  for (UnsignedInteger dim = 0; dim < nDims; ++dim)
  {
    const UnsignedInteger dimSize = dimensions[dim];
    const UnsignedInteger numFFTs = totalSize / dimSize;
    KISSFFTScalar fft(dimSize, isIFFT);
    struct FFTPolicyND
    {
      const ComplexCollection & work_;
      ComplexCollection & result_;
      KISSFFTScalar & fft_;
      const UnsignedInteger dimSize_;
      const UnsignedInteger stride_;

      inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
      {
        ComplexCollection buf(dimSize_);
        ComplexCollection out(dimSize_);
        for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
        {
          const UnsignedInteger lower = i % stride_;
          const UnsignedInteger upper = i / stride_;
          const UnsignedInteger base = lower + upper * dimSize_ * stride_;
          for (UnsignedInteger j = 0; j < dimSize_; ++j)
            buf[j] = work_[base + j * stride_];
          fft_.transform( &buf[0], &out[0] );
          for (UnsignedInteger j = 0; j < dimSize_; ++j)
            result_[base + j * stride_] = out[j];
        }
      }
    };

    const FFTPolicyND policy{work, result, fft, dimSize, stride};
    TBBImplementation::ParallelFor( 0, numFFTs, policy );
    stride *= dimSize;
    if (dim < nDims - 1) work = result;
  }
  if (isIFFT)
  {
    const Complex factor(1.0 / totalSize);
    for (UnsignedInteger i = 0; i < totalSize; ++i)
      result[i] *= factor;
  }
  return result;
}

/* FFT ND transformation on real data */
KissFFT::ComplexCollection KissFFT::transform(const Point & data,
    const Indices & dimensions) const
{
  const UnsignedInteger totalSize = data.getSize();
  ComplexCollection complexData(totalSize);
  for (UnsignedInteger i = 0; i < totalSize; ++i)
    complexData[i] = Complex(data[i], 0.0);
  return transform(complexData, dimensions);
}

/* FFT ND transformation on complex data */
KissFFT::ComplexCollection KissFFT::transform(const ComplexCollection & data,
    const Indices & dimensions) const
{
  return fftND(data, dimensions, false);
}

/* IFFT ND transformation on real data */
KissFFT::ComplexCollection KissFFT::inverseTransform(const Point & data,
    const Indices & dimensions) const
{
  const UnsignedInteger totalSize = data.getSize();
  ComplexCollection complexData(totalSize);
  for (UnsignedInteger i = 0; i < totalSize; ++i)
    complexData[i] = Complex(data[i], 0.0);
  return inverseTransform(complexData, dimensions);
}

/* IFFT ND transformation on complex data */
KissFFT::ComplexCollection KissFFT::inverseTransform(const ComplexCollection & data,
    const Indices & dimensions) const
{
  return fftND(data, dimensions, true);
}

END_NAMESPACE_OPENTURNS
