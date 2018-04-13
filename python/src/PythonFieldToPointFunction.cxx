//                                               -*- C++ -*-
/**
 * @brief PythonFieldToPointFunction implementation
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include <Python.h>
#include "openturns/swig_runtime.hxx"

#include "openturns/PythonFieldToPointFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonFieldToPointFunction)

static const Factory<PythonFieldToPointFunction> Factory_PythonFieldToPointFunction;

/* Default constructor */
PythonFieldToPointFunction::PythonFieldToPointFunction()
  : FieldToPointFunctionImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonFieldToPointFunction::PythonFieldToPointFunction(PyObject * pyCallable)
  : FieldToPointFunctionImplementation()
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

  ScopedPyObjectPointer inputMesh(PyObject_CallMethod ( pyObj_,
                                  const_cast<char *>("getInputMesh"),
                                  const_cast<char *>("()")));
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(inputMesh.get(), &ptr, SWIG_TypeQuery("OT::Mesh *"), 0)))
  {
    inputMesh_ = *reinterpret_cast< OT::Mesh * >(ptr);
  }
  else
  {
    throw InvalidArgumentException(HERE) << "getInputMesh() does not return a Mesh";
  }
}

/* Virtual constructor */
PythonFieldToPointFunction * PythonFieldToPointFunction::clone() const
{
  return new PythonFieldToPointFunction(*this);
}

/* Copy constructor */
PythonFieldToPointFunction::PythonFieldToPointFunction(const PythonFieldToPointFunction & other)
  : FieldToPointFunctionImplementation(other)
  , pyObj_(other.pyObj_)
{
  Py_XINCREF(pyObj_);
}

/* Destructor */
PythonFieldToPointFunction::~PythonFieldToPointFunction()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonFieldToPointFunction::operator ==(const PythonFieldToPointFunction & other) const
{
  return true;
}

/* String converter */
String PythonFieldToPointFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonFieldToPointFunction::GetClassName()
      << " name=" << getName()
      << " input description=" << getInputDescription()
      << " output description=" << getOutputDescription();
  return oss;
}

/* String converter */
String PythonFieldToPointFunction::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << PythonFieldToPointFunction::GetClassName()
      << " name=" << getName();
  return oss;
}



/* Here is the interface that all derived class must implement */

/* Operator () */
Point PythonFieldToPointFunction::operator() (const Field & inF) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inputDimension != inF.getOutputDimension())
    throw InvalidDimensionException(HERE) << "Input field has incorrect dimension. Got " << inF.getOutputDimension() << ". Expected " << getInputDimension();

  if (getInputMesh().getDimension() != inF.getInputDimension())
    throw InvalidDimensionException(HERE) << "Input field has incorrect spatial dimension. Got " << inF.getInputDimension() << ". Expected " << getInputMesh().getDimension();

  callsNumber_.increment();

  ScopedPyObjectPointer pyInField(SWIG_NewPointerObj(new OT::Field(inF), SWIG_TypeQuery("OT::Field *"), SWIG_POINTER_OWN | 0));
  ScopedPyObjectPointer pyOutP(PyObject_CallFunctionObjArgs( pyObj_, pyInField.get(), NULL));
  if (pyOutP.isNull())
  {
    handleException();
  }
  Point outP;
  try
  {
    outP = convert< _PySequence_, Point >(pyOutP.get());
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Output value for " << getName() << "._exec() method is not a sequence object (list, tuple, Point, etc.)";
  }
  if (outP.getDimension() != getOutputDimension())
  {
    throw InvalidDimensionException(HERE) << "Output point has incorrect dimension. Got " << outP.getDimension() << ". Expected " << getOutputDimension();
  }
  return outP;
}


/* Accessor for input point dimension */
UnsignedInteger PythonFieldToPointFunction::getInputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonFieldToPointFunction::getOutputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Method save() stores the object through the StorageManager */
void PythonFieldToPointFunction::save(Advocate & adv) const
{
  FieldToPointFunctionImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonFieldToPointFunction::load(Advocate & adv)
{
  FieldToPointFunctionImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
