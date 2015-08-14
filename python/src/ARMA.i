// SWIG file ARMA.i

%{
#include "ARMA.hxx"
%}

%include ARMA_doc.i

%include ARMA.hxx
namespace OT { %extend ARMA { ARMA(const ARMA & other) { return new OT::ARMA(other); } } }
