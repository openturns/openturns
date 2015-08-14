// SWIG file ProcessImplementation.i

%{
#include "ProcessImplementation.hxx"
%}

%include ProcessImplementation_doc.i

%include ProcessImplementation.hxx
namespace OT { %extend ProcessImplementation { ProcessImplementation(const ProcessImplementation & other) { return new OT::ProcessImplementation(other); } } }
