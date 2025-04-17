// SWIG file DomainImplementation.i

%{
#include "openturns/DomainImplementation.hxx"
%}

%include DomainImplementation_doc.i

%copyctor OT::DomainImplementation;

%include openturns/DomainImplementation.hxx

%pythoncode %{
DomainImplementation.__contains__ = DomainImplementation.contains
%}
