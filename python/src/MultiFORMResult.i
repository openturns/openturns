// SWIG file MultiFORMResult.i

%{
#include "openturns/MultiFORMResult.hxx"

namespace OT {
  template <>
  struct traitsPythonType<OT::FORMResult>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::FORMResult
  convert< _PyObject_, OT::FORMResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::FORMResult *"), SWIG_POINTER_NO_NULL))) {
      OT::FORMResult * p_it = reinterpret_cast< OT::FORMResult * >(ptr);
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a FORMResult";
    }
    return OT::FORMResult();
  }
}
%}

%include MultiFORMResult_doc.i

%template(_FORMResultCollection) OT::Collection<OT::FORMResult>;

%include openturns/MultiFORMResult.hxx

namespace OT { %extend MultiFORMResult { MultiFORMResult(const MultiFORMResult & other) { return new OT::MultiFORMResult(other); } } }
