// SWIG file WeibullMuSigma.i

%{
#include "openturns/WeibullMuSigma.hxx"
%}

%include WeibullMuSigma_doc.i

%include openturns/WeibullMuSigma.hxx
namespace OT{ %extend WeibullMuSigma { WeibullMuSigma(const WeibullMuSigma & other) { return new OT::WeibullMuSigma(other); } } }
