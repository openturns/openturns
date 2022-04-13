//                                               -*- C++ -*-
/**
 * @brief PythonEvaluation implementation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PythonEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonEvaluation)

static const Factory<PythonEvaluation> Factory_PythonEvaluation;



/* Default constructor */
PythonEvaluation::PythonEvaluation()
  : EvaluationImplementation()
  , pyObj_(NULL)
  , pyObj_has_exec_(false)
  , pyObj_has_exec_sample_(false)
  , pyObj_discard_openturns_memoryview_(true)
  , pyBufferClass_(NULL)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonEvaluation::PythonEvaluation(PyObject * pyCallable)
  : EvaluationImplementation()
  , pyObj_(pyCallable)
  , pyObj_has_exec_(false)
  , pyObj_has_exec_sample_(false)
  , pyObj_discard_openturns_memoryview_(true)
  , pyBufferClass_(NULL)
{
  Py_XINCREF(pyCallable);

  initializePythonState();

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString (pyObj_,
                            const_cast<char *>("__class__")));
  ScopedPyObjectPointer name(PyObject_GetAttrString(cls.get(),
                             const_cast<char *>("__name__" )));
  setName(convert< _PyString_, String >(name.get()));

  const UnsignedInteger inputDimension  = getInputDimension();
  const UnsignedInteger outputDimension = getOutputDimension();
  Description description(inputDimension + outputDimension);

  ScopedPyObjectPointer descIn(PyObject_CallMethod(pyObj_,
                               const_cast<char *>("getInputDescription"),
                               const_cast<char *>("()")));
  if (descIn.get()
      && PySequence_Check(descIn.get())
      && (PySequence_Size(descIn.get()) == (SignedInteger)inputDimension))
  {
    Description inputDescription(convert< _PySequence_, Description >(descIn.get()));
    for (UnsignedInteger i = 0; i < inputDimension; ++ i)
    {
      description[i] = inputDescription[i];
    }
  }
  else for (UnsignedInteger i = 0; i < inputDimension; ++ i) description[i] = (OSS() << "x" << i);


  ScopedPyObjectPointer descOut(PyObject_CallMethod(pyObj_,
                                const_cast<char *>("getOutputDescription"),
                                const_cast<char *>("()")));
  if (descOut.get()
      && PySequence_Check(descOut.get())
      && (PySequence_Size(descOut.get()) == (SignedInteger)outputDimension))
  {
    Description outputDescription(convert< _PySequence_, Description >(descOut.get()));
    for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    {
      description[inputDimension + i] = outputDescription[i];
    }
  }
  else for (UnsignedInteger i = 0; i < outputDimension; ++ i) description[inputDimension + i] = (OSS() << "y" << i);

  setDescription(description);
}

/* Virtual constructor */
PythonEvaluation * PythonEvaluation::clone() const
{
  return new PythonEvaluation(*this);
}

/* Copy constructor */
PythonEvaluation::PythonEvaluation(const PythonEvaluation & other)
  : EvaluationImplementation(other)
  , pyObj_()
  , pyObj_has_exec_(other.pyObj_has_exec_)
  , pyObj_has_exec_sample_(other.pyObj_has_exec_sample_)
  , pyObj_discard_openturns_memoryview_(other.pyObj_discard_openturns_memoryview_)
  , pyBufferClass_()
{
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  pyObj_ = pyObjClone.get();

  ScopedPyObjectPointer pyBufferClone(deepCopy(other.pyBufferClass_));
  pyBufferClass_ = pyBufferClone.get();
  Py_XINCREF(pyObj_);
  Py_XINCREF(pyBufferClass_);
}


/* Copy assignment operator */
PythonEvaluation& PythonEvaluation::operator=(const PythonEvaluation & rhs)
{
  if (this != &rhs)
  {
    EvaluationImplementation::operator=(rhs);
    ScopedPyObjectPointer pyObjClone(deepCopy(rhs.pyObj_));
    pyObj_ = pyObjClone.get();

    pyObj_has_exec_ = rhs.pyObj_has_exec_;
    pyObj_has_exec_sample_ = rhs.pyObj_has_exec_sample_;
    pyObj_discard_openturns_memoryview_ = rhs.pyObj_discard_openturns_memoryview_;

    ScopedPyObjectPointer pyBufferClone(deepCopy(rhs.pyBufferClass_));
    pyBufferClass_ = pyBufferClone.get();
    Py_XINCREF(pyObj_);
    Py_XINCREF(pyBufferClass_);
  }
  return *this;
}

/* Destructor */
PythonEvaluation::~PythonEvaluation()
{
  Py_XDECREF(pyObj_);
  Py_XDECREF(pyBufferClass_);
}

/* Comparison operator */
Bool PythonEvaluation::operator ==(const PythonEvaluation & ) const
{
  return true;
}

/* String converter */
String PythonEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonEvaluation::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " parameter=" << getParameter();
  return oss;
}

/* String converter */
String PythonEvaluation::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << PythonEvaluation::GetClassName()
      << " name=" << getName();
  return oss;
}


/* Here is the interface that all derived class must implement */

/* Operator () */
Point PythonEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger dimension = inP.getDimension();

  if (dimension != getInputDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getInputDimension();

  Point outP;
  callsNumber_.increment();

  ScopedPyObjectPointer result;

  if (pyObj_discard_openturns_memoryview_)
  {
    // Force a memory copy of inP into a Python list
    ScopedPyObjectPointer point(convert< Point, _PySequence_ >(inP));
    ScopedPyObjectPointer execName(convert< String, _PyString_ >("_exec"));
    result = PyObject_CallMethodObjArgs(pyObj_, execName.get(), point.get(), NULL);
    if (! result.get())
      PyErr_SetString(PyExc_RuntimeError, "_exec did not return any value");
    else if (! PySequence_Check(result.get()))
      PyErr_SetString(PyExc_TypeError, "_exec return value is not a sequence");
  }
  else
  {
    // Wrap inP into a memoryview.Buffer object:
    //    openturns.memoryview.Buffer((int(&inP[0]), False), (inP.getSize(),))
    // First argument
    ScopedPyObjectPointer ptrTuple(PyTuple_New(2));
    PyTuple_SetItem(ptrTuple.get(), 0, PyLong_FromVoidPtr(static_cast<void *>(const_cast<double*>(inP.data()))));
    PyTuple_SetItem(ptrTuple.get(), 1, PyBool_FromLong(0));  // We do not own memory

    // Second argument
    ScopedPyObjectPointer shapeTuple(PyTuple_New(1));
    PyTuple_SetItem(shapeTuple.get(), 0, convert< UnsignedInteger, _PyInt_ > (inP.getSize()));

    // Call openturns.memoryview.Buffer() to create a read-only buffer
    ScopedPyObjectPointer bufferArgs(Py_BuildValue("OO", ptrTuple.get(), shapeTuple.get()));
    ScopedPyObjectPointer readOnlyBufferObj(PyObject_CallObject(pyBufferClass_, bufferArgs.get()));

    // Pass this buffer to _exec function if it has been defined by user, otherwise call _exec_sample(Buffer.augment())[0]
    // If both pyObj_has_exec_ and pyObj_has_exec_sample_ are false, this is not a PythonFunction but a Function(OpenTURNSPythonFunction).
    if (pyObj_has_exec_ || ! pyObj_has_exec_sample_)
    {
      ScopedPyObjectPointer execName(convert< String, _PyString_ >("_exec"));
      result = PyObject_CallMethodObjArgs(pyObj_, execName.get(), readOnlyBufferObj.get(), NULL);
    }
    else
    {
      // Only _exec_sample is defined, not _exec
      ScopedPyObjectPointer augmentName(convert< String, _PyString_ >("augment"));
      ScopedPyObjectPointer sampleObj(PyObject_CallMethodObjArgs(readOnlyBufferObj.get(), augmentName.get(), NULL));
      if (sampleObj.get())
      {
        ScopedPyObjectPointer execSampleName(convert< String, _PyString_ >("_exec_sample"));
        ScopedPyObjectPointer sampleResult(PyObject_CallMethodObjArgs(pyObj_, execSampleName.get(), sampleObj.get(), NULL));
        if (sampleResult.get())
        {
          if (PySequence_Check(sampleResult.get()))
            result = PySequence_ITEM(sampleResult.get(), 0);
          else
            PyErr_SetString(PyExc_TypeError, "_exec_sample return value is not a sequence");
        }
        else
          PyErr_SetString(PyExc_RuntimeError, "_exec_sample did not return any value");
      }
      else
        PyErr_SetString(PyExc_RuntimeError, "openturns.memoryview.Buffer.augment did not return any value");
    }
  }

  if (result.isNull())
  {
    handleException();
  }

  try
  {
    outP = convert< _PySequence_, Point >(result.get());
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


/* Operator () */
Sample PythonEvaluation::operator() (const Sample & inS) const
{
  const UnsignedInteger inDim = inS.getDimension();

  if (inDim != getInputDimension())
    throw InvalidDimensionException(HERE) << "Sample has incorrect dimension. Got " << inDim << ". Expected " << getInputDimension();

  const UnsignedInteger size = inS.getSize();
  const UnsignedInteger outDim = getOutputDimension();

  Sample outS(0, outDim);
  if (size > 0)
  {
    callsNumber_.fetchAndAdd(size);

    ScopedPyObjectPointer result;

    if (pyObj_discard_openturns_memoryview_)
    {
      // Force a memory copy of inS into a Python list
      ScopedPyObjectPointer inTuple(PyTuple_New(size));
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        PyObject * eltTuple = PyTuple_New(inDim);
        for (UnsignedInteger j = 0; j < inDim; ++ j) PyTuple_SetItem(eltTuple, j, convert< Scalar, _PyFloat_ > (inS(i, j)));
        PyTuple_SetItem(inTuple.get(), i, eltTuple);
      }
      ScopedPyObjectPointer execSampleName(convert< String, _PyString_ >("_exec_sample"));
      result = PyObject_CallMethodObjArgs(pyObj_, execSampleName.get(), inTuple.get(), NULL);
    }
    else
    {
      // Wrap inS into a memoryview.Buffer object:
      //    openturns.memoryview.Buffer((int(inS.data()), False), (inS.getSize(), inS.getDimension()))
      // First argument
      ScopedPyObjectPointer ptrTuple(PyTuple_New(2));
      PyTuple_SetItem(ptrTuple.get(), 0, PyLong_FromVoidPtr(static_cast<void *>(const_cast<Scalar*>(inS.data()))));
      PyTuple_SetItem(ptrTuple.get(), 1, PyBool_FromLong(0));  // We do not own memory

      // Second argument
      ScopedPyObjectPointer shapeTuple(PyTuple_New(2));
      PyTuple_SetItem(shapeTuple.get(), 0, convert< UnsignedInteger, _PyInt_ > (size));
      PyTuple_SetItem(shapeTuple.get(), 1, convert< UnsignedInteger, _PyInt_ > (inDim));

      // Call openturns.memoryview.Buffer() to create a read-only buffer
      ScopedPyObjectPointer bufferArgs(Py_BuildValue("OO", ptrTuple.get(), shapeTuple.get()));
      ScopedPyObjectPointer readOnlyBufferObj(PyObject_CallObject(pyBufferClass_, bufferArgs.get()));

      // Pass this buffer to _exec_sample function if it has been defined by user, otherwise loop on Buffer on call _exec
      // If both pyObj_has_exec_ and pyObj_has_exec_sample_ are false, this is not a PythonFunction but a Function(OpenTURNSPythonFunction).
      if (pyObj_has_exec_sample_ || ! pyObj_has_exec_)
      {
        ScopedPyObjectPointer execSampleName(convert< String, _PyString_ >("_exec_sample"));
        result = PyObject_CallMethodObjArgs(pyObj_, execSampleName.get(), readOnlyBufferObj.get(), NULL);
      }
      else
      {
        // Only _exec is defined, not _exec_sample
        ScopedPyObjectPointer execName(convert< String, _PyString_ >("_exec"));
        result = PyTuple_New(size);
        if (execName.get() && result.get())
        {
          for(UnsignedInteger i = 0; i < size; ++i)
          {
            ScopedPyObjectPointer itemObj(Py_TYPE(readOnlyBufferObj.get())->tp_as_sequence->sq_item(readOnlyBufferObj.get(), i));
            PyObject * pointI = PyObject_CallMethodObjArgs(pyObj_, execName.get(), itemObj.get(), NULL);
            if (!pointI)
            {
              handleException();
            }
            PyTuple_SetItem(result.get(), i, pointI);
          }
        }
      }
    }

    if (result.isNull())
    {
      handleException();
    }

    try
    {
      outS = convert< _PySequence_, Sample >(result.get());
    }
    catch (InvalidArgumentException &)
    {
      throw InvalidArgumentException(HERE) << "Output value for " << getName() << "._exec_sample() method is not a 2d-sequence object";
    }
    if (outS.getSize() != size)
      throw InvalidArgumentException(HERE) << "Python Function returned a sequence object with incorrect size (got "
                                           << outS.getSize() << ", expected " << size << ")";
    if (outS.getDimension() != outDim)
      throw InvalidArgumentException(HERE) << "Python Function returned a sequence object with incorrect dimension (got "
                                           << outS.getDimension() << ", expected " << outDim << ")";
  }
  outS.setDescription(getOutputDescription());
  return outS;
}

/* Accessor for input point dimension */
void PythonEvaluation::initializePythonState()
{
  // Check whether PythonFunction object define these members
  pyObj_has_exec_ = (PyObject_HasAttrString(pyObj_, "_has_exec") > 0);
  pyObj_has_exec_sample_ = (PyObject_HasAttrString(pyObj_, "_has_exec_sample") > 0);
  pyObj_discard_openturns_memoryview_ = (PyObject_HasAttrString(pyObj_, "_discard_openturns_memoryview") > 0);

  // We do not copy, get a reference to openturns.memoryview.Buffer class
  if (! pyObj_discard_openturns_memoryview_)
  {
    ScopedPyObjectPointer memoryWrapperModule(PyImport_ImportModule("openturns.memoryview"));
    if (memoryWrapperModule.isNull())
    {
      handleException();
    }
    pyBufferClass_ = PyObject_GetAttrString(memoryWrapperModule.get(), const_cast<char *>("Buffer"));
    if (pyBufferClass_ == NULL)
    {
      handleException();
    }
    Py_INCREF(pyBufferClass_);
  }
}


/* Accessor for input point dimension */
UnsignedInteger PythonEvaluation::getInputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getInputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}


/* Accessor for output point dimension */
UnsignedInteger PythonEvaluation::getOutputDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod (pyObj_,
                               const_cast<char *>("getOutputDimension"),
                               const_cast<char *>("()")));
  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >(result.get());
  return dim;
}

/* Linearity accessors */
Bool PythonEvaluation::isLinear() const
{
  if (PyObject_HasAttrString(pyObj_, "isLinear"))
  {
    ScopedPyObjectPointer result( PyObject_CallMethod (pyObj_,
                                  const_cast<char *>("isLinear"),
                                  const_cast<char *>("()")));

    const Bool isLinear = convert< _PyBool_, Bool >(result.get());
    return isLinear;
  }
  else
    return false;
}

Bool PythonEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  // Check index consistency
  if (index > getInputDimension())
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function input dimension (" << getInputDimension() << ")";

  if (PyObject_HasAttrString(pyObj_, "isVariableLinear"))
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_ >("isVariableLinear"));
    ScopedPyObjectPointer indexArg(convert< UnsignedInteger, _PyInt_ >( index ));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs( pyObj_,
                                     methodName.get(),
                                     indexArg.get(), NULL));

    const Bool varLinear = convert< _PyBool_, Bool >(callResult.get());
    return varLinear;
  }
  else
    return false;
}

/* Is it safe to call in parallel? */
Bool PythonEvaluation::isParallel() const
{
  return false;
}

/* Method save() stores the object through the StorageManager */
void PythonEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  pickleSave(adv, pyObj_);
  pickleSave(adv, pyBufferClass_, "pyBufferClass_");
  adv.saveAttribute("pyObj_has_exec_", pyObj_has_exec_);
  adv.saveAttribute("pyObj_has_exec_sample_", pyObj_has_exec_sample_);
  adv.saveAttribute("pyObj_discard_openturns_memoryview_", pyObj_discard_openturns_memoryview_);
}


/* Method save() reloads the object from the StorageManager */
void PythonEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  pickleLoad(adv, pyObj_);
  pickleLoad(adv, pyBufferClass_, "pyBufferClass_");
  adv.loadAttribute("pyObj_has_exec_", pyObj_has_exec_);
  adv.loadAttribute("pyObj_has_exec_sample_", pyObj_has_exec_sample_);
  adv.loadAttribute("pyObj_discard_openturns_memoryview_", pyObj_discard_openturns_memoryview_);
}


END_NAMESPACE_OPENTURNS
