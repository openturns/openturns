// SWIG file GenericSimulationResult.i

%{
#include "openturns/GenericSimulationResult.hxx"

namespace OT {

template <>
inline PyObject * convert< OT::GenericSimulationResult, _PyObject_ >(OT::GenericSimulationResult val)
{
  return SWIG_NewPointerObj(new OT::GenericSimulationResult(val), SWIG_TypeQuery("OT::GenericSimulationResult *"), SWIG_POINTER_OWN);
}

template <>
inline OT::Collection<OT::GenericSimulationResult> convert< _PySequence_, OT::Collection<OT::GenericSimulationResult> >(PyObject * pyObj)
{
  OT::Pointer< OT::Collection<OT::GenericSimulationResult> > ptr = OT::buildCollectionFromPySequence< OT::GenericSimulationResult >(pyObj);
  return *ptr;
}

} // namespace OT

%}

%include GenericSimulationResult_doc.i

%ignore OT::GenericSimulationResult::setImplementationAsPersistentObject;
%ignore OT::GenericSimulationResult::swap;
%ignore OT::GenericSimulationResult::copyOnWrite;

// Automatic downcast machinery: same role as OTTypedInterfaceObjectHelper,
// with the actual implementation name (the stock macro assumes
// Interface ## Implementation)
OTTypedInterfaceObjectImplementationHelper(GenericSimulationResult, ProbabilitySimulationResult)

// Conversion machinery for collections of handles: same content as
// OTDefaultCollectionConvertFunctionsMisnamed with the actual
// implementation name (the stock macro assumes Interface ## Implementation)
%{
namespace OT {
  template <>
  struct traitsPythonType<OT::GenericSimulationResult>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::GenericSimulationResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::GenericSimulationResult *"), SWIG_POINTER_NO_NULL))) {
      OT::GenericSimulationResult * p_it = reinterpret_cast< OT::GenericSimulationResult * >(ptr);
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ProbabilitySimulationResult *"), SWIG_POINTER_NO_NULL))) {
      OT::ProbabilitySimulationResult * p_impl = reinterpret_cast< OT::ProbabilitySimulationResult * >(ptr);
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::GenericSimulationResult
  convert< _PyObject_, OT::GenericSimulationResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::GenericSimulationResult *"), SWIG_POINTER_NO_NULL))) {
      OT::GenericSimulationResult * p_it = reinterpret_cast< OT::GenericSimulationResult * >(ptr);
      return *p_it;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ProbabilitySimulationResult *"), SWIG_POINTER_NO_NULL))) {
      OT::ProbabilitySimulationResult * p_impl = reinterpret_cast< OT::ProbabilitySimulationResult * >(ptr);
      return OT::GenericSimulationResult(*p_impl);
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a GenericSimulationResult";
    }
    return OT::GenericSimulationResult();
  }

} /* namespace OT */
%}

OTTypedCollectionInterfaceObjectHelper(GenericSimulationResult)

%extend OT::Collection<OT::GenericSimulationResult> {
  OT_COLLECTION_GETITEM(OT::Collection<OT::GenericSimulationResult>, OT::GenericSimulationResult)
  OT_COLLECTION_SETITEM(OT::Collection<OT::GenericSimulationResult>, OT::GenericSimulationResult)
}

%copyctor OT::GenericSimulationResult;

%include openturns/GenericSimulationResult.hxx
