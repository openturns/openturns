// SWIG file BaseFuncCollection.i

%{
#include "openturns/PythonNumericalMathEvaluationImplementation.hxx"
#include "openturns/PythonNumericalMathGradientImplementation.hxx"
#include "openturns/PythonNumericalMathHessianImplementation.hxx"


namespace OT {

  template <>
  struct traitsPythonType< OT::NumericalMathFunction >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::NumericalMathFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__NumericalMathFunction, 0))) {
      OT::NumericalMathFunction * p_nmf = reinterpret_cast< OT::NumericalMathFunction * >(ptr);
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__NumericalMathFunctionImplementation, 0))) {
      OT::NumericalMathFunctionImplementation * p_impl = reinterpret_cast< OT::NumericalMathFunctionImplementation * >(ptr);
      return p_impl != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::NumericalMathFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::NumericalMathFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::NumericalMathFunctionImplementation> * >(ptr);
      return p_impl != NULL && !p_impl->isNull();
    } else {
      return PyCallable_Check(pyObj);
    }
    return false;
  }

  template <>
  inline
  OT::NumericalMathFunction
  convert< _PyObject_, OT::NumericalMathFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__NumericalMathFunction, 0))) {
      OT::NumericalMathFunction * p_nmf = reinterpret_cast< OT::NumericalMathFunction * >(ptr);
      return *p_nmf;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__NumericalMathFunctionImplementation, 0))) {
      OT::NumericalMathFunctionImplementation * p_impl = reinterpret_cast< OT::NumericalMathFunctionImplementation * >(ptr);
      return *p_impl;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::NumericalMathFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::NumericalMathFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::NumericalMathFunctionImplementation> * >(ptr);
      return **p_impl;
    } else if (!PyCallable_Check(pyObj)) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object (function or class) - can not be convertible to a NumericalMathFunction";
    }
    OT::NumericalMathFunction pythonFunction(new OT::NumericalMathFunctionImplementation(new OT::PythonNumericalMathEvaluationImplementation(pyObj)));
    if (PyObject_HasAttrString( pyObj, const_cast<char *>("_gradient")))
    {
      pythonFunction.setGradient(new OT::PythonNumericalMathGradientImplementation(pyObj));
    }
    if (PyObject_HasAttrString(pyObj, const_cast<char *>("_hessian")))
    {
      pythonFunction.setHessian(new OT::PythonNumericalMathHessianImplementation(pyObj));
    }
    return pythonFunction;
  }


  template <>
  struct traitsPythonType< OT::Basis >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::Basis >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Basis, 0))) {
      OT::Basis * p_nmf = reinterpret_cast< OT::Basis * >(ptr);
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__BasisImplementation, 0 ))) {
      OT::BasisImplementation * p_impl = reinterpret_cast< OT::BasisImplementation * >(ptr);
      return p_impl != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::BasisImplementation> *"), 0))) {
      OT::Pointer<OT::BasisImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::BasisImplementation> * >(ptr);
      return p_impl != NULL && !p_impl->isNull();
    }
    // Case of Collection<NMF>?
    return false;
  }

  template <>
  inline
  OT::Basis
  convert< _PyObject_, OT::Basis >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__Basis, 0)))
    {
      OT::Basis * p_nmf = reinterpret_cast< OT::Basis * >(ptr);
      return *p_nmf;
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__BasisImplementation, 0)))
    {
      OT::BasisImplementation * p_impl = reinterpret_cast< OT::BasisImplementation * >(ptr);
      return *p_impl;
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::BasisImplementation> *"), 0)))
    {
      OT::Pointer<OT::BasisImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::BasisImplementation> * >(ptr);
      return **p_impl;
    }
    // Case of Collection<NMF>?
    throw OT::NotYetImplementedException(HERE) << "convert< _PyObject_, OT::Basis >"; 
  }

} /* namespace OT */

%}
