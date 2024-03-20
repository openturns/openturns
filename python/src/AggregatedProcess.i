// SWIG file AggregatedProcess.i

%{
#include "openturns/AggregatedProcess.hxx"
%}

%include AggregatedProcess_doc.i

OTTypedCollectionInterfaceObjectHelper(Process)
OTDefaultCollectionConvertFunctions(Process)

%copyctor OT::AggregatedProcess;

%include openturns/AggregatedProcess.hxx
