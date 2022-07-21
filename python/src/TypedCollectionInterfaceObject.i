// SWIG file TypedCollectionInterfaceObject.i

%{
#include "openturns/TypedCollectionInterfaceObject.hxx"
%}

%ignore OT::TypedCollectionInterfaceObject::swap;

%define TypedCollectionInterfaceObjectMisnamedHelper(Namespace, Interface, CollectionType)

%template(CollectionType) OT::Collection<Namespace::Interface>;

%typemap(in) const CollectionType & (OT::Pointer<OT::Collection<Namespace::Interface> > temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence< Namespace::Interface >($input);
      $1 = temp.get();
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of " # Interface);
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CollectionType & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< Namespace::Interface >($input);
}

%apply const CollectionType & { const OT::Collection<Namespace::Interface> & };

%enddef

%define OTTypedCollectionInterfaceObjectMisnamedHelper(Interface,CollectionType)
TypedCollectionInterfaceObjectMisnamedHelper(OT, Interface, CollectionType)
%enddef

%define OTTypedCollectionInterfaceObjectHelper(Interface)
OTTypedCollectionInterfaceObjectMisnamedHelper(Interface, Interface ## Collection)
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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Interface, SWIG_POINTER_NO_NULL))) {
      OT::Interface * p_it = reinterpret_cast< OT::Interface * >(ptr);
      return p_it != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Implementation, SWIG_POINTER_NO_NULL))) {
      OT::Implementation * p_impl = reinterpret_cast< OT::Implementation * >( ptr );
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::Interface
  convert< _PyObject_, OT::Interface >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Interface, SWIG_POINTER_NO_NULL))) {
      OT::Interface * p_it = reinterpret_cast< OT::Interface * >(ptr);
      return *p_it;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__ ## Implementation, SWIG_POINTER_NO_NULL))) {
      OT::Implementation * p_impl = reinterpret_cast< OT::Implementation * >(ptr);
      return *p_impl;
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


%include openturns/TypedCollectionInterfaceObject.hxx

namespace OT {  
%extend TypedCollectionInterfaceObject {
  void erase(UnsignedInteger position) { self->erase(position); }
}
}
