// SWIG file BaseFuncCollection.i

%{
#include "openturns/PythonEvaluation.hxx"
#include "openturns/PythonGradient.hxx"
#include "openturns/PythonHessian.hxx"


namespace OT {

  template <>
  struct traitsPythonType< OT::Function >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::Function >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Function, 0))) {
      OT::Function * p_nmf = reinterpret_cast< OT::Function * >(ptr);
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FunctionImplementation, 0))) {
      OT::FunctionImplementation * p_impl = reinterpret_cast< OT::FunctionImplementation * >(ptr);
      return p_impl != NULL;
    } else {
      // pure callable
      return PyCallable_Check(pyObj) && !SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Object, 0));
    }
    return false;
  }

  template <>
  inline
  OT::Function
  convert< _PyObject_, OT::Function >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Function, 0))) {
      OT::Function * p_nmf = reinterpret_cast< OT::Function * >(ptr);
      return *p_nmf;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FunctionImplementation, 0))) {
      OT::FunctionImplementation * p_impl = reinterpret_cast< OT::FunctionImplementation * >(ptr);
      return *p_impl;
    } else if (!PyCallable_Check(pyObj) || SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Object, 0))) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a pure callable object (function or class) - can not be convertible to a Function";
    }
    OT::Function pythonFunction(new OT::FunctionImplementation(new OT::PythonEvaluation(pyObj)));
    if (PyObject_HasAttrString( pyObj, const_cast<char *>("_gradient")))
    {
      pythonFunction.setGradient(new OT::PythonGradient(pyObj));
    }
    if (PyObject_HasAttrString(pyObj, const_cast<char *>("_hessian")))
    {
      pythonFunction.setHessian(new OT::PythonHessian(pyObj));
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
    // Case of Collection<NMF>?
    throw OT::NotYetImplementedException(HERE) << "convert< _PyObject_, OT::Basis >"; 
  }

} /* namespace OT */

%}
