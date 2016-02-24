//                                               -*- C++ -*-
/**
 *  @brief This class is enables to launch an FFT transform/inverse transform
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FFTIMPLEMENTATION_HXX
#define OPENTURNS_FFTIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/ComplexMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/ComplexTensor.hxx"
#include "openturns/Tensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FFTImplementation
 */

class OT_API FFTImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  typedef Collection<NumericalScalar>   NumericalScalarCollection;
  typedef Collection<NumericalComplex>  NumericalComplexCollection;

  /** Default constructor without parameters */
  FFTImplementation();

  /** Virtual copy constructor */
  virtual FFTImplementation * clone() const;

  /** FFT transformation on real */
  virtual NumericalComplexCollection transform(const NumericalScalarCollection & collection) const;

  /** FFT transformation on real - The transformation is applied on a part of the collection*/
  virtual NumericalComplexCollection transform(const NumericalScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT transformation on real with a regular sequence of the collection (between first and last, by step = step)*/
  virtual NumericalComplexCollection transform(const NumericalScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** FFT transformation on complex */
  virtual NumericalComplexCollection transform(const NumericalComplexCollection & collection) const;

  /** FFT transformation on complex - For some FFT implementation, the transformation is applied on a part of the collection */
  virtual NumericalComplexCollection transform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT transformation on complex - For some FFT implementation, the need is to transform a regular sequence of the collection (between first and last, by step = step)*/
  virtual NumericalComplexCollection transform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** FFT inverse transformation */
  virtual NumericalComplexCollection inverseTransform(const NumericalScalarCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  virtual NumericalComplexCollection inverseTransform(const NumericalScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT inverse transformation on a regular sequence of the collection (between first and last, spearated  by step)*/
  virtual NumericalComplexCollection inverseTransform(const NumericalScalarCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** FFT inverse transformation */
  virtual NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection) const;

  /** FFT inverse transformation - The transformation is applied on a part of the collection */
  virtual NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger size) const;

  /** FFT transformation on a regular sequence of the collection (between first and last, spearated  by step)*/
  virtual NumericalComplexCollection inverseTransform(const NumericalComplexCollection & collection,
      const UnsignedInteger first,
      const UnsignedInteger step,
      const UnsignedInteger last) const;

  /** FFT 2D transformation on complex */
  virtual ComplexMatrix transform2D(const ComplexMatrix & complexMatrix) const;

  /** FFT 2D transformation on real matrix */
  virtual ComplexMatrix transform2D(const Matrix & matrix) const;

  /** FFT 2D transformation on sample */
  virtual ComplexMatrix transform2D(const NumericalSample & sample) const;

  /** IFFT 2D transformation on complex */
  virtual ComplexMatrix inverseTransform2D(const ComplexMatrix & complexMatrix) const;

  /** IFFT 2D transformation on real matrix */
  virtual ComplexMatrix inverseTransform2D(const Matrix & matrix) const;

  /** IFFT 2D transformation on sample */
  virtual ComplexMatrix inverseTransform2D(const NumericalSample & sample) const;

  /** FFT 3D transformation */
  virtual ComplexTensor transform3D(const ComplexTensor & tensor) const;

  /** FFT 3D transformation on real data */
  virtual ComplexTensor transform3D(const Tensor & tensor) const;

  /** IFFT 3D transformation */
  virtual ComplexTensor inverseTransform3D(const ComplexTensor & tensor) const;

  /** IFFT 3D transformation */
  virtual ComplexTensor inverseTransform3D(const Tensor & tensor) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class FFTImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FFTIMPLEMENTATION_HXX */
