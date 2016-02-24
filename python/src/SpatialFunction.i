// SWIG file SpatialFunction.i

%{
#include "openturns/SpatialFunction.hxx"
%}

%include SpatialFunction_doc.i

%include openturns/SpatialFunction.hxx
namespace OT { %extend SpatialFunction { SpatialFunction(const SpatialFunction & other) { return new OT::SpatialFunction(other); } } }
