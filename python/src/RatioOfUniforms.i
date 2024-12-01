// SWIG file RatioOfUniforms.i

%typemap(in, numinputs=0) OT::UnsignedInteger & tryNumberOut ($*ltype temp) %{ temp = OT::UnsignedInteger(); $1 = &temp; %}
%typemap(argout) OT::UnsignedInteger & tryNumberOut %{ $result = OT::AppendOutput($result, PyLong_FromUnsignedLong(*$1)); %}

%{
#include "openturns/RatioOfUniforms.hxx"
namespace OT {
  template <>
  struct traitsPythonType<OT::MultiStart>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::MultiStart
  convert< _PyObject_, OT::MultiStart >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::MultiStart *"), SWIG_POINTER_NO_NULL))) {
      OT::MultiStart * p_it = reinterpret_cast< OT::MultiStart * >(ptr);
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a MultiStart";
    }
    return OT::MultiStart();
  }
}
%}

%template(MultiStartCollection) OT::Collection<OT::MultiStart>;

%include RatioOfUniforms_doc.i

%copyctor OT::RatioOfUniforms;

%include openturns/RatioOfUniforms.hxx
