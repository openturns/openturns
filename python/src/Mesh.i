// SWIG file Mesh.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & coordinatesOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & coordinatesOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}
%typemap(in, numinputs=0) OT::UnsignedInteger & simplexIndexOut ($*ltype temp) %{ temp = OT::UnsignedInteger(); $1 = &temp; %}
%typemap(argout) OT::UnsignedInteger & simplexIndexOut %{ $result = OT::AppendOutput($result, OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(*$1)); %}

%{
#include "openturns/Mesh.hxx"
%}

%include Mesh_doc.i

%copyctor OT::Mesh;

%include openturns/Mesh.hxx
