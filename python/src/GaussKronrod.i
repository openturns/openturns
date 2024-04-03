// SWIG file GaussKronrod.i

%{
#include "openturns/GaussKronrod.hxx"

namespace OT {
  template <>
  struct traitsPythonType<OT::GaussKronrodRule>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::GaussKronrodRule
  convert< _PyObject_, OT::GaussKronrodRule >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::GaussKronrodRule *"), SWIG_POINTER_NO_NULL))) {
      OT::GaussKronrodRule * p_it = reinterpret_cast< OT::GaussKronrodRule * >(ptr);
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a GaussKronrodRule";
    }
    return OT::GaussKronrodRule();
  }
}
%}

%include GaussKronrod_doc.i

%template(GaussKronrodRuleCollection) OT::Collection<OT::GaussKronrodRule>;

%copyctor OT::GaussKronrod;

%include openturns/GaussKronrod.hxx
