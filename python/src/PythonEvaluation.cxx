//                                               -*- C++ -*-
/**
 * @brief PythonEvaluation implementation
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
 *  You should have received a copy of the GNU Lesser General Public
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

typedef EvaluationImplementation::CacheKeyType             CacheKeyType;
typedef EvaluationImplementation::CacheValueType           CacheValueType;
typedef EvaluationImplementation::CacheType                CacheType;


CLASSNAMEINIT(PythonEvaluation)

static const Factory<PythonEvaluation> Factory_PythonEvaluation;



/* Default constructor */
PythonEvaluation::PythonEvaluation()
  : EvaluationImplementation()
  , pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonEvaluation::PythonEvaluation(PyObject * pyCallable)
  : EvaluationImplementation()
  , pyObj_(pyCallable)
{
  Py_XINCREF(pyCallable);

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
  , pyObj_(other.pyObj_)
{
  Py_XINCREF(pyObj_);
}

/* Destructor */
PythonEvaluation::~PythonEvaluation()
{
  Py_XDECREF(pyObj_);
}

/* Comparison operator */
Bool PythonEvaluation::operator ==(const PythonEvaluation & other) const
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
String PythonEvaluation::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << PythonEvaluation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool PythonEvaluation::isActualImplementation() const
{
  return true;
}



/* Here is the interface that all derived class must implement */

/* Operator () */
Point PythonEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger dimension = inP.getDimension();

  if (dimension != getInputDimension())
    throw InvalidDimensionException(HERE) << "Input point has incorrect dimension. Got " << dimension << ". Expected " << getInputDimension();

  Point outP;
  CacheKeyType inKey(inP.getCollection());
  if (p_cache_->isEnabled() && p_cache_->hasKey(inKey))
  {
    outP = Point::ImplementationType(p_cache_->find(inKey));
  }
  else
  {
    ++ callsNumber_;

    ScopedPyObjectPointer point(convert< Point, _PySequence_ >(inP));
    ScopedPyObjectPointer execName(convert< String, _PyString_ >("_exec"));
    ScopedPyObjectPointer result(PyObject_CallMethodObjArgs(pyObj_, execName.get(), point.get(), NULL));

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

    if (p_cache_->isEnabled())
    {
      CacheValueType outValue(outP.getCollection());
      p_cache_->add(inKey, outValue);
    }
  }
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(outP);
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
    callsNumber_ += size;

    ScopedPyObjectPointer result;
    ScopedPyObjectPointer execSampleName(convert< String, _PyString_ >("_exec_sample"));
    Bool copy = true;
    ScopedPyObjectPointer copyObj(PyObject_GetAttrString( pyObj_, "_copy" ));
    if ( copyObj.get() )
    {
      copy = convert< _PyBool_, Bool >(copyObj.get());
    }
    if (inS.pyObjSample_ != NULL && (!copy))
    {
      result = PyObject_CallMethodObjArgs(pyObj_, execSampleName.get(), inS.pyObjSample_, NULL);
    }
    else
    {
      ScopedPyObjectPointer inTuple(PyTuple_New(size));

      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        PyObject * eltTuple = PyTuple_New(inDim);
        for (UnsignedInteger j = 0; j < inDim; ++ j) PyTuple_SetItem(eltTuple, j, convert< Scalar, _PyFloat_ > (inS(i, j)));
        PyTuple_SetItem(inTuple.get(), i, eltTuple);
      }

      result = PyObject_CallMethodObjArgs(pyObj_, execSampleName.get(), inTuple.get(), NULL);
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

  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(outS);
  }
  outS.setDescription(getOutputDescription());
  return outS;
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


/* Method save() stores the object through the StorageManager */
void PythonEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
