// SWIG file StandardSpaceCrossEntropyImportanceSampling.i

%{
#include "openturns/StandardSpaceCrossEntropyImportanceSampling.hxx"
%}

%include StandardSpaceCrossEntropyImportanceSampling_doc.i

%include openturns/StandardSpaceCrossEntropyImportanceSampling.hxx
namespace OT { %extend StandardSpaceCrossEntropyImportanceSampling { StandardSpaceCrossEntropyImportanceSampling(const StandardSpaceCrossEntropyImportanceSampling & other) { return new OT::StandardSpaceCrossEntropyImportanceSampling(other); } } }
