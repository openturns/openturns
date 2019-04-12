// SWIG file DomainUnion.i

%{
#include "openturns/DomainUnion.hxx"
%}

%include DomainUnion_doc.i

%include openturns/DomainUnion.hxx
namespace OT { %extend DomainUnion { DomainUnion(const DomainUnion & other) { return new OT::DomainUnion(other); } } }
