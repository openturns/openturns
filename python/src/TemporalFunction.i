// SWIG file TemporalFunction.i

%{
#include "TemporalFunction.hxx"
%}

%include TemporalFunction_doc.i

%include TemporalFunction.hxx
namespace OT { %extend TemporalFunction { TemporalFunction(const TemporalFunction & other) { return new OT::TemporalFunction(other); } } }
