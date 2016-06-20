// SWIG file AggregatedProcess.i

%{
#include "openturns/AggregatedProcess.hxx"
%}

%include AggregatedProcess_doc.i

OTTypedCollectionInterfaceObjectHelper(Process)
OTDefaultCollectionConvertFunctions(Process)

%include openturns/AggregatedProcess.hxx

namespace OT { %extend AggregatedProcess { AggregatedProcess(const AggregatedProcess & other) { return new OT::AggregatedProcess(other); } } }
