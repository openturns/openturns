// SWIG file Domain.i

%{
#include "openturns/Domain.hxx"
%}

%include Domain_doc.i

OTTypedInterfaceObjectHelper(Domain)
OTTypedCollectionInterfaceObjectHelper(Domain)
OTDefaultCollectionConvertFunctions(Domain)

%include openturns/Domain.hxx

namespace OT { %extend Domain { Domain (const Domain & other) { return new OT::Domain(other); } } }

%pythoncode %{
Domain.__contains__ = Domain.contains
%}



