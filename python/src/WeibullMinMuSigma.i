// SWIG file WeibullMinMuSigma.i

%{
#include "openturns/WeibullMinMuSigma.hxx"
%}

%include WeibullMinMuSigma_doc.i

%include openturns/WeibullMinMuSigma.hxx
namespace OT{ %extend WeibullMinMuSigma { WeibullMinMuSigma(const WeibullMinMuSigma & other) { return new OT::WeibullMinMuSigma(other); } } }


