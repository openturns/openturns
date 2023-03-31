// SWIG file CrossEntropyImportanceSamplingAlgorithm.i

%{
#include "openturns/CrossEntropyImportanceSamplingAlgorithm.hxx"
%}

%include CrossEntropyImportanceSamplingAlgorithm_doc.i

%include openturns/CrossEntropyImportanceSamplingAlgorithm.hxx
namespace OT { %extend CrossEntropyImportanceSamplingAlgorithm { CrossEntropyImportanceSamplingAlgorithm(const CrossEntropyImportanceSamplingAlgorithm & other) { return new OT::CrossEntropyImportanceSamplingAlgorithm(other); } } }
