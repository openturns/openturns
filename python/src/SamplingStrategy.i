// SWIG file SamplingStrategy.i

%{
#include "openturns/SamplingStrategy.hxx"
%}

%include SamplingStrategy_doc.i

OTTypedInterfaceObjectHelper(SamplingStrategy)

%copyctor OT::SamplingStrategy;

%include openturns/SamplingStrategy.hxx
