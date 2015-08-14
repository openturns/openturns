// SWIG file TypedCollectionInterfaceObject.i

%{
#include "TypedCollectionInterfaceObject.hxx"
%}

%ignore OT::TypedCollectionInterfaceObject::swap;

%define OTTypedCollectionInterfaceObjectMisnamedHelper(Interface,CollectionType)

%template(CollectionType)           OT::Collection<OT::Interface>;

%typemap(in) const CollectionType & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    $1 = OT::buildCollectionFromPySequence< OT::Interface >( $input );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CollectionType & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OT::Interface >( $input );
}

%apply const CollectionType & { const OT::Collection<OT::Interface> & };

%enddef

%define OTTypedCollectionInterfaceObjectHelper(Interface)
OTTypedCollectionInterfaceObjectMisnamedHelper(Interface,Interface ## Collection)
%enddef


%define OTDefaultCollectionConvertFunctionsMisnamed(Interface,Implementation)
%{
namespace OT {
  template <>
  struct traitsPythonType<OT::Interface>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::Interface >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Interface, 0 ))) {
      OT::Interface * p_it = reinterpret_cast< OT::Interface * >( ptr );
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Implementation, 0 ))) {
      OT::Implementation * p_impl = reinterpret_cast< OT::Implementation * >( ptr );
      return p_impl != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::Implementation> *"), 0))) {
      OT::Pointer<OT::Implementation> * p_impl = reinterpret_cast< OT::Pointer<OT::Implementation> * >( ptr );
      return p_impl != NULL && !p_impl->isNull();
    }
    return false;
  }

  template <>
  inline
  OT::Interface
  convert< _PyObject_, OT::Interface >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Interface, 0 ))) {
      OT::Interface * p_it = reinterpret_cast< OT::Interface * >( ptr );
      return *p_it;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Implementation, 0 ))) {
      OT::Implementation * p_impl = reinterpret_cast< OT::Implementation * >( ptr );
      return *p_impl;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::Implementation> *"), 0))) {
      OT::Pointer<OT::Implementation> * p_impl = reinterpret_cast< OT::Pointer<OT::Implementation> * >( ptr );
      return **p_impl;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Interface";
    }
    return OT::Interface();
  }

} /* namespace OT */
%}
%enddef

%define OTDefaultCollectionConvertFunctions(Interface)
OTDefaultCollectionConvertFunctionsMisnamed(Interface,Interface ## Implementation)
%enddef


%include TypedCollectionInterfaceObject.hxx

namespace OT {  
%extend TypedCollectionInterfaceObject {
  void erase(UnsignedInteger position) { self->erase(position); }
}
}
