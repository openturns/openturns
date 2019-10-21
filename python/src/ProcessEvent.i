// SWIG file ProcessEvent.i

%{
#include "openturns/ProcessEvent.hxx"
%}

%include ProcessEvent_doc.i

%include openturns/ProcessEvent.hxx
namespace OT { %extend ProcessEvent { ProcessEvent(const ProcessEvent & other) { return new OT::ProcessEvent(other); } } }
