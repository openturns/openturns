// SWIG file ProcessImplementation.i

%{
#include "openturns/ProcessImplementation.hxx"
%}

%include ProcessImplementation_doc.i

%include openturns/ProcessImplementation.hxx
namespace OT { %extend ProcessImplementation { ProcessImplementation(const ProcessImplementation & other) { return new OT::ProcessImplementation(other); } } }
