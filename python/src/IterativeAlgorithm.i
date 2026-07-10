// SWIG file IterativeAlgorithm.i

%{
#include "openturns/IterativeAlgorithm.hxx"
%}

%include IterativeAlgorithm_doc.i

OTTypedInterfaceObjectHelper(IterativeAlgorithm)

%copyctor OT::IterativeAlgorithm;
%include openturns/IterativeAlgorithm.hxx

