// SWIG file FittingAlgorithm.i

%{
#include "openturns/FittingAlgorithm.hxx"
%}

%include FittingAlgorithm_doc.i

OTTypedInterfaceObjectHelper(FittingAlgorithm)

%copyctor OT::FittingAlgorithm;

%include openturns/FittingAlgorithm.hxx
