//                                               -*- C++ -*-
/**
 *  @brief This class is enables to launch an FFT transform/inverse transform
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
#ifndef OPENTURNS_FFTIMPLEMENTATION_HXX
#define OPENTURNS_FFTIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/ComplexMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Sample.hxx"
#include "openturns/ComplexTensor.hxx"
#include "openturns/Tensor.hxx"
#include "openturns/Point.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FFTImplementation
 */

class OT_API FFTImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  typedef Collection<Scalar>   ScalarCollection;
  typedef Collection<Complex>  ComplexCollection;

  /** Default constructor without parameters */
  FFTImplementation();

  /** Virtual copy constructor */
  FFTImplementation * clone() const override;

  /** FFT transformation on real */
  virtual ComplexCollection transform(const ScalarCollection & collection) const;

  /** FFT transformation on real - The transformation is applied on a part of the collection*/
  virtual ComplexCollection transform(const ScalarCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger size) const;

  /** FFT transformation on real with a regular sequence of the collection (between first and last, by step = step)*/
  virtual ComplexCollection transform(const ScalarCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger step,
                                      const UnsignedInteger last) const;

  /** FFT transformation on complex */
  virtual ComplexCollection transform(const ComplexCollection & collection) const;

  /** FFT transformation on complex - For some FFT implementation, the transformation is applied on a part of the collection */
  virtual ComplexCollection transform(const ComplexCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger size) const;

  /** FFT transformation on complex - For some FFT implementation, the need is to transform a regular sequence of the collection (between first and last, by step = step)*/
  virtual ComplexCollection transform(const ComplexCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger step,
                                      const UnsignedInteger last) const;

  /** FFT ND transformation on data stored as a flat array */
  virtual ComplexCollection transform(const Point & data,
                                      const Indices & dimensions) const;

  /** FFT ND transformation on complex data stored as a flat array */
  virtual ComplexCollection transform(const ComplexCollection & data,
                                      const Indices & dimensions) const;

  /** FFT inverse transformation */
  virtual ComplexCollection inverseTransform(const ScalarCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  virtual ComplexCollection inverseTransform(const ScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT inverse transformation on a regular sequence of the collection (between first and last, separated by step)*/
  virtual ComplexCollection inverseTransform(const ScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** FFT inverse transformation */
  virtual ComplexCollection inverseTransform(const ComplexCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  virtual ComplexCollection inverseTransform(const ComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT transformation on a regular sequence of the collection (between first and last, separated by step)*/
  virtual ComplexCollection inverseTransform(const ComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** IFFT ND transformation on data stored as a flat array */
  virtual ComplexCollection inverseTransform(const Point & data,
      const Indices & dimensions) const;

  /** IFFT ND transformation on complex data stored as a flat array */
  virtual ComplexCollection inverseTransform(const ComplexCollection & data,
      const Indices & dimensions) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

} ; /* class FFTImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FFTIMPLEMENTATION_HXX */
