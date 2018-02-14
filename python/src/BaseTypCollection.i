// SWIG file BaseTypCollection.i

%{
#include "openturns/PythonWrappingFunctions.hxx"

namespace OT {
  template <>
  inline
  bool
  canConvert< _PySequence_, Indices >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Indices *"), 0))) {
      Indices * p_it = reinterpret_cast< Indices * >(ptr);
      return p_it != NULL;
    } else {
      return canConvertCollectionObjectFromPySequence< UnsignedInteger >(pyObj);
    }
    return false;
  }
}
%}

%typemap(in) const OT::Collection<OT::Indices> & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence<OT::Indices>($input);
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Indices");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::Collection<OT::Indices> & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence<OT::Indices>($input);
}

