// SWIG file TypedInterfaceObject.i

%{
#include "openturns/TypedInterfaceObject.hxx"
%}

%ignore OT::TypedInterfaceObject::swap;

%define TypedInterfaceObjectImplementationHelper(Namespace, Interface, ParentImplementation)

%template(_ ## ParentImplementation ## TypedInterfaceObject) OT::TypedInterfaceObject<Namespace::ParentImplementation>;

%typemap(in) const Namespace::Interface & ($1_basetype temp) {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, $descriptor(Namespace ## __ ## ParentImplementation *), SWIG_POINTER_NO_NULL))) {
    // From ParentImplementation*
    Namespace::ParentImplementation * p_impl = reinterpret_cast< Namespace::ParentImplementation * >(ptr);
    temp = Namespace::Interface(*p_impl);
    $1 = &temp;
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a " # Interface);
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const Namespace::Interface & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $descriptor(Namespace ## __ ## ParentImplementation *), SWIG_POINTER_NO_NULL));
}

%enddef


// SWIG typemaps to convert Pointer<ParentImplementation> objects to ChildImplementation.
// The typemaps implement dynamic typecasting to ChildImplementation for the Python version of TypedInterfaceObject::getImplementation.
%define OTgetImplementationHelper(Namespace, Interface, ParentImplementation)

// Avoid arbitrary creation of SwigValueWrapper thingies around Pointer<ParentImplementation> type objects
// following SWIG maintainer William Fulton's idea here:
// https://github.com/swig/swig/blob/90cdbee6a69d13b39d734083b9f91069533b0d7b/Examples/test-suite/template_private_assignment.i
%template() OT::Pointer<Namespace::ParentImplementation>;

// Different typemaps are generated for all ParentImplementation classes.
%typemap(out) OT::TypedInterfaceObject< Namespace::ParentImplementation >::Implementation
{
  // Construct the SWIG identifier of the derived class we want to get (say ChildImplementation).
  const OT::String childname = "Namespace::" + $1->getClassName() + "*";
  swig_type_info * childinfo = SWIG_TypeQuery(childname.c_str());

  // Make the result a PyObject* pointing to the ChildImplementation.
  $result = SWIG_NewPointerObj(SWIG_as_voidptr($1.get()->clone()), childinfo, SWIG_POINTER_OWN);
}

%enddef

%define OTTypedInterfaceObjectImplementationHelper(Interface, Implementation)
OTgetImplementationHelper(OT, Interface, Implementation)
TypedInterfaceObjectImplementationHelper(OT, Interface, Implementation)
%enddef

%define OTTypedInterfaceObjectHelper(Interface)
OTTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef

%include openturns/TypedInterfaceObject.hxx
