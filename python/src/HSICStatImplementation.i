// SWIG file HSICStatImplementation.i

%{
#include "openturns/HSICStatImplementation.hxx"
%}

%include Gamma.i

%include HSICStatImplementation_doc.i

%include openturns/HSICStatImplementation.hxx

namespace OT {

%extend HSICStatImplementation {

HSICStatImplementation(const HSICStatImplementation & other) { return new OT::HSICStatImplementation(other); }

} }
