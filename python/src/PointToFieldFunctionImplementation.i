// SWIG file PointToFieldFunctionImplementation.i

%{
#include "openturns/PointToFieldFunctionImplementation.hxx"
%}

%include PointToFieldFunctionImplementation_doc.i

%include openturns/PointToFieldFunctionImplementation.hxx
namespace OT { %extend PointToFieldFunctionImplementation { PointToFieldFunctionImplementation(const PointToFieldFunctionImplementation & other) { return new OT::PointToFieldFunctionImplementation(other); } } }
