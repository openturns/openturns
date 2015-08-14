// SWIG file TypedInterfaceObject.i

%{
#include "TypedInterfaceObject.hxx"
%}

%ignore OT::TypedInterfaceObject::swap;

%define OTTypedInterfaceObjectImplementationHelper(Interface,Implementation)

%template(Implementation ## TypedInterfaceObject)           OT::TypedInterfaceObject<OT::Implementation>;

%typemap(in) const OT::Interface & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__ ## Implementation, 0))) {
    // From Implementation*
    OT::Implementation * p_impl = reinterpret_cast< OT::Implementation * >( ptr );
    $1 = new OT::Interface( *p_impl );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Pointer<OT::Implementation> *"), 0))) {
    // From Pointer<Implementation>
    OT::Pointer<OT::Implementation> * p_impl = reinterpret_cast< OT::Pointer<OT::Implementation> * >( ptr );
    $1 = new OT::Interface( **p_impl );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::Interface & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__ ## Implementation, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Pointer<OT::Implementation> *"), 0));
}

%enddef

%define OTTypedInterfaceObjectHelper(Interface)
OTTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef

%include TypedInterfaceObject.hxx
