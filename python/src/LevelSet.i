// SWIG file LevelSet.i

%{
#include "openturns/LevelSet.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

namespace OT {

  template <>
  struct traitsPythonType< OT::LevelSet >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::LevelSet >(PyObject * pyObj)
  {
    void * ptr = 0;
    return SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::LevelSet *"), SWIG_POINTER_NO_NULL));
  }

  template <>
  inline
  OT::LevelSet
  convert< _PyObject_, OT::LevelSet >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::LevelSet *"), SWIG_POINTER_NO_NULL)))
    {
      OT::LevelSet * p_levelSet = reinterpret_cast< OT::LevelSet * >(ptr);
      return *p_levelSet;
    }
    throw OT::InvalidArgumentException(HERE) << "Argument is not a LevelSet";
  }

}
%}

%include LevelSet_doc.i

%copyctor OT::LevelSet;

%template(LevelSetCollection) OT::Collection<OT::LevelSet>;

%typemap(in) const OT::Collection<OT::LevelSet> & (OT::Pointer<OT::Collection<OT::LevelSet> > temp) {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From collection object, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence< OT::LevelSet >($input);
      $1 = temp.get();
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of LevelSet");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::Collection<OT::LevelSet> & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< OT::LevelSet >($input);
}

%include openturns/LevelSet.hxx
