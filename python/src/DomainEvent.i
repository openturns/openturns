// SWIG file DomainEvent.i

%{
#include "openturns/DomainEvent.hxx"
%}

%include DomainEvent_doc.i

%include openturns/DomainEvent.hxx
namespace OT { %extend DomainEvent { DomainEvent(const DomainEvent & other) { return new OT::DomainEvent(other); } } }
