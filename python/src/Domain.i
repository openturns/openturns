// SWIG file Domain.i

%{
#include "openturns/Domain.hxx"
%}

%include Domain_doc.i

%template(DomainImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::DomainImplementation>;

%typemap(in) const Domain & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__DomainImplementation, SWIG_POINTER_NO_NULL))) {
    // From DomainImplementation*
    OT::DomainImplementation * p_impl = reinterpret_cast<OT::DomainImplementation * >(ptr);
    $1 = new OT::Domain(*p_impl);
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__Mesh, SWIG_POINTER_NO_NULL))) {
    // From Mesh
    OT::Mesh * p_mesh = reinterpret_cast< OT::Mesh * >(ptr);
    OT::MeshDomain * p_impl = new OT::MeshDomain(*p_mesh);
    $1 = new OT::Domain(*p_impl);
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a OT::Domain");
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const Domain & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__DomainImplementation, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__Mesh, SWIG_POINTER_NO_NULL));
}

%apply const Domain & { const OT::Domain & };

%include openturns/Domain.hxx

namespace OT { %extend Domain { Domain (const Domain & other) { return new OT::Domain(other); } } }

%pythoncode %{
Domain.__contains__ = Domain.contains
%}

