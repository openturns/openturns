// SWIG file IntersectionEvent.i

%{
#include "openturns/IntersectionEvent.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

namespace OT {
  template <>
  struct traitsPythonType< OT::RandomVector >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::RandomVector >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__RandomVector, SWIG_POINTER_NO_NULL))) {
      OT::RandomVector * p_it = reinterpret_cast< OT::RandomVector * >(ptr);
      return p_it != NULL;
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__RandomVectorImplementation, SWIG_POINTER_NO_NULL))) {
      OT::RandomVectorImplementation * p_impl = reinterpret_cast< OT::RandomVectorImplementation * >(ptr);
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::RandomVector
  convert< _PyObject_, OT::RandomVector >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__RandomVector, SWIG_POINTER_NO_NULL))) {
      OT::RandomVector * p_mf = reinterpret_cast< OT::RandomVector * >(ptr);
      return *p_mf;
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__RandomVectorImplementation, SWIG_POINTER_NO_NULL))) {
      OT::RandomVectorImplementation * p_impl = reinterpret_cast< OT::RandomVectorImplementation * >(ptr);
      return *p_impl;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a RandomVector";
    }
    return OT::RandomVector();
  }

} // OT
%}

%include IntersectionEvent_doc.i

%template(RandomVectorCollection) OT::Collection<OT::RandomVector>;

%typemap(in) const RandomVectorCollection & (OT::Pointer<OT::Collection<OT::RandomVector > > temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence< OT::RandomVector >($input);
      $1 = temp.get();
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of RandomVector");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const RandomVectorCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< OT::RandomVector >($input);
}

%apply const RandomVectorCollection & { const OT::IntersectionEvent::RandomVectorCollection & };


%include openturns/IntersectionEvent.hxx
namespace OT { %extend IntersectionEvent { IntersectionEvent(const IntersectionEvent & other) { return new OT::IntersectionEvent(other); } } }
