// SWIG file DomainDisjunctiveUnion.i

%{
#include "openturns/DomainDisjunctiveUnion.hxx"
%}

%include DomainDisjunctiveUnion_doc.i

%include openturns/DomainDisjunctiveUnion.hxx
namespace OT { %extend DomainDisjunctiveUnion { DomainDisjunctiveUnion(const DomainDisjunctiveUnion & other) { return new OT::DomainDisjunctiveUnion(other); } } }
