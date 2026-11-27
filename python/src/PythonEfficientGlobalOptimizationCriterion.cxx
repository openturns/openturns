//                                               -*- C++ -*-
/**
 *  @brief PythonEfficientGlobalOptimizationCriterion binds a Python object to
 *  an OpenTURNS' EfficientGlobalOptimizationCriterionImplementation
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
#include "openturns/PythonEfficientGlobalOptimizationCriterion.hxx"
#include "openturns/swigpyrun_once.h"
#include "openturns/OSS.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PythonEfficientGlobalOptimizationCriterion)

static const Factory<PythonEfficientGlobalOptimizationCriterion> Factory_PythonEfficientGlobalOptimizationCriterion;

/* Default constructor */
PythonEfficientGlobalOptimizationCriterion::PythonEfficientGlobalOptimizationCriterion()
  : EfficientGlobalOptimizationCriterionImplementation()
  , pyObj_(nullptr)
{
  // Nothing to do
}

/* Constructor from Python object */
PythonEfficientGlobalOptimizationCriterion::PythonEfficientGlobalOptimizationCriterion(PyObject * pyObject)
  : EfficientGlobalOptimizationCriterionImplementation()
  , pyObj_(pyObject)
{
  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString(pyObj_, const_cast<char *>("__class__")));
  ScopedPyObjectPointer name(PyObject_GetAttrString(cls.get(), const_cast<char *>("__name__")));
  setName(checkAndConvert<_PyString_, String>(name.get()));

  if (!PyObject_HasAttrString(pyObj_, const_cast<char *>("buildObjective")))
    throw InvalidArgumentException(HERE) << "Error: the given object does not have a buildObjective() method.";
  // Take the reference only once construction cannot throw: the destructor
  // does not run when the constructor throws, so an early incref would leak.
  Py_XINCREF(pyObj_);
}

/* Virtual constructor */
PythonEfficientGlobalOptimizationCriterion * PythonEfficientGlobalOptimizationCriterion::clone() const
{
  return new PythonEfficientGlobalOptimizationCriterion(*this);
}

/* Copy constructor */
PythonEfficientGlobalOptimizationCriterion::PythonEfficientGlobalOptimizationCriterion(const PythonEfficientGlobalOptimizationCriterion & other)
  : EfficientGlobalOptimizationCriterionImplementation(other)
  , pyObj_(nullptr)
{
  ScopedPyObjectPointer pyObjClone(deepCopy(other.pyObj_));
  // Keep the new reference alive while syncStateToPython() may throw: the
  // scoped pointer owns it until the end of the constructor.
  pyObj_ = pyObjClone.get();
  syncStateToPython();
  Py_XINCREF(pyObj_);
}

/* Copy assignment operator */
PythonEfficientGlobalOptimizationCriterion & PythonEfficientGlobalOptimizationCriterion::operator=(const PythonEfficientGlobalOptimizationCriterion & rhs)
{
  if (this != &rhs)
  {
    EfficientGlobalOptimizationCriterionImplementation::operator=(rhs);
    PyObject * newObj = rhs.pyObj_ ? deepCopy(rhs.pyObj_) : nullptr; // new reference
    Py_XDECREF(pyObj_);
    pyObj_ = newObj;
    syncStateToPython();
  }
  return *this;
}

/* Destructor */
PythonEfficientGlobalOptimizationCriterion::~PythonEfficientGlobalOptimizationCriterion()
{
  Py_XDECREF(pyObj_);
}

/* String converter */
String PythonEfficientGlobalOptimizationCriterion::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonEfficientGlobalOptimizationCriterion::GetClassName()
      << " name=" << getName();
  return oss;
}

/* The objective to maximize, built by the Python criterion */
Function PythonEfficientGlobalOptimizationCriterion::buildObjective() const
{
  ScopedPyObjectPointer methodName(convert<String, _PyString_>("buildObjective"));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), nullptr));
  if (callResult.isNull())
    handleException();
  void * ptr = nullptr;
  if (!SWIG_IsOK(SWIG_ConvertPtr(callResult.get(), &ptr, SWIG_TypeQuery("OT::Function *"), 0)))
    throw InvalidArgumentException(HERE) << "Output value for PythonEfficientGlobalOptimizationCriterion.buildObjective() is not a Function object";
  const OT::Function * const p_function = reinterpret_cast<const OT::Function *>(ptr);
  if (p_function == nullptr)
    throw InvalidArgumentException(HERE) << "Output value for PythonEfficientGlobalOptimizationCriterion.buildObjective() is a null Function object";
  return *p_function;
}

/* Convergence test on the criterion value, forwarded to Python */
Bool PythonEfficientGlobalOptimizationCriterion::checkConvergence(const Scalar criterionValue) const
{
  ScopedPyObjectPointer methodName(convert<String, _PyString_>("checkConvergence"));
  ScopedPyObjectPointer argument(convert<Scalar, _PyFloat_>(criterionValue));
  ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), argument.get(), nullptr));
  if (callResult.isNull())
    handleException();
  return checkAndConvert<_PyBool_, Bool>(callResult.get());
}

/* Gaussian process surrogate accessor, keeps the Python side in sync */
void PythonEfficientGlobalOptimizationCriterion::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  EfficientGlobalOptimizationCriterionImplementation::setGaussianProcessRegression(gprResult);
  ScopedPyObjectPointer value(SWIG_NewPointerObj(new GaussianProcessRegressionResult(gprResult_), SWIG_TypeQuery("OT::GaussianProcessRegressionResult *"), SWIG_POINTER_OWN));
  forwardToPython("setGaussianProcessRegression", "_gprResult", value.get());
}

/* Current design accessor, keeps the Python side in sync */
void PythonEfficientGlobalOptimizationCriterion::setInputSample(const Sample & inputSample)
{
  EfficientGlobalOptimizationCriterionImplementation::setInputSample(inputSample);
  ScopedPyObjectPointer value(SWIG_NewPointerObj(new Sample(inputSample_), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  forwardToPython("setInputSample", "_inputSample", value.get());
}

/* Best observed value accessor, keeps the Python side in sync */
void PythonEfficientGlobalOptimizationCriterion::setOptimalValue(const Scalar optimalValue)
{
  EfficientGlobalOptimizationCriterionImplementation::setOptimalValue(optimalValue);
  ScopedPyObjectPointer value(convert<Scalar, _PyFloat_>(optimalValue_));
  forwardToPython("setOptimalValue", "_optimalValue", value.get());
}

/* Direction accessor, keeps the Python side in sync */
void PythonEfficientGlobalOptimizationCriterion::setMinimization(const Bool minimization)
{
  EfficientGlobalOptimizationCriterionImplementation::setMinimization(minimization);
  ScopedPyObjectPointer value(PyBool_FromLong(minimization_ ? 1L : 0L));
  forwardToPython("setMinimization", "_minimization", value.get());
}

/* Noise function accessor, keeps the Python side in sync */
void PythonEfficientGlobalOptimizationCriterion::setNoiseFunction(const Function & noiseFunction)
{
  EfficientGlobalOptimizationCriterionImplementation::setNoiseFunction(noiseFunction);
  ScopedPyObjectPointer value(SWIG_NewPointerObj(new Function(noiseFunction_), SWIG_TypeQuery("OT::Function *"), SWIG_POINTER_OWN));
  forwardToPython("setNoiseFunction", "_noiseFunction", value.get());
}

/* Forward one value to the Python object */
void PythonEfficientGlobalOptimizationCriterion::forwardToPython(const char * setterName, const char * attributeName, PyObject * value) const
{
  if (pyObj_ == nullptr)
    return;
  if (PyObject_HasAttrString(pyObj_, const_cast<char *>(setterName)))
  {
    ScopedPyObjectPointer methodName(convert<String, _PyString_>(setterName));
    ScopedPyObjectPointer callResult(PyObject_CallMethodObjArgs(pyObj_, methodName.get(), value, nullptr));
    if (callResult.isNull())
      handleException();
  }
  else if (PyObject_SetAttrString(pyObj_, const_cast<char *>(attributeName), value) != 0)
    handleException();
}

/* Push the whole current state to the Python object */
void PythonEfficientGlobalOptimizationCriterion::syncStateToPython() const
{
  if (pyObj_ == nullptr)
    return;
  ScopedPyObjectPointer pyGpr(SWIG_NewPointerObj(new GaussianProcessRegressionResult(gprResult_), SWIG_TypeQuery("OT::GaussianProcessRegressionResult *"), SWIG_POINTER_OWN));
  forwardToPython("setGaussianProcessRegression", "_gprResult", pyGpr.get());
  ScopedPyObjectPointer pyDesign(SWIG_NewPointerObj(new Sample(inputSample_), SWIG_TypeQuery("OT::Sample *"), SWIG_POINTER_OWN));
  forwardToPython("setInputSample", "_inputSample", pyDesign.get());
  ScopedPyObjectPointer pyOptimalValue(convert<Scalar, _PyFloat_>(optimalValue_));
  forwardToPython("setOptimalValue", "_optimalValue", pyOptimalValue.get());
  ScopedPyObjectPointer pyMinimization(PyBool_FromLong(minimization_ ? 1L : 0L));
  forwardToPython("setMinimization", "_minimization", pyMinimization.get());
  ScopedPyObjectPointer pyNoise(SWIG_NewPointerObj(new Function(noiseFunction_), SWIG_TypeQuery("OT::Function *"), SWIG_POINTER_OWN));
  forwardToPython("setNoiseFunction", "_noiseFunction", pyNoise.get());
}

/* Method save() stores the object through the StorageManager */
void PythonEfficientGlobalOptimizationCriterion::save(Advocate & adv) const
{
  EfficientGlobalOptimizationCriterionImplementation::save(adv);
  pickleSave(adv, pyObj_);
}

/* Method load() reloads the object from the StorageManager */
void PythonEfficientGlobalOptimizationCriterion::load(Advocate & adv)
{
  EfficientGlobalOptimizationCriterionImplementation::load(adv);
  pickleLoad(adv, pyObj_);
  // The Python __getstate__ drops the Gaussian process and the noise function
  // (SWIG objects): restore the whole state from C++
  syncStateToPython();
}

END_NAMESPACE_OPENTURNS
