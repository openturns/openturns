// SWIG file FunctionalChaosValidation.i

%{
#include "openturns/FunctionalChaosValidation.hxx"
%}

%include FunctionalChaosValidation_doc.i

%include openturns/FunctionalChaosValidation.hxx

namespace OT {%extend FunctionalChaosValidation {FunctionalChaosValidation(const FunctionalChaosValidation & other){return new OT::FunctionalChaosValidation(other);}}}
