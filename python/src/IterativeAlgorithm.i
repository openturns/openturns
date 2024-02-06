// SWIG file IterativeAlgorithm.i

%{
#include "openturns/IterativeAlgorithm.hxx"
%}

%include IterativeAlgorithm_doc.i

OTTypedInterfaceObjectHelper(IterativeAlgorithm)

%include openturns/IterativeAlgorithm.hxx

%copyctor OT::IterativeAlgorithm;
