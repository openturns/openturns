// SWIG file PolygonArray.i

%{
#include "openturns/PolygonArray.hxx"

namespace OT {
  template <>
  struct traitsPythonType<OT::Polygon>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::Polygon >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Polygon *"), SWIG_POINTER_NO_NULL))) {
      OT::Polygon * p_it = reinterpret_cast< OT::Polygon * >(ptr);
      return p_it != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::Polygon
  convert< _PyObject_, OT::Polygon >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Polygon *"), SWIG_POINTER_NO_NULL))) {
      OT::Polygon * p_it = reinterpret_cast< OT::Polygon * >(ptr);
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Polygon";
    }
    return OT::Polygon();
  }
} // namespace OT
%}

%template(_PolygonCollection) OT::Collection<OT::Polygon>;

%typemap(in) const PolygonCollection & (OT::Pointer<OT::Collection<OT::Polygon> > temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence< OT::Polygon >($input);
      $1 = temp.get();
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Polygon");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const PolygonCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< OT::Polygon >($input);
}

%apply const PolygonCollection & { const OT::PolygonArray::PolygonCollection & };

%include PolygonArray_doc.i

%copyctor OT::PolygonArray;

%include openturns/PolygonArray.hxx
