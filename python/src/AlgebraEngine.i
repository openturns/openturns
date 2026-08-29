// SWIG file AlgebraEngine.i

%{
#include "openturns/AlgebraEngine.hxx"
%}

%typemap(in, numinputs=0) OT::Scalar & signOut ($*ltype temp) %{ temp = 0.; $1 = &temp; %}
%typemap(argout) OT::Scalar & signOut %{ $result = OT::AppendOutput($result, OT::convert< OT::Scalar, OT::_PyFloat_ >(*$1) ); %}

%include AlgebraEngine_doc.i

%nodefaultctor AlgebraEngine;

%include openturns/AlgebraEngine.hxx

namespace OT {}
