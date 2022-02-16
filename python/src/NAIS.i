// SWIG file NAIS.i

%{
#include "openturns/NAIS.hxx"
#include "openturns/NAISResult.hxx"
%}

%include NAIS_doc.i

%include openturns/NAIS.hxx
namespace OT { %extend NAIS { NAIS(const NAIS & other) { return new OT::NAIS(other); } } }
