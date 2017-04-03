// SWIG file HessianImplementation.i

%{
#include "openturns/HessianImplementation.hxx"
%}

%include HessianImplementation_doc.i

%include openturns/HessianImplementation.hxx
namespace OT { %extend HessianImplementation { HessianImplementation(const HessianImplementation & other) { return new OT::HessianImplementation(other); } } }
