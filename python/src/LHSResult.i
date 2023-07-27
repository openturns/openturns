// SWIG file

%{
#include "openturns/LHSResult.hxx"
%}

%include LHSResult_doc.i

%ignore OT::LHSResult::add;

%include openturns/LHSResult.hxx
namespace OT { %extend LHSResult { LHSResult(const LHSResult & other) { return new OT::LHSResult(other); } } }
