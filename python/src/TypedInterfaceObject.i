// SWIG file TypedInterfaceObject.i

%{
#include "openturns/TypedInterfaceObject.hxx"
%}

%ignore OT::TypedInterfaceObject::swap;

%define TypedInterfaceObjectImplementationHelper(Namespace, Interface, Implementation)

%template(Implementation ## TypedInterfaceObject) OT::TypedInterfaceObject<Namespace::Implementation>;

%typemap(in) const Namespace::Interface & ($1_basetype temp) {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, $descriptor(Namespace ## __ ## Implementation *), SWIG_POINTER_NO_NULL))) {
    // From Implementation*
    Namespace::Implementation * p_impl = reinterpret_cast< Namespace::Implementation * >(ptr);
    temp = Namespace::Interface(*p_impl);
    $1 = &temp;
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a " # Interface);
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const Namespace::Interface & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $descriptor(Namespace ## __ ## Implementation *), SWIG_POINTER_NO_NULL));
}

%enddef


%define OTTypedInterfaceObjectImplementationHelper(Interface, Implementation)
TypedInterfaceObjectImplementationHelper(OT, Interface, Implementation)
%enddef

%define OTTypedInterfaceObjectHelper(Interface)
OTTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef

%include openturns/TypedInterfaceObject.hxx
