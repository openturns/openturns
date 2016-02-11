// SWIG file TemporalFunction.i

%{
#include "openturns/TemporalFunction.hxx"
%}

%include TemporalFunction_doc.i

%include openturns/TemporalFunction.hxx
namespace OT { %extend TemporalFunction { TemporalFunction(const TemporalFunction & other) { return new OT::TemporalFunction(other); } } }
