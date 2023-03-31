// SWIG file PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm.i

%{
#include "openturns/PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm.hxx"
%}

%include PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm.i

%include openturns/PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm.hxx
namespace OT { %extend PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm { PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm(const PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm & other) { return new OT::PhysicalSpaceCrossEntropyImportanceSamplingAlgorithm(other); } } }
