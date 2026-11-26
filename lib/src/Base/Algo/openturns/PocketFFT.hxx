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
#ifndef OPENTURNS_POCKETFFT_HXX
#define OPENTURNS_POCKETFFT_HXX

#include "openturns/FFTImplementation.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PocketFFT
 */

class OT_API PocketFFT
  : public FFTImplementation
{

  CLASSNAME

public:


  /** Default constructor without parameters */
  PocketFFT();

  /** Virtual copy constructor */
  PocketFFT * clone() const override;

  /** FFT transformation on complex */
  ComplexCollection transform(const ComplexCollection & collection) const override;

  /** FFT transformation with application on a block-part of the collection */
  ComplexCollection transform(const ComplexCollection & collection,
                              const UnsignedInteger first,
                              const UnsignedInteger size) const override;

  /** FFT inverse transformation */
  ComplexCollection inverseTransform(const ComplexCollection & collection) const override;

  /** FFT inverse transformation with application on a block-part of the collection */
  ComplexCollection inverseTransform(const ComplexCollection & collection,
                                     const UnsignedInteger first,
                                     const UnsignedInteger size) const override;

  /** FFT ND transformation on data stored as a flat array */
  ComplexCollection transform(const Point & data,
                              const Indices & dimensions) const override;

  /** FFT ND transformation on complex data stored as a flat array */
  ComplexCollection transform(const ComplexCollection & data,
                              const Indices & dimensions) const override;

  /** IFFT ND transformation on data stored as a flat array */
  ComplexCollection inverseTransform(const Point & data,
                                     const Indices & dimensions) const override;

  /** IFFT ND transformation on complex data stored as a flat array */
  ComplexCollection inverseTransform(const ComplexCollection & data,
                                     const Indices & dimensions) const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

private:

  /** Apply the ND transform in place over the axes of the given shape,
      scaling the result by 1 / product(dimensions) when isIFFT is true */
  void fftND(ComplexCollection & data,
             const Indices & dimensions,
             const Bool isIFFT) const;

} ; /* class PocketFFT */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POCKETFFT_HXX */
