// SWIG file ARMAFactory.i

%{
#include "openturns/ARMAFactory.hxx"
%}

%include ARMAFactory_doc.i

OTTypedInterfaceObjectHelper(ARMAFactory)

%copyctor OT::ARMAFactory;

%include openturns/ARMAFactory.hxx

