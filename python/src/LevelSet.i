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

%include openturns/LevelSet.hxx
