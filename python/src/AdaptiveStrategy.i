// SWIG file AdaptiveStrategy.i

%{
#include "openturns/AdaptiveStrategy.hxx"
%}

%include AdaptiveStrategy_doc.i

OTTypedInterfaceObjectHelper(AdaptiveStrategy)

%copyctor OT::AdaptiveStrategy;

%include openturns/AdaptiveStrategy.hxx
