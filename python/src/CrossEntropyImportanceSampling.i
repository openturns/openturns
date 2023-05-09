// SWIG file CrossEntropyImportanceSampling.i

%{
#include "openturns/CrossEntropyImportanceSampling.hxx"
%}

%include CrossEntropyImportanceSampling_doc.i

%include openturns/CrossEntropyImportanceSampling.hxx
namespace OT { %extend CrossEntropyImportanceSampling { CrossEntropyImportanceSampling(const CrossEntropyImportanceSampling & other) { return new OT::CrossEntropyImportanceSampling(other); } } }
