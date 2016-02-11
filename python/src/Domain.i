// SWIG file Domain.i

%{
#include "openturns/Domain.hxx"
%}

%include Domain_doc.i

OTTypedInterfaceObjectHelper(Domain)

%include openturns/Domain.hxx

namespace OT { %extend Domain { Domain (const Domain & other) { return new OT::Domain(other); } } }
