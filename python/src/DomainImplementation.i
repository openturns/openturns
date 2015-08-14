// SWIG file DomainImplementation.i

%{
#include "DomainImplementation.hxx"
%}

%include DomainImplementation_doc.i

%include DomainImplementation.hxx
namespace OT { %extend DomainImplementation { DomainImplementation(const DomainImplementation & other) { return new OT::DomainImplementation(other); } } }
