// SWIG file MultiStart.i

%{
#include "openturns/MultiStart.hxx"

namespace OT {
  template <>
  struct traitsPythonType<OT::OptimizationResult>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::OptimizationResult
  convert< _PyObject_, OT::OptimizationResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::OptimizationResult *"), 0))) {
      OT::OptimizationResult * p_it = reinterpret_cast< OT::OptimizationResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a OptimizationResult";
    }
    return OT::OptimizationResult();
  }
}
%}

%include MultiStart_doc.i

%template(OptimizationResultCollection) OT::Collection<OT::OptimizationResult>;

%include openturns/MultiStart.hxx
namespace OT{ %extend MultiStart { MultiStart(const MultiStart & other) { return new OT::MultiStart(other); } } }

