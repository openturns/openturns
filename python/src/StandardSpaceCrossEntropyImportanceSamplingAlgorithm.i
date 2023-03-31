// SWIG file StandardSpaceCrossEntropyImportanceSamplingAlgorithm.i

%{
#include "openturns/StandardSpaceCrossEntropyImportanceSamplingAlgorithm.hxx"
%}

%include StandardSpaceCrossEntropyImportanceSamplingAlgorithm_doc.i

%include openturns/StandardSpaceCrossEntropyImportanceSamplingAlgorithm.hxx
namespace OT { %extend StandardSpaceCrossEntropyImportanceSamplingAlgorithm { StandardSpaceCrossEntropyImportanceSamplingAlgorithm(const StandardSpaceCrossEntropyImportanceSamplingAlgorithm & other) { return new OT::StandardSpaceCrossEntropyImportanceSamplingAlgorithm(other); } } }
