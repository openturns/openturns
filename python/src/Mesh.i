// SWIG file Mesh.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & coordinatesOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & coordinatesOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIGTYPE_p_OT__Point,  SWIG_POINTER_OWN)); %}
%typemap(in, numinputs=0) OT::UnsignedInteger & simplexIndexOut ($*ltype temp) %{ temp = OT::UnsignedInteger(); $1 = &temp; %}
%typemap(argout) OT::UnsignedInteger & simplexIndexOut %{ $result = OT::AppendOutput($result, PyLong_FromUnsignedLong(*$1)); %}

%{
#include "openturns/Mesh.hxx"
%}

%include Mesh_doc.i

%copyctor OT::Mesh;

%include openturns/Mesh.hxx
