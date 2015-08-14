// SWIG file FittingAlgorithm.i

%{
#include "FittingAlgorithm.hxx"
%}

%include FittingAlgorithm_doc.i

OTTypedInterfaceObjectHelper(FittingAlgorithm)

%include FittingAlgorithm.hxx
namespace OT { %extend FittingAlgorithm { FittingAlgorithm(const FittingAlgorithm & other) { return new OT::FittingAlgorithm(other); } } }
