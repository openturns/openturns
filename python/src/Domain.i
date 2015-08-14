// SWIG file Domain.i

%{
#include "Domain.hxx"
%}

%include Domain_doc.i

OTTypedInterfaceObjectHelper(Domain)

%include Domain.hxx

namespace OT { %extend Domain { Domain (const Domain & other) { return new OT::Domain(other); } } }
