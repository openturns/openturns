//                                               -*- C++ -*-
/**
 *  @brief  This class is enables to launch an FFT transformation /inverse transformation
 *  This is the interface class
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
#ifndef OPENTURNS_FFT_HXX
#define OPENTURNS_FFT_HXX

#include "openturns/FFTImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FFT
 */

class OT_API FFT
  : public TypedInterfaceObject<FFTImplementation>
{

  CLASSNAME

public:

  typedef Pointer<FFTImplementation>    Implementation;
  typedef Collection<Scalar>   ScalarCollection;
  typedef Collection<Complex>  ComplexCollection;

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
  ComplexCollection transform(const ScalarCollection & collection) const;

  /** FFT transformation on real - The transformation is applied on a part of the collection*/
  ComplexCollection transform(const ScalarCollection & collection,
                              const UnsignedInteger first,
                              const UnsignedInteger size) const;

  /** FFT transformation on real with a regular sequence of the collection (between first and last, by step = step)*/
  ComplexCollection transform(const ScalarCollection & collection,
                              const UnsignedInteger first,
                              const UnsignedInteger step,
                              const UnsignedInteger last) const;

  /** FFT transformation on complex
   * Given the complex sequence Y_n, compute the sequence Z_k such that:
   * Z_k = \sum_{n=0}^{N-1} Y_n\exp(-\frac{2i\pi kn}{N})
   */
  ComplexCollection transform(const ComplexCollection & collection) const;

  /** FFT transformation on complex - For some FFT implementation, the transformation is applied on a part of the collection */
  ComplexCollection transform(const ComplexCollection & collection,
                              const UnsignedInteger first,
                              const UnsignedInteger size) const;

  /** FFT transformation on complex - For some FFT implementation, the need is to transform a regular sequence of the collection (between first and last, by step = step)*/
  ComplexCollection transform(const ComplexCollection & collection,
                              const UnsignedInteger first,
                              const UnsignedInteger step,
                              const UnsignedInteger last) const;

  /** FFT ND transformation on data stored as a flat array */
  ComplexCollection transform(const Point & data,
                              const Indices & dimensions) const;

  /** FFT ND transformation on complex data stored as a flat array */
  ComplexCollection transform(const ComplexCollection & data,
                              const Indices & dimensions) const;

  /** FFT inverse transformation
   * Given the complex sequence Z_n, compute the sequence Y_k such that:
   * Y_k = \frac{1}{N}\sum_{n=0}^{N-1} Z_n\exp(\frac{2i\pi kn}{N})
   */
  /** FFT inverse transformation */
  ComplexCollection inverseTransform(const ScalarCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  ComplexCollection inverseTransform(const ScalarCollection & collection,
                                     const UnsignedInteger first,
                                     const UnsignedInteger size) const;

  /** FFT inverse transformation on a regular sequence of the collection (between first and last, separated by step)*/
  ComplexCollection inverseTransform(const ScalarCollection & collection,
                                     const UnsignedInteger first,
                                     const UnsignedInteger step,
                                     const UnsignedInteger last) const;

  ComplexCollection inverseTransform(const ComplexCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  ComplexCollection inverseTransform(const ComplexCollection & collection,
                                     const UnsignedInteger first,
                                     const UnsignedInteger size) const;

  /** FFT inverse transformation on a regular sequence of the collection (between first and last, by step = step)*/
  ComplexCollection inverseTransform(const ComplexCollection & collection,
                                     const UnsignedInteger first,
                                     const UnsignedInteger step,
                                     const UnsignedInteger last) const;

  /** IFFT ND transformation on data stored as a flat array */
  ComplexCollection inverseTransform(const Point & data,
                                     const Indices & dimensions) const;

  /** IFFT ND transformation on complex data stored as a flat array */
  ComplexCollection inverseTransform(const ComplexCollection & data,
                                     const Indices & dimensions) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;


} ; /* class FFT */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FFT_HXX */
