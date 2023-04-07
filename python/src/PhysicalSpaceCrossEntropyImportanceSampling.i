// SWIG file PhysicalSpaceCrossEntropyImportanceSampling.i

%{
#include "openturns/PhysicalSpaceCrossEntropyImportanceSampling.hxx"
%}

%include PhysicalSpaceCrossEntropyImportanceSampling_doc.i

%include openturns/PhysicalSpaceCrossEntropyImportanceSampling.hxx
namespace OT { %extend PhysicalSpaceCrossEntropyImportanceSampling { PhysicalSpaceCrossEntropyImportanceSampling(const PhysicalSpaceCrossEntropyImportanceSampling & other) { return new OT::PhysicalSpaceCrossEntropyImportanceSampling(other); } } }
