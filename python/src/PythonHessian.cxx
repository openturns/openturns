//                                               -*- C++ -*-
/**
 * @brief PythonHessian implementation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PythonHessian.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonHessian)

static const Factory<PythonHessian> Factory_PythonHessian;



/* Default constructor */
PythonHessian::PythonHessian()
  : HessianImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonHessian::PythonHessian(PyObject * pyCallable)
  : HessianImplementation()
  , pyObj_(pyCallable)
{
  InterpreterUnlocker iul;
  Py_XINCREF(pyCallable);

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString (pyObj_,
                            const_cast<char *>("__class__")));
  ScopedPyObjectPointer name(PyObject_GetAttrString(cls.get(),
                             const_cast<char *>("__name__")));
  setName(convert< _PyString_, String >(name.get()));
}

/* Virtual constructor */
PythonHessian * PythonHessian::clone() const
{
  return new PythonHessian(*this);
}

/* Copy constructor */
PythonHessian::PythonHessian(const PythonHessian & other)
  : HessianImplementation(other)
  , pyObj_()
{
  InterpreterUnlocker iul;
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  pyObj_ = pyObjClone.get();
  Py_XINCREF(pyObj_);
}

/* Copy assignment operator */
PythonHessian & PythonHessian::operator=(const PythonHessian & rhs)
{
  if (this != &rhs)
  {
    InterpreterUnlocker iul;
    HessianImplementation::operator=(rhs);
    ScopedPyObjectPointer pyObjClone(deepCopy(rhs.pyObj_));
    pyObj_ = pyObjClone.get();
    Py_XINCREF(pyObj_);
  }
  return *this;
}

/* Destructor */
PythonHessian::~PythonHessian()
{
  InterpreterUnlocker iul;
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonHessian::operator ==(const PythonHessian & ) const
{
  return true;
}

/* String converter */
String PythonHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonHessian::GetClassName()
      << " name=" << getName()
      << " parameter=" << getParameter();
  return oss;
}

/* String converter */
String PythonHessian::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << PythonHessian::GetClassName()
      << " name=" << getName();
  return oss;
}


/* Here is the interface that all derived class must implement */

/* Operator () */
SymmetricTensor PythonHessian::hessian(const Point & inP) const
{
  InterpreterUnlocker iul;
  const UnsignedInteger dimension = inP.getDimension();

  if (dimension != getInputDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getInputDimension();

  ScopedPyObjectPointer point(convert< Point, _PySequence_ >(inP));
  ScopedPyObjectPointer methodName(convert< String, _PyString_ >("_hessian"));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_,
                                   methodName.get(),
                                   point.get(), NULL));

  if (callResult.isNull())
  {
    handleException();
  }
  SymmetricTensor result;
  try
  {
    result = convert< _PySequence_, Tensor >(callResult.get()).getImplementation();
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Output value for " << getName() << "._hessian() method is not a 3-d float sequence object (list, tuple, etc.)";
  }

  if (result.getNbRows() != getInputDimension())
    throw InvalidDimensionException(HERE) << "Hessian has incorrect row count. Got " << result.getNbRows() << ". Expected " << getInputDimension();

  if (result.getNbColumns() != getInputDimension())
    throw InvalidDimensionException(HERE) << "Hessian has incorrect column count. Got " << result.getNbColumns() << ". Expected " << getInputDimension();

  if (result.getNbSheets() != getOutputDimension())
    throw InvalidDimensionException(HERE) << "Hessian has incorrect sheet count. Got " << result.getNbSheets() << ". Expected " << getOutputDimension();

  return result;
}


/* Accessor for input point dimension */
UnsignedInteger PythonHessian::getInputDimension() const
{
  InterpreterUnlocker iul;
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonHessian::getOutputDimension() const
{
  InterpreterUnlocker iul;
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Method save() stores the object through the StorageManager */
void PythonHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  InterpreterUnlocker iul;
  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  InterpreterUnlocker iul;
  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
