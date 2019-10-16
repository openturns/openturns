//                                               -*- C++ -*-
/**
 * @brief PythonGradient implementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/PythonGradient.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonGradient)

static const Factory<PythonGradient> Factory_PythonGradient;



/* Default constructor */
PythonGradient::PythonGradient()
  : GradientImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonGradient::PythonGradient(PyObject * pyCallable)
  : GradientImplementation()
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
PythonGradient * PythonGradient::clone() const
{
  return new PythonGradient(*this);
}

/* Copy constructor */
PythonGradient::PythonGradient(const PythonGradient & other)
  : GradientImplementation(other)
  , pyObj_()
{
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  pyObj_ = pyObjClone.get();
  Py_XINCREF(pyObj_);
}

/* Copy assignment operator */
PythonGradient & PythonGradient::operator=(const PythonGradient & rhs)
{
  if (this != &rhs)
  {
    ScopedPyObjectPointer pyObjClone(deepCopy(rhs.pyObj_));
    pyObj_ = pyObjClone.get();
    Py_XINCREF(pyObj_);
  }
  return *this;
}

/* Destructor */
PythonGradient::~PythonGradient()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonGradient::operator ==(const PythonGradient & ) const
{
  return true;
}

/* String converter */
String PythonGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonGradient::GetClassName()
      << " name=" << getName()
      << " parameter=" << getParameter();
  return oss;
}

/* String converter */
String PythonGradient::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << PythonGradient::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool PythonGradient::isActualImplementation() const
{
  return true;
}



/* Here is the interface that all derived class must implement */

/* Operator () */
Matrix PythonGradient::gradient(const Point & inP) const
{
  const UnsignedInteger dimension = inP.getDimension();

  if (dimension != getInputDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getInputDimension();

  ScopedPyObjectPointer point(convert< Point, _PySequence_ >(inP));
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
  catch (InvalidArgumentException &)
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
UnsignedInteger PythonGradient::getInputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonGradient::getOutputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Method save() stores the object through the StorageManager */
void PythonGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
