//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build an exponential covariance
 *  model, a second order model's implementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_KISSFFT_HXX
#define OPENTURNS_KISSFFT_HXX

#include "PersistentObject.hxx"
#include "FFTImplementation.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KissFFT
 */

class OT_API KissFFT
  : public FFTImplementation
{

  CLASSNAME;

public:


  /** Default constructor without parameters */
  KissFFT();

  /** Virtual copy constructor */
  KissFFT * clone() const;

  /** FFT transformation on complex */
  NumericalComplexCollection transform(const NumericalComplexCollection & collection) const;

  /** FFT transformation  with application on a block-part of the collection */
  NumericalComplexCollection transform(const NumericalComplexCollection & collection,
                                       const UnsignedInteger first,
                                       const UnsignedInteger size) const;

  /** FFT inverse transformation */
  NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection) const;

  /** FFT inverse transformation with application on a block-part of the collection */
  NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT 2D transformation on complex */
  ComplexMatrix transform2D(const ComplexMatrix & complexMatrix) const;

  /** FFT 2D transformation on real matrix */
  ComplexMatrix transform2D(const Matrix & matrix) const;

  /** FFT 2D transformation on sample */
  ComplexMatrix transform2D(const NumericalSample & sample) const;

  /** IFFT 2D transformation on complex */
  ComplexMatrix inverseTransform2D(const ComplexMatrix & complexMatrix) const;

  /** IFFT 2D transformation on real matrix */
  ComplexMatrix inverseTransform2D(const Matrix & matrix) const;

  /** IFFT 2D transformation on sample */
  ComplexMatrix inverseTransform2D(const NumericalSample & sample) const;

  /** FFT 3D transformation */
  ComplexTensor transform3D(const ComplexTensor & tensor) const;

  /** FFT 3D transformation on real data */
  ComplexTensor transform3D(const Tensor & tensor) const;

  /** IFFT 3D transformation */
  ComplexTensor inverseTransform3D(const ComplexTensor & tensor) const;

  /** IFFT 3D transformation */
  ComplexTensor inverseTransform3D(const Tensor & tensor) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

private:

  ComplexMatrix fft2D(const ComplexMatrix & complexMatrix, const Bool isIFFT) const;
  ComplexTensor fft3D(const ComplexTensor & tensor, const Bool isIFFT) const;

} ; /* class KissFFT */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KISSFFT_HXX */
