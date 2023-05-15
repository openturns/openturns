// SWIG file TimeVaryingResult.i

%{
#include "openturns/TimeVaryingResult.hxx"
%}

%include TimeVaryingResult_doc.i

%include openturns/TimeVaryingResult.hxx
namespace OT { %extend TimeVaryingResult { TimeVaryingResult(const TimeVaryingResult & other) { return new OT::TimeVaryingResult(other); } } }
