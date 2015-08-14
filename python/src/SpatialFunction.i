// SWIG file SpatialFunction.i

%{
#include "SpatialFunction.hxx"
%}

%include SpatialFunction_doc.i

%include SpatialFunction.hxx
namespace OT { %extend SpatialFunction { SpatialFunction(const SpatialFunction & other) { return new OT::SpatialFunction(other); } } }
