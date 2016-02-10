//                                               -*- C++ -*-
/**
 *  @brief  This class is enables to launch an FFT transformation /inverse transformation
 *  This is the interface class
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
#ifndef OPENTURNS_FFT_HXX
#define OPENTURNS_FFT_HXX

#include "PersistentObject.hxx"
#include "FFTImplementation.hxx"
#include "TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FFT
 */

class OT_API FFT
  : public TypedInterfaceObject<FFTImplementation>
{

  CLASSNAME;

public:

  typedef Pointer<FFTImplementation>    Implementation;
  typedef Collection<NumericalScalar>   NumericalScalarCollection;
  typedef Collection<NumericalComplex>  NumericalComplexCollection;

  /** Default onstructor */
  FFT();

  /** Copy constructors */
  FFT(const FFTImplementation & implementation);

  /** Constructor from implementation */
  FFT(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  FFT(FFTImplementation * p_implementation);
#endif

  /** FFT transformation on real
   * Given the real sequence X_n, compute the sequence Z_k such that:
   * Z_k = \sum_{n=0}^{N-1} X_n\exp(-\frac{2i\pi kn}{N})
   */
  NumericalComplexCollection transform(const NumericalScalarCollection & collection) const;

  /** FFT transformation on real - The transformation is applied on a part of the collection*/
  NumericalComplexCollection transform(const NumericalScalarCollection & collection,
                                       const UnsignedInteger first,
                                       const UnsignedInteger size) const;

  /** FFT transformation on real with a regular sequence of the collection (between first and last, by step = step)*/
  NumericalComplexCollection transform(const NumericalScalarCollection & collection,
                                       const UnsignedInteger first,
                                       const UnsignedInteger step,
                                       const UnsignedInteger last) const;

  /** FFT transformation on complex
   * Given the complex sequence Y_n, compute the sequence Z_k such that:
   * Z_k = \sum_{n=0}^{N-1} Y_n\exp(-\frac{2i\pi kn}{N})
   */
  NumericalComplexCollection transform(const NumericalComplexCollection & collection) const;

  /** FFT transformation on complex - For some FFT implementation, the transformation is applied on a part of the collection */
  NumericalComplexCollection transform(const NumericalComplexCollection & collection,
                                       const UnsignedInteger first,
                                       const UnsignedInteger size) const;

  /** FFT transformation on complex - For some FFT implementation, the need is to transform a regular sequence of the collection (between first and last, by step = step)*/
  NumericalComplexCollection transform(const NumericalComplexCollection & collection,
                                       const UnsignedInteger first,
                                       const UnsignedInteger step,
                                       const UnsignedInteger last) const;

  /** FFT 2D transformation on real
   * Given the real sequence X_n, compute the sequence Z_k such that:
   * Z_{k,l} = \sum_{m=0}^{M-1}\sum_{n=0}^{N-1} X_{m,n}\exp(-\frac{2i\pi km}{M}) \exp(-\frac{2i\pi ln}{N})
   */
  ComplexMatrix transform2D(const ComplexMatrix & matrix) const;

  /** FFT 2D transformation on complex */
  ComplexMatrix transform2D(const Matrix & matrix) const;

  /** FFT 2D transformation on complex */
  ComplexMatrix transform2D(const NumericalSample & sample) const;

  /** FFT 3D transformation
   * Given the real sequence X, compute the sequence Z such that:
   * Z_{k,l,r} = \sum_{m=0}^{M-1}\sum_{n=0}^{N-1}\sum_{p=0}^{P-1} X_{m,n,p}\exp(-\frac{2i\pi km}{M}) \exp(-\frac{2i\pi ln}{N}) \exp(-\frac{2i\pi rp}{P})
   */
  ComplexTensor transform3D(const ComplexTensor & tensor) const;

  /** FFT 3D transformation on real data */
  ComplexTensor transform3D(const Tensor & tensor) const;


  /** FFT inverse transformation
   * Given the complex sequence Z_n, compute the sequence Y_k such that:
   * Y_k = \frac{1}{N}\sum_{n=0}^{N-1} Z_n\exp(\frac{2i\pi kn}{N})
   */
  /** FFT inverse transformation */
  NumericalComplexCollection inverseTransform(const NumericalScalarCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  NumericalComplexCollection inverseTransform(const NumericalScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT inverse transformation on a regular sequence of the collection (between first and last, spearated  by step)*/
  NumericalComplexCollection inverseTransform(const NumericalScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT inverse transformation on a regular sequence of the collection (between first and last, by step = step)*/
  NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** FFT inverse transformation
   * Given the complex sequence Z_n, compute the sequence Y_k such that:
   * Y_{k,l} = \frac{1}{M.N}\sum_{m=0}^{M-1}\sum_{n=0}^{N-1} Z_{m,n}\exp(\frac{2i\pi km}{M} \exp(\frac{2i\pi ln}{N})
   */
  ComplexMatrix inverseTransform2D(const ComplexMatrix & matrix) const;

  /** IFFT 2D transformation on real matrix */
  ComplexMatrix inverseTransform2D(const Matrix & matrix) const;

  /** IFFT 2D transformation on sample */
  ComplexMatrix inverseTransform2D(const NumericalSample & sample) const;

  /** FFT inverse transformation
   * Given the complex sequence Z, compute the sequence Y such that:
   * Y_{k,l,r} = \frac{1}{M.N.P}\sum_{m=0}^{M-1}\sum_{n=0}^{N-1}\sum_{n=0}^{P-1} Z_{m,n,p}\exp(\frac{2i\pi km}{M} \exp(\frac{2i\pi ln}{N}) \exp(\frac{2i\pi rp}{P})
   */
  ComplexTensor inverseTransform3D(const ComplexTensor & tensor) const;

  /** IFFT 3D transformation on real tensors */
  ComplexTensor inverseTransform3D(const Tensor & tensor) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;


} ; /* class FFT */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FFT_HXX */
