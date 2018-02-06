// SWIG file DomainDifference.i

%{
#include "openturns/DomainDifference.hxx"
%}

%include DomainComplement_doc.i

%include openturns/DomainDifference.hxx
namespace OT { %extend DomainDifference { DomainDifference(const DomainDifference & other) { return new OT::DomainDifference(other); } } }
