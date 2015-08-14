// SWIG file Process.i

%{
#include "Process.hxx"
%}

%include Process_doc.i

OTTypedInterfaceObjectHelper(Process)

%include Process.hxx
namespace OT { %extend Process { Process(const Process & other) { return new OT::Process(other); } } }

