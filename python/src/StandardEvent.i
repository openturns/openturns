// SWIG file StandardEvent.i

%{
#include "openturns/StandardEvent.hxx"
%}

%include StandardEvent_doc.i

%include openturns/StandardEvent.hxx
namespace OT { %extend StandardEvent { StandardEvent(const StandardEvent & other) { return new OT::StandardEvent(other); } } }
