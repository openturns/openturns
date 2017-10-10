//                                               -*- C++ -*-
/**
 * @brief PythonPointToFieldFunction implementation
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <Python.h>
#include "openturns/swig_runtime.hxx"

#include "openturns/PythonPointToFieldFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonPointToFieldFunction)

static const Factory<PythonPointToFieldFunction> Factory_PythonPointToFieldFunction;

/* Default constructor */
PythonPointToFieldFunction::PythonPointToFieldFunction()
  : PointToFieldFunctionImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonPointToFieldFunction::PythonPointToFieldFunction(PyObject * pyCallable)
  : PointToFieldFunctionImplementation()
  , pyObj_(pyCallable)
{
  Py_XINCREF(pyCallable);

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString ( pyObj_,
                            const_cast<char *>("__class__" )));
  ScopedPyObjectPointer name(PyObject_GetAttrString( cls.get(),
                             const_cast<char *>("__name__" )));
  setName(convert< _PyString_, String >(name.get()));



  const UnsignedInteger inputDimension  = getInputDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
  Description description(inputDimension + outputDimension);

  ScopedPyObjectPointer descIn(PyObject_CallMethod( pyObj_,
                               const_cast<char *>("getInputDescription"),
                               const_cast<char *>("()")));
  if ( descIn.get()
       && PySequence_Check(descIn.get())
       && (PySequence_Size(descIn.get()) == (SignedInteger)inputDimension))
  {
    setInputDescription(convert< _PySequence_, Description >(descIn.get()));
  }
  else setInputDescription(Description::BuildDefault(inputDimension, "x"));


  ScopedPyObjectPointer descOut(PyObject_CallMethod( pyObj_,
                                const_cast<char *>("getOutputDescription" ),
                                const_cast<char *>("()")));
  if ( descOut.get()
       && PySequence_Check(descOut.get())
       && (PySequence_Size(descOut.get()) == (SignedInteger)outputDimension))
  {
    setOutputDescription(convert< _PySequence_, Description >(descOut.get()));
  }
  else setOutputDescription(Description::BuildDefault(outputDimension, "y"));
}

/* Virtual constructor */
PythonPointToFieldFunction * PythonPointToFieldFunction::clone() const
{
  return new PythonPointToFieldFunction(*this);
}

/* Copy constructor */
PythonPointToFieldFunction::PythonPointToFieldFunction(const PythonPointToFieldFunction & other)
  : PointToFieldFunctionImplementation(other)
  , pyObj_(other.pyObj_)
{
  Py_XINCREF(pyObj_);
}

/* Destructor */
PythonPointToFieldFunction::~PythonPointToFieldFunction()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonPointToFieldFunction::operator ==(const PythonPointToFieldFunction & other) const
{
  return true;
}

/* String converter */
String PythonPointToFieldFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonPointToFieldFunction::GetClassName()
      << " name=" << getName()
      << " input description=" << getInputDescription()
      << " output description=" << getOutputDescription();
  return oss;
}

/* String converter */
String PythonPointToFieldFunction::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << PythonPointToFieldFunction::GetClassName()
      << " name=" << getName();
  return oss;
}



/* Here is the interface that all derived class must implement */

/* Operator () */
Field PythonPointToFieldFunction::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inputDimension != inP.getDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << inP.getDimension() << ". Expected " << getInputDimension();

  ++ callsNumber_;

  ScopedPyObjectPointer pyInP(SWIG_NewPointerObj(new OT::Point(inP), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN | 0));
  ScopedPyObjectPointer pyOutField(PyObject_CallFunctionObjArgs( pyObj_, pyInP.get(), NULL));
  if (pyOutField.isNull())
  {
    handleException();
  }
  void * ptr = 0;
  OT::Field * p_outF = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyOutField.get(), &ptr, SWIG_TypeQuery("OT::Field *"), 0)))
  {
    p_outF = reinterpret_cast< OT::Field * >(ptr);
  }
  else
  {
    throw InvalidArgumentException(HERE) << "Output value for " << getName() << "._exec() method is not a Field";
  }

  if (p_outF->getDimension() != getOutputDimension())
  {
    throw InvalidDimensionException(HERE) << "Output field has incorrect dimension. Got " << p_outF->getDimension() << ". Expected " << getOutputDimension();
  }
  return *p_outF;
}

/* Accessor for input point dimension */
UnsignedInteger PythonPointToFieldFunction::getInputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonPointToFieldFunction::getOutputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Method save() stores the object through the StorageManager */
void PythonPointToFieldFunction::save(Advocate & adv) const
{
  PointToFieldFunctionImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonPointToFieldFunction::load(Advocate & adv)
{
  PointToFieldFunctionImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
