// SWIG file TestResult.i

%{
#include "openturns/TestResult.hxx"

namespace OT {
  template <>
  struct traitsPythonType<OT::TestResult>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::TestResult
  convert<_PyObject_, OT::TestResult>(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::TestResult *"), SWIG_POINTER_NO_NULL))) {
      OT::TestResult * p_it = reinterpret_cast< OT::TestResult * >(ptr);
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a TestResult";
    }
    return OT::TestResult();
  }
}
%}

%template(TestResultCollection) OT::Collection<OT::TestResult>;

%include TestResult_doc.i

%copyctor OT::TestResult;

%include openturns/TestResult.hxx

