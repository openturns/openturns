// SWIG file BetaMuSigma.i

%{
#include "openturns/BetaMuSigma.hxx"
%}

%include BetaMuSigma_doc.i

%include openturns/BetaMuSigma.hxx
namespace OT{ %extend BetaMuSigma { BetaMuSigma(const BetaMuSigma & other) { return new OT::BetaMuSigma(other); } } }
