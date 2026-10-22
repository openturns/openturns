// SWIG file ProbabilitySimulationResult.i

%{
#include "openturns/ProbabilitySimulationResult.hxx"

namespace OT {

template <>
inline PyObject * convert< OT::ProbabilitySimulationResult, _PyObject_ >(OT::ProbabilitySimulationResult val)
{
  return SWIG_NewPointerObj(new OT::ProbabilitySimulationResult(val), SWIG_TypeQuery("OT::ProbabilitySimulationResult *"), SWIG_POINTER_OWN);
}

template <>
inline OT::Collection<OT::ProbabilitySimulationResult> convert< _PySequence_, OT::Collection<OT::ProbabilitySimulationResult> >(PyObject * pyObj)
{
  OT::Pointer< OT::Collection<OT::ProbabilitySimulationResult> > ptr = OT::buildCollectionFromPySequence< OT::ProbabilitySimulationResult >(pyObj);
  return *ptr;
}

} // namespace OT

%}

%include ProbabilitySimulationResult_doc.i

%ignore OT::ProbabilitySimulationResult::setImplementationAsPersistentObject;
%ignore OT::ProbabilitySimulationResult::swap;
%ignore OT::ProbabilitySimulationResult::copyOnWrite;

// Automatic downcast machinery: same role as OTTypedInterfaceObjectHelper,
// with the actual implementation name (the stock macro assumes
// Interface ## Implementation)
OTTypedInterfaceObjectImplementationHelper(ProbabilitySimulationResult, ProbabilitySimulationResultImplementation)

// Conversion machinery for collections of handles: same content as
// OTDefaultCollectionConvertFunctionsMisnamed with the actual
// implementation name (the stock macro assumes Interface ## Implementation)
%{
namespace OT {
  template <>
  struct traitsPythonType<OT::ProbabilitySimulationResult>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::ProbabilitySimulationResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ProbabilitySimulationResult *"), SWIG_POINTER_NO_NULL))) {
      OT::ProbabilitySimulationResult * p_it = reinterpret_cast< OT::ProbabilitySimulationResult * >(ptr);
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ProbabilitySimulationResultImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::ProbabilitySimulationResultImplementation * p_impl = reinterpret_cast< OT::ProbabilitySimulationResultImplementation * >(ptr);
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::ProbabilitySimulationResult
  convert< _PyObject_, OT::ProbabilitySimulationResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ProbabilitySimulationResult *"), SWIG_POINTER_NO_NULL))) {
      OT::ProbabilitySimulationResult * p_it = reinterpret_cast< OT::ProbabilitySimulationResult * >(ptr);
      return *p_it;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ProbabilitySimulationResultImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::ProbabilitySimulationResultImplementation * p_impl = reinterpret_cast< OT::ProbabilitySimulationResultImplementation * >(ptr);
      return OT::ProbabilitySimulationResult(*p_impl);
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a ProbabilitySimulationResult";
    }
    return OT::ProbabilitySimulationResult();
  }

} /* namespace OT */
%}

OTTypedCollectionInterfaceObjectHelper(ProbabilitySimulationResult)

%extend OT::Collection<OT::ProbabilitySimulationResult> {
  OT_COLLECTION_GETITEM(OT::Collection<OT::ProbabilitySimulationResult>, OT::ProbabilitySimulationResult)
  OT_COLLECTION_SETITEM(OT::Collection<OT::ProbabilitySimulationResult>, OT::ProbabilitySimulationResult)
}

%copyctor OT::ProbabilitySimulationResult;

%include openturns/ProbabilitySimulationResult.hxx
