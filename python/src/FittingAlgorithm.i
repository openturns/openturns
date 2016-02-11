// SWIG file FittingAlgorithm.i

%{
#include "openturns/FittingAlgorithm.hxx"
%}

%include FittingAlgorithm_doc.i

OTTypedInterfaceObjectHelper(FittingAlgorithm)

%include openturns/FittingAlgorithm.hxx
namespace OT { %extend FittingAlgorithm { FittingAlgorithm(const FittingAlgorithm & other) { return new OT::FittingAlgorithm(other); } } }
