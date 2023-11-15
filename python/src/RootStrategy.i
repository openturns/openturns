// SWIG file RootStrategy.i

%{
#include "openturns/RootStrategy.hxx"
%}

%include RootStrategy_doc.i

OTTypedInterfaceObjectHelper(RootStrategy)

%copyctor OT::RootStrategy;

%include openturns/RootStrategy.hxx
