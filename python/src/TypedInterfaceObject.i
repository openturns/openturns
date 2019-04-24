// SWIG file TypedInterfaceObject.i

%{
#include "openturns/TypedInterfaceObject.hxx"
%}

%ignore OT::TypedInterfaceObject::swap;

%define TypedInterfaceObjectImplementationHelper(Namespace, Interface, Implementation)

%template(Implementation ## TypedInterfaceObject) OT::TypedInterfaceObject<Namespace::Implementation>;

%typemap(in) const Namespace::Interface & {
  void * ptr = 0;
  if ($input == Py_None) {
    // operators ignore the typecheck
    SWIG_exception(SWIG_TypeError, "Object passed as argument is None");
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_ ## Namespace ## __ ## Implementation, 0))) {
    // From Implementation*
    Namespace::Implementation * p_impl = reinterpret_cast< Namespace::Implementation * >(ptr);
    $1 = new Namespace::Interface(*p_impl);
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a " # Interface);
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const Namespace::Interface & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_ ## Namespace ## __ ## Implementation, 0));
}

%enddef


%define OTTypedInterfaceObjectImplementationHelper(Interface, Implementation)
TypedInterfaceObjectImplementationHelper(OT, Interface, Implementation)
%enddef

%define OTTypedInterfaceObjectHelper(Interface)
OTTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef

%include openturns/TypedInterfaceObject.hxx
