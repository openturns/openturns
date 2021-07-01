//                                               -*- C++ -*-
/**
 * @brief PythonFieldFunction implementation
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
#include <Python.h>
#include "openturns/swig_runtime.hxx"

#include "openturns/PythonFieldFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonFieldFunction)

static const Factory<PythonFieldFunction> Factory_PythonFieldFunction;

/* Default constructor */
PythonFieldFunction::PythonFieldFunction()
  : FieldFunctionImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonFieldFunction::PythonFieldFunction(PyObject * pyCallable)
  : FieldFunctionImplementation()
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
                                const_cast<char *>("getOutputDescription"),
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

  ScopedPyObjectPointer outputMesh(PyObject_CallMethod ( pyObj_,
                                   const_cast<char *>("getOutputMesh"),
                                   const_cast<char *>("()")));
  if (SWIG_IsOK(SWIG_ConvertPtr(outputMesh.get(), &ptr, SWIG_TypeQuery("OT::Mesh *"), 0)))
  {
    outputMesh_ = *reinterpret_cast< OT::Mesh * >(ptr);
  }
  else
  {
    throw InvalidArgumentException(HERE) << "getOutputMesh() does not return a Mesh";
  }
}

/* Virtual constructor */
PythonFieldFunction * PythonFieldFunction::clone() const
{
  return new PythonFieldFunction(*this);
}

/* Copy constructor */
PythonFieldFunction::PythonFieldFunction(const PythonFieldFunction & other)
  : FieldFunctionImplementation(other)
  , pyObj_()
{
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  pyObj_ = pyObjClone.get();
  Py_XINCREF(pyObj_);
}

/* Copy assignment operator */
PythonFieldFunction & PythonFieldFunction::operator=(const PythonFieldFunction & rhs)
{
  if (this != &rhs)
  {
    FieldFunctionImplementation::operator=(rhs);
    ScopedPyObjectPointer pyObjClone(deepCopy(rhs.pyObj_));
    pyObj_ = pyObjClone.get();
    Py_XINCREF(pyObj_);
  }
  return *this;
}

/* Destructor */
PythonFieldFunction::~PythonFieldFunction()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonFieldFunction::operator ==(const PythonFieldFunction & ) const
{
  return true;
}

/* String converter */
String PythonFieldFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonFieldFunction::GetClassName()
      << " name=" << getName()
      << " input description=" << getInputDescription()
      << " output description=" << getOutputDescription();
  return oss;
}

/* String converter */
String PythonFieldFunction::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << PythonFieldFunction::GetClassName()
      << " name=" << getName();
  return oss;
}



/* Here is the interface that all derived class must implement */

/* Operator () */
Sample PythonFieldFunction::operator() (const Sample & inF) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inputDimension != inF.getDimension())
    throw InvalidDimensionException(HERE) << "Input field values have incorrect dimension. Got " << inF.getDimension() << ". Expected " << inputDimension;

  const UnsignedInteger inputSize = getInputMesh().getVerticesNumber();
  if (inF.getSize() != inputSize)
    throw InvalidDimensionException(HERE) << "Input field values have incorrect size. Got " << inF.getSize() << ". Expected " << inputSize;

  callsNumber_.increment();

  // Force a memory copy of inS into a Python list
  ScopedPyObjectPointer inTuple(PyTuple_New(inputSize));
  for (UnsignedInteger i = 0; i < inputSize; ++ i)
  {
    PyObject * eltTuple = PyTuple_New(inputDimension);
    for (UnsignedInteger j = 0; j < inputDimension; ++ j) PyTuple_SetItem(eltTuple, j, convert< Scalar, _PyFloat_ > (inF(i, j)));
    PyTuple_SetItem(inTuple.get(), i, eltTuple);
  }

  ScopedPyObjectPointer execSampleName(convert< String, _PyString_ >("_exec"));
  ScopedPyObjectPointer result(PyObject_CallMethodObjArgs(pyObj_, execSampleName.get(), inTuple.get(), NULL));
  if (result.isNull())
  {
    handleException();
  }

  Sample outF;
  try
  {
    outF = convert< _PySequence_, Sample >(result.get());
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Output value for " << getName() << "._exec_sample() method is not a 2d-sequence object";
  }

  const UnsignedInteger outputSize = getOutputMesh().getVerticesNumber();
  if (outF.getSize() != outputSize)
    throw InvalidArgumentException(HERE) << "Python Field function returned a sequence object with incorrect size (got "
                                         << outF.getSize() << ", expected " << outputSize << ")";

  const UnsignedInteger outputDimension = getOutputDimension();
  if (outF.getDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "Python Field function returned a sequence object with incorrect dimension (got "
                                         << outF.getDimension() << ", expected " << outputDimension << ")";
  outF.setDescription(getOutputDescription());
  return outF;
}


/* Accessor for input point dimension */
UnsignedInteger PythonFieldFunction::getInputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonFieldFunction::getOutputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
Bool PythonFieldFunction::isActingPointwise() const
{
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("isActingPointwise")))
  {
    ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                                 const_cast<char *>("isActingPointwise"),
                                 const_cast<char *>("()")));
    return convert< _PyBool_, Bool >(result.get());
  }
  else
    return FieldFunctionImplementation::isActingPointwise();
}

/* Method save() stores the object through the StorageManager */
void PythonFieldFunction::save(Advocate & adv) const
{
  FieldFunctionImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonFieldFunction::load(Advocate & adv)
{
  FieldFunctionImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
