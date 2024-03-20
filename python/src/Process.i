// SWIG file Process.i

%{
#include "openturns/Process.hxx"
%}

%include Process_doc.i

OTTypedInterfaceObjectHelper(Process)

%copyctor OT::Process;

%include openturns/Process.hxx

