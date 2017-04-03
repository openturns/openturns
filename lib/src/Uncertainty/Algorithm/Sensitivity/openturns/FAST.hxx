//                                               -*- C++ -*-
/**
 * @brief FAST implements the sensivity analysis method based on fourier decomposition
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_FAST_HXX
#define OPENTURNS_FAST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/FFT.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FAST
 *
 */

class OT_API FAST
{
public:

  typedef Collection<NumericalComplex> NumericalComplexCollection;

  /** Constructor with model */
  FAST(const Function & model,
       const Distribution & inputsDistribution,
       const UnsignedInteger samplingSize,
       const UnsignedInteger resamplingSize = ResourceMap::GetAsUnsignedInteger("FAST-DefaultResamplingSize"),
       const UnsignedInteger interferenceFactor = ResourceMap::GetAsUnsignedInteger("FAST-DefaultInterferenceFactor"));

  /** First order indices accessor */
  NumericalPoint getFirstOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** Total order indices accessor */
  NumericalPoint getTotalOrderIndices(const UnsignedInteger marginalIndex = 0) const;

  /** FFT algorithm accessor */
  FFT getFFTAlgorithm() const;
  void setFFTAlgorithm(const FFT & fft);

  /** Block size accessor */
  void setBlockSize(const UnsignedInteger blockSize);
  UnsignedInteger getBlockSize() const;

private:
  /** Compute all the FAST indices */
  void run() const;

  /** The model */
  Function model_;

  /** Inputs distribution */
  Distribution inputsDistribution_;

  /** Parameters of FAST method */
  UnsignedInteger samplingSize_;
  UnsignedInteger blockSize_;
  UnsignedInteger resamplingSize_;
  UnsignedInteger interferenceFactor_;

  /** First order indices */
  mutable NumericalSample firstOrderIndice_;

  /** Total order indices */
  mutable NumericalSample totalOrderIndice_;

  /** Store if indices are already computed */
  mutable Bool alreadyComputedIndices_;

  /** FFT algorithm */
  FFT fftAlgorithm_;

}; /* class FAST */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FAST_HXX */
