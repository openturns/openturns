// SWIG file WeibullMaxMuSigma.i

%{
#include "openturns/WeibullMaxMuSigma.hxx"
%}

%include WeibullMaxMuSigma_doc.i

%include openturns/WeibullMaxMuSigma.hxx
namespace OT{ %extend WeibullMaxMuSigma { WeibullMaxMuSigma(const WeibullMaxMuSigma & other) { return new OT::WeibullMaxMuSigma(other); } } }

