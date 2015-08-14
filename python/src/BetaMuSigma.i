// SWIG file BetaMuSigma.i

%{
#include "BetaMuSigma.hxx"
%}

%include BetaMuSigma_doc.i

%include BetaMuSigma.hxx
namespace OT{ %extend BetaMuSigma { BetaMuSigma(const BetaMuSigma & other) { return new OT::BetaMuSigma(other); } } }
