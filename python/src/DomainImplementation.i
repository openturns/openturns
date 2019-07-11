// SWIG file DomainImplementation.i

%{
#include "openturns/DomainImplementation.hxx"
%}

%include DomainImplementation_doc.i

%include openturns/DomainImplementation.hxx
namespace OT { %extend DomainImplementation { DomainImplementation(const DomainImplementation & other) { return new OT::DomainImplementation(other); } } }

%pythoncode %{
DomainImplementation.__contains__ = DomainImplementation.contains
%}
