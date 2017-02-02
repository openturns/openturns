// SWIG file AggregatedFunction.i

%{
#include "openturns/AggregatedFunction.hxx"
%}

%include AggregatedFunction_doc.i

%include openturns/AggregatedFunction.hxx
namespace OT { %extend AggregatedFunction { AggregatedFunction(const AggregatedFunction & other) { return new OT::AggregatedFunction(other); } } }
