// SWIG file NAISResult.i

%{
#include "openturns/NAISResult.hxx"
%}

%include NAISResult_doc.i

%include openturns/NAISResult.hxx
namespace OT { %extend NAISResult { NAISResult(const NAISResult & other) { return new OT::NAISResult(other); } } }
