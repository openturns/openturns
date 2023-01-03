//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FFT.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KissFFT.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FFT)

/* Constructor with parameters */
FFT::FFT()
  : TypedInterfaceObject<FFTImplementation>(new KissFFT())
{
  // Nothing to do
}

/* Parameters constructor */
FFT::FFT(const FFTImplementation & implementation)
  : TypedInterfaceObject<FFTImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
FFT::FFT(const Implementation & p_implementation)
  : TypedInterfaceObject<FFTImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
FFT::FFT(FFTImplementation * p_implementation)
  : TypedInterfaceObject<FFTImplementation>(p_implementation)
{
  // Nothing to do
}


/* FFT transformation on real */
FFT::ComplexCollection FFT::transform(const ScalarCollection & collection) const
{
  return getImplementation()->transform(collection);
}

/** FFT transformation on real - The transformation is applied on a part of the collection*/
FFT::ComplexCollection FFT::transform(const ScalarCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger size) const
{
  return getImplementation()->transform(collection, first, size);
}

/** FFT transformation on real with a regular sequence of the collection (between first and last, by step = step)*/
FFT::ComplexCollection FFT::transform(const ScalarCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger step,
                                      const UnsignedInteger last) const
{
  return getImplementation()->transform(collection, first, step, last);
}

/* FFT transformation on complex */
FFT::ComplexCollection FFT::transform(const ComplexCollection & collection) const
{
  return getImplementation()->transform(collection);
}

/* FFT transformation on complex. The transformation is applied on a part of the collection */
FFT::ComplexCollection FFT::transform(const ComplexCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger size) const
{
  return getImplementation()->transform(collection, first, size);
}


/* FFT transformation on complex. This transforms a regular sequence of the collection (between first and last, with elements separated by step)*/
FFT::ComplexCollection FFT::transform(const ComplexCollection & collection,
                                      const UnsignedInteger first,
                                      const UnsignedInteger step,
                                      const UnsignedInteger last) const
{
  return getImplementation()->transform(collection, first, step, last);
}


/** FFT 2D transformation on complex */
ComplexMatrix FFT::transform2D(const ComplexMatrix & matrix) const
{
  return getImplementation()->transform2D(matrix);
}


/** FFT 2D transformation on complex */
ComplexMatrix FFT::transform2D(const Matrix & matrix) const
{
  return getImplementation()->transform2D(matrix);
}

/** FFT 2D transformation on complex */
ComplexMatrix FFT::transform2D(const Sample & sample) const
{
  return getImplementation()->transform2D(sample);
}

/** FFT 3D transformation on complex */
ComplexTensor FFT::transform3D(const ComplexTensor & tensor) const
{
  return getImplementation()->transform3D(tensor);
}

/* FFT 3D transformation on real data */
ComplexTensor FFT::transform3D(const Tensor & tensor) const
{
  return getImplementation()->transform3D(tensor);
}

FFT::ComplexCollection FFT::inverseTransform(const ScalarCollection & collection) const
{
  return getImplementation()->inverseTransform(collection);
}

/** FFT inverse transformation - The transformation is applied on a part of the collection */
FFT::ComplexCollection FFT::inverseTransform(const ScalarCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  return getImplementation()->inverseTransform(collection, first, size);
}

/** FFT inverse transformation on a regular sequence of the collection (between first and last, separated by step)*/
FFT::ComplexCollection FFT::inverseTransform(const ScalarCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger step,
    const UnsignedInteger last) const
{
  return getImplementation()->inverseTransform(collection, first, step, last);
}

/* FFT inverse transformation */
FFT::ComplexCollection FFT::inverseTransform(const ComplexCollection & collection) const
{
  return getImplementation()->inverseTransform(collection);
}

/* FFT transformation on complex. The transformation is applied on a part of the collection */
FFT::ComplexCollection FFT::inverseTransform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  return getImplementation()->inverseTransform(collection, first, size);
}


/* FFT transformation on complex. This transforms a regular sequence of the collection (between first and last, with elements separated by step)*/
FFT::ComplexCollection FFT::inverseTransform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger step,
    const UnsignedInteger last) const
{
  return getImplementation()->inverseTransform(collection, first, step, last);
}


/** IFFT 2D transformation on complex */
ComplexMatrix FFT::inverseTransform2D(const ComplexMatrix & matrix) const
{
  return getImplementation()->inverseTransform2D(matrix);
}

/** IFFT 2D transformation on real matrix */
ComplexMatrix FFT::inverseTransform2D(const Matrix & matrix) const
{
  return getImplementation()->inverseTransform2D(matrix);
}

/** IFFT 2D transformation on sample */
ComplexMatrix FFT::inverseTransform2D(const Sample & sample) const
{
  return getImplementation()->inverseTransform2D(sample);
}

/* IFFT 3D transformation */
ComplexTensor FFT::inverseTransform3D(const ComplexTensor & tensor) const
{
  return getImplementation()->inverseTransform3D(tensor);
}

/* IFFT 3D transformation on real tensors*/
ComplexTensor FFT::inverseTransform3D(const Tensor & tensor) const
{
  return getImplementation()->inverseTransform3D(tensor);
}

/* String converter */
String FFT::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FFT::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String FFT::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << FFT::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
