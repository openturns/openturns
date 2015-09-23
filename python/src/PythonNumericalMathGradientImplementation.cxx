//                                               -*- C++ -*-
/**
 * @brief PythonNumericalMathGradientImplementation implementation
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

#include "PythonNumericalMathGradientImplementation.hxx"
#include "OSS.hxx"
#include "Description.hxx"
#include "PythonWrappingFunctions.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonNumericalMathGradientImplementation);

static const Factory<PythonNumericalMathGradientImplementation> RegisteredFactory;



/* Default constructor */
PythonNumericalMathGradientImplementation::PythonNumericalMathGradientImplementation()
  : NumericalMathGradientImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonNumericalMathGradientImplementation::PythonNumericalMathGradientImplementation(PyObject * pyCallable)
  : NumericalMathGradientImplementation()
  , pyObj_(pyCallable)
{
  Py_XINCREF(pyCallable);

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString (pyObj_,
                            const_cast<char *>("__class__")));
  ScopedPyObjectPointer name(PyObject_GetAttrString(cls.get(),
                             const_cast<char *>("__name__")));
  setName(convert< _PyString_, String >(name.get()));
}

/* Virtual constructor */
PythonNumericalMathGradientImplementation * PythonNumericalMathGradientImplementation::clone() const
{
  return new PythonNumericalMathGradientImplementation(*this);
}

/* Copy constructor */
PythonNumericalMathGradientImplementation::PythonNumericalMathGradientImplementation(const PythonNumericalMathGradientImplementation & other)
  : NumericalMathGradientImplementation(other)
  , pyObj_(other.pyObj_)
{
  Py_XINCREF(pyObj_);
}

/* Destructor */
PythonNumericalMathGradientImplementation::~PythonNumericalMathGradientImplementation()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonNumericalMathGradientImplementation::operator ==(const PythonNumericalMathGradientImplementation & other) const
{
  return true;
}

/* String converter */
String PythonNumericalMathGradientImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonNumericalMathGradientImplementation::GetClassName()
      << " name=" << getName()
      << " parameters=" << getParameters();
  return oss;
}

/* String converter */
String PythonNumericalMathGradientImplementation::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << PythonNumericalMathGradientImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool PythonNumericalMathGradientImplementation::isActualImplementation() const
{
  return true;
}



/* Here is the interface that all derived class must implement */

/* Operator () */
Matrix PythonNumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension = inP.getDimension();

  if (dimension != getInputDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getInputDimension();

  ScopedPyObjectPointer point(convert< NumericalPoint, _PySequence_ >(inP));
  ScopedPyObjectPointer methodName(convert< String, _PyString_ >("_gradient" ));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_,
                                   methodName.get(),
                                   point.get(), NULL ));

  if (callResult.isNull())
  {
    handleException();
  }
  Matrix result;
  try
  {
    result = convert< _PySequence_, Matrix >(callResult.get());
  }
  catch (const InvalidArgumentException & ex)
  {
    throw InvalidArgumentException(HERE) << "Output value for " << getName() << "._gradient() method is not a 2-d float sequence object (list, tuple, etc.)";
  }
  if (result.getNbRows() != getInputDimension())
    throw InvalidDimensionException(HERE) << "Gradient has incorrect row count. Got " << result.getNbRows() << ". Expected " << getInputDimension();

  if (result.getNbColumns() != getOutputDimension())
    throw InvalidDimensionException(HERE) << "Gradient has incorrect column count. Got " << result.getNbColumns() << ". Expected " << getOutputDimension();

  return result;
}


/* Accessor for input point dimension */
UnsignedInteger PythonNumericalMathGradientImplementation::getInputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonNumericalMathGradientImplementation::getOutputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Method save() stores the object through the StorageManager */
void PythonNumericalMathGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonNumericalMathGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
