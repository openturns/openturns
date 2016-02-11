// SWIG file CompositeProcess.i

%{
#include "openturns/CompositeProcess.hxx"
%}

%include CompositeProcess_doc.i

%include openturns/CompositeProcess.hxx
namespace OT { %extend CompositeProcess { CompositeProcess(const CompositeProcess & other) { return new OT::CompositeProcess(other); } } }
