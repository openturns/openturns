// SWIG file CrossEntropyResult.i

%{
#include "openturns/CrossEntropyResult.hxx"
%}

%include CrossEntropyResult_doc.i

%include openturns/CrossEntropyResult.hxx
namespace OT { %extend CrossEntropyResult { CrossEntropyResult(const CrossEntropyResult & other) { return new OT::CrossEntropyResult(other); } } }
