//                                               -*- C++ -*-
/**
 *  @brief PythonActiveLearningReliabilityFunctionImplementation implementation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PythonActiveLearningReliabilityFunctionImplementation.hxx"
#include "openturns/swigpyrun_once.h"
#include "openturns/OSS.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonActiveLearningReliabilityFunctionImplementation)

static const Factory<PythonActiveLearningReliabilityFunctionImplementation> Factory_PythonActiveLearningReliabilityFunctionImplementation;

/* Default constructor */
PythonActiveLearningReliabilityFunctionImplementation::PythonActiveLearningReliabilityFunctionImplementation()
  : ActiveLearningReliabilityFunctionImplementation()
  , pyObj_(nullptr)
{
  // Nothing to do
}

/* Constructor from Python object */
PythonActiveLearningReliabilityFunctionImplementation::PythonActiveLearningReliabilityFunctionImplementation(PyObject * pyObject)
  : ActiveLearningReliabilityFunctionImplementation()
  , pyObj_(pyObject)
{
  Py_XINCREF(pyObj_);

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString(pyObj_, const_cast<char *>("__class__")));
  ScopedPyObjectPointer name(PyObject_GetAttrString(cls.get(), const_cast<char *>("__name__")));
  setName(checkAndConvert<_PyString_, String>(name.get()));

  if (!PyObject_HasAttrString(pyObj_, const_cast<char *>("getReliabilityThreshold")))
    throw InvalidArgumentException(HERE) << "Error: the given object does not have a getReliabilityThreshold() method.";
  if (!PyObject_HasAttrString(pyObj_, const_cast<char *>("getLearningThreshold")))
    throw InvalidArgumentException(HERE) << "Error: the given object does not have a getLearningThreshold() method.";

  ScopedPyObjectPointer relResult(PyObject_CallMethod(pyObj_, const_cast<char *>("getReliabilityThreshold"), const_cast<char *>("()")));
  if (relResult.isNull())
    handleException();
  const Scalar reliabilityThreshold = checkAndConvert<_PyFloat_, Scalar>(relResult.get());

  ScopedPyObjectPointer learnResult(PyObject_CallMethod(pyObj_, const_cast<char *>("getLearningThreshold"), const_cast<char *>("()")));
  if (learnResult.isNull())
    handleException();
  const Scalar learningThreshold = checkAndConvert<_PyFloat_, Scalar>(learnResult.get());

  // Bypass the syncing setters: the Python side already holds these values
  ActiveLearningReliabilityFunctionImplementation::setReliabilityThreshold(reliabilityThreshold);
  ActiveLearningReliabilityFunctionImplementation::setLearningThreshold(learningThreshold);
}

/* Virtual constructor */
PythonActiveLearningReliabilityFunctionImplementation * PythonActiveLearningReliabilityFunctionImplementation::clone() const
{
  return new PythonActiveLearningReliabilityFunctionImplementation(*this);
}

/* Copy constructor */
PythonActiveLearningReliabilityFunctionImplementation::PythonActiveLearningReliabilityFunctionImplementation(const PythonActiveLearningReliabilityFunctionImplementation & other)
  : ActiveLearningReliabilityFunctionImplementation(other)
  , pyObj_(nullptr)
{
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  pyObj_ = pyObjClone.get();
  Py_XINCREF(pyObj_);
  // deepCopy drops the GPR result through Python __getstate__: restore it from C++
  syncGPRToPython();
  // The Python copy is the source of truth for the thresholds
  syncThresholdsFromPython();
}

/* Copy assignment operator */
PythonActiveLearningReliabilityFunctionImplementation & PythonActiveLearningReliabilityFunctionImplementation::operator=(const PythonActiveLearningReliabilityFunctionImplementation & rhs)
{
  if (this != &rhs)
  {
    ActiveLearningReliabilityFunctionImplementation::operator=(rhs);
    ScopedPyObjectPointer pyObjClone(deepCopy(rhs.pyObj_));
    pyObj_ = pyObjClone.get();
    Py_XINCREF(pyObj_);
    // deepCopy drops the GPR result through Python __getstate__: restore it from C++
    syncGPRToPython();
    // The Python copy is the source of truth for the thresholds
    syncThresholdsFromPython();
  }
  return *this;
}

/* Destructor */
PythonActiveLearningReliabilityFunctionImplementation::~PythonActiveLearningReliabilityFunctionImplementation()
{
  Py_XDECREF(pyObj_);
}

/* String converter */
String PythonActiveLearningReliabilityFunctionImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonActiveLearningReliabilityFunctionImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Is it safe to call in parallel? No: Python calls hold the GIL. */
Bool PythonActiveLearningReliabilityFunctionImplementation::isParallel() const
{
#ifdef Py_GIL_DISABLED
  return true;
#else
  return false;
#endif
}

static Sample convertPyResultToSample(PyObject * pyResult, const char * const methodName)
{
  void * ptr = nullptr;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyResult, &ptr, SWIG_TypeQuery("OT::Sample *"), 0)))
  {
    const OT::Sample * const p_sample = reinterpret_cast<const OT::Sample *>(ptr);
    if (p_sample != nullptr)
      return *p_sample;
  }
  try
  {
    return convert<_PySequence_, Sample>(pyResult);
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Output value for PythonActiveLearningReliabilityFunction." << methodName << "() is not a Sample nor a 2d-sequence object";
  }
}

/* Score the candidate pool against the current design */
Sample PythonActiveLearningReliabilityFunctionImplementation::operator()(const Sample & inputSample, const Sample & inputDoE) const
{
  ScopedPyObjectPointer pyCandidate(SWIG_NewPointerObj(new Sample(inputSample), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  ScopedPyObjectPointer pyDoe(SWIG_NewPointerObj(new Sample(inputDoE), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  ScopedPyObjectPointer methodName(convert<String, _PyString_>("__call__"));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), pyCandidate.get(), pyDoe.get(), nullptr));
  if (callResult.isNull())
    handleException();
  const Sample result(convertPyResultToSample(callResult.get(), "__call__"));
  if (result.getDimension() != 1)
    throw InvalidDimensionException(HERE) << "Criterion sample returned by PythonActiveLearningReliabilityFunction has incorrect dimension. Got " << result.getDimension() << ". Expected 1";
  if (result.getSize() != inputSample.getSize())
    throw InvalidDimensionException(HERE) << "Criterion sample returned by PythonActiveLearningReliabilityFunction has incorrect size. Got " << result.getSize() << ". Expected " << inputSample.getSize();
  return result;
}

/* Select the infill sample to evaluate on the true function */
Sample PythonActiveLearningReliabilityFunctionImplementation::getInfillSample(const Sample & inputSample,
    const Sample & criterionValues) const
{
  ScopedPyObjectPointer pyCandidates(SWIG_NewPointerObj(new Sample(inputSample), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  ScopedPyObjectPointer pyValues(SWIG_NewPointerObj(new Sample(criterionValues), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  ScopedPyObjectPointer methodName(convert<String, _PyString_>("getInfillSample"));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), pyCandidates.get(), pyValues.get(), nullptr));
  if (callResult.isNull())
    handleException();
  const Sample result(convertPyResultToSample(callResult.get(), "getInfillSample"));
  if (result.getDimension() != inputSample.getDimension())
    throw InvalidDimensionException(HERE) << "Infill sample returned by PythonActiveLearningReliabilityFunction has incorrect dimension. Got " << result.getDimension() << ". Expected " << inputSample.getDimension();
  if (result.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Infill sample returned by PythonActiveLearningReliabilityFunction is empty";
  return result;
}

/* Test convergence of active learning */
Bool PythonActiveLearningReliabilityFunctionImplementation::checkConvergenceLearning(const Sample & criterionValues) const
{
  ScopedPyObjectPointer pyValues(SWIG_NewPointerObj(new Sample(criterionValues), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  ScopedPyObjectPointer methodName(convert<String, _PyString_>("checkConvergenceLearning"));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), pyValues.get(), nullptr));
  if (callResult.isNull())
    handleException();
  const Bool result = checkAndConvert<_PyBool_, Bool>(callResult.get());
  return result;
}

/* Selection direction */
Bool PythonActiveLearningReliabilityFunctionImplementation::isMaximization() const
{
  ScopedPyObjectPointer callResult(PyObject_CallMethod(pyObj_, const_cast<char *>("isMaximization"), const_cast<char *>("()")));
  if (callResult.isNull())
    handleException();
  const Bool result = checkAndConvert<_PyBool_, Bool>(callResult.get());
  return result;
}

/* Pointwise score, forwards to Python computeAsScalar */
Scalar PythonActiveLearningReliabilityFunctionImplementation::computeAsScalar(const Point & x) const
{
  if (!PyObject_HasAttrString(pyObj_, const_cast<char *>("computeAsScalar")))
    throw NotYetImplementedException(HERE) << "In PythonActiveLearningReliabilityFunctionImplementation::computeAsScalar: Python object has no computeAsScalar() method";
  ScopedPyObjectPointer methodName(convert<String, _PyString_>("computeAsScalar"));
  ScopedPyObjectPointer point(convert<Point, _PySequence_>(x));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), point.get(), nullptr));
  if (callResult.isNull())
    handleException();
  const Scalar result = checkAndConvert<_PyFloat_, Scalar>(callResult.get());
  return result;
}

/* Gaussian process surrogate accessor, keeps the Python side in sync */
void PythonActiveLearningReliabilityFunctionImplementation::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  ActiveLearningReliabilityFunctionImplementation::setGaussianProcessRegression(gprResult);
  syncGPRToPython();
}

/* Threshold accessors, keep the Python side in sync */
void PythonActiveLearningReliabilityFunctionImplementation::setReliabilityThreshold(const Scalar reliabilityThreshold)
{
  ActiveLearningReliabilityFunctionImplementation::setReliabilityThreshold(reliabilityThreshold);
  syncThresholdsToPython();
}

void PythonActiveLearningReliabilityFunctionImplementation::setLearningThreshold(const Scalar learningThreshold)
{
  ActiveLearningReliabilityFunctionImplementation::setLearningThreshold(learningThreshold);
  syncThresholdsToPython();
}

Scalar PythonActiveLearningReliabilityFunctionImplementation::getReliabilityThreshold() const
{
  // The Python object may have modified its threshold: refresh the C++ value
  syncThresholdsFromPython();
  return ActiveLearningReliabilityFunctionImplementation::getReliabilityThreshold();
}

Scalar PythonActiveLearningReliabilityFunctionImplementation::getLearningThreshold() const
{
  // The Python object may have modified its threshold: refresh the C++ value
  syncThresholdsFromPython();
  return ActiveLearningReliabilityFunctionImplementation::getLearningThreshold();
}

void PythonActiveLearningReliabilityFunctionImplementation::syncGPRToPython() const
{
  if (pyObj_ == nullptr)
    return;
  ScopedPyObjectPointer pyGpr(SWIG_NewPointerObj(new GaussianProcessRegressionResult(gprResult_), SWIG_TypeQuery("OT::GaussianProcessRegressionResult *"), SWIG_POINTER_OWN));
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("setGaussianProcessRegression")))
  {
    ScopedPyObjectPointer methodName(convert<String, _PyString_>("setGaussianProcessRegression"));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), pyGpr.get(), nullptr));
    if (callResult.isNull())
      handleException();
  }
  else if (PyObject_SetAttrString(pyObj_, const_cast<char *>("_gprResult"), pyGpr.get()) != 0)
    handleException();
}

void PythonActiveLearningReliabilityFunctionImplementation::syncThresholdsToPython() const
{
  if (pyObj_ == nullptr)
    return;
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>("setReliabilityThreshold")) && PyObject_HasAttrString(pyObj_, const_cast<char *>("setLearningThreshold")))
  {
    ScopedPyObjectPointer relName(convert<String, _PyString_>("setReliabilityThreshold"));
    ScopedPyObjectPointer relArg(convert<Scalar, _PyFloat_>(reliabilityThreshold_));
    ScopedPyObjectPointer relResult(PyObject_CallMethodObjArgs(pyObj_, relName.get(), relArg.get(), nullptr));
    if (relResult.isNull())
      handleException();
    ScopedPyObjectPointer learnName(convert<String, _PyString_>("setLearningThreshold"));
    ScopedPyObjectPointer learnArg(convert<Scalar, _PyFloat_>(learningThreshold_));
    ScopedPyObjectPointer learnResult(PyObject_CallMethodObjArgs(pyObj_, learnName.get(), learnArg.get(), nullptr));
    if (learnResult.isNull())
      handleException();
  }
  else
  {
    ScopedPyObjectPointer relArg(convert<Scalar, _PyFloat_>(reliabilityThreshold_));
    if (PyObject_SetAttrString(pyObj_, const_cast<char *>("_reliabilityThreshold"), relArg.get()) != 0)
      handleException();
    ScopedPyObjectPointer learnArg(convert<Scalar, _PyFloat_>(learningThreshold_));
    if (PyObject_SetAttrString(pyObj_, const_cast<char *>("_learningThreshold"), learnArg.get()) != 0)
      handleException();
  }
}

void PythonActiveLearningReliabilityFunctionImplementation::syncThresholdsFromPython() const
{
  if (pyObj_ == nullptr)
    return;
  if (!PyObject_HasAttrString(pyObj_, const_cast<char *>("getReliabilityThreshold"))
      || !PyObject_HasAttrString(pyObj_, const_cast<char *>("getLearningThreshold")))
    return;

  ScopedPyObjectPointer relResult(PyObject_CallMethod(pyObj_, const_cast<char *>("getReliabilityThreshold"), const_cast<char *>("()")));
  if (relResult.isNull())
    handleException();
  const Scalar reliabilityThreshold = checkAndConvert<_PyFloat_, Scalar>(relResult.get());

  ScopedPyObjectPointer learnResult(PyObject_CallMethod(pyObj_, const_cast<char *>("getLearningThreshold"), const_cast<char *>("()")));
  if (learnResult.isNull())
    handleException();
  const Scalar learningThreshold = checkAndConvert<_PyFloat_, Scalar>(learnResult.get());

  // Bypass the syncing setters: the Python side already holds these values
  PythonActiveLearningReliabilityFunctionImplementation * self = const_cast<PythonActiveLearningReliabilityFunctionImplementation *>(this);
  self->ActiveLearningReliabilityFunctionImplementation::setReliabilityThreshold(reliabilityThreshold);
  self->ActiveLearningReliabilityFunctionImplementation::setLearningThreshold(learningThreshold);
}

/* Method save() stores the object through the StorageManager */
void PythonActiveLearningReliabilityFunctionImplementation::save(Advocate & adv) const
{
  // The Python object may have modified its thresholds: store up-to-date values
  syncThresholdsFromPython();
  ActiveLearningReliabilityFunctionImplementation::save(adv);
  pickleSave(adv, pyObj_);
}

/* Method load() reloads the object from the StorageManager */
void PythonActiveLearningReliabilityFunctionImplementation::load(Advocate & adv)
{
  ActiveLearningReliabilityFunctionImplementation::load(adv);
  pickleLoad(adv, pyObj_);
  // The Python __getstate__ drops the GPR result (SWIG object): restore it from C++
  syncGPRToPython();
  // The unpickled Python object is the source of truth for the thresholds
  syncThresholdsFromPython();
}

END_NAMESPACE_OPENTURNS
