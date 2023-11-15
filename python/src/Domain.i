// SWIG file Domain.i

%{
#include "openturns/Domain.hxx"
%}

%include Domain_doc.i

OTTypedInterfaceObjectHelper(Domain)
OTTypedCollectionInterfaceObjectHelper(Domain)
OTDefaultCollectionConvertFunctions(Domain)

%copyctor OT::Domain;

%include openturns/Domain.hxx

%pythoncode %{
Domain.__contains__ = Domain.contains
%}



