// SWIG file DomainIntersection.i

%{
#include "openturns/DomainIntersection.hxx"
%}

%include DomainIntersection_doc.i

%include openturns/DomainIntersection.hxx
namespace OT { %extend DomainIntersection { DomainIntersection(const DomainIntersection & other) { return new OT::DomainIntersection(other); } } }
