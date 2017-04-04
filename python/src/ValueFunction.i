// SWIG file ValueFunction.i

%{
#include "openturns/ValueFunction.hxx"
%}

%include ValueFunction_doc.i

%include openturns/ValueFunction.hxx
namespace OT { %extend ValueFunction { ValueFunction(const ValueFunction & other) { return new OT::ValueFunction(other); } } }


