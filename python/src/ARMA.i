// SWIG file ARMA.i

%{
#include "openturns/ARMA.hxx"
%}

%include ARMA_doc.i

%include openturns/ARMA.hxx
namespace OT { %extend ARMA { ARMA(const ARMA & other) { return new OT::ARMA(other); } } }
