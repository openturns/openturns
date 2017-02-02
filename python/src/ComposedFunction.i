// SWIG file ComposedFunction.i

%{
#include "openturns/ComposedFunction.hxx"
%}

%include ComposedFunction_doc.i

%include openturns/ComposedFunction.hxx
namespace OT { %extend ComposedFunction { ComposedFunction(const ComposedFunction & other) { return new OT::ComposedFunction(other); } } }
