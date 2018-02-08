// SWIG file DomainComplement.i

%{
#include "openturns/DomainComplement.hxx"
%}

%include DomainComplement_doc.i

%include openturns/DomainComplement.hxx
namespace OT { %extend DomainComplement { DomainComplement(const DomainComplement & other) { return new OT::DomainComplement(other); } } }
