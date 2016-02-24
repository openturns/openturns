// SWIG file Process.i

%{
#include "openturns/Process.hxx"
%}

%include Process_doc.i

OTTypedInterfaceObjectHelper(Process)

%include openturns/Process.hxx
namespace OT { %extend Process { Process(const Process & other) { return new OT::Process(other); } } }

