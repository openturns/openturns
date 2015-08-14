// SWIG file WeibullMuSigma.i

%{
#include "WeibullMuSigma.hxx"
%}

%include WeibullMuSigma_doc.i

%include WeibullMuSigma.hxx
namespace OT{ %extend WeibullMuSigma { WeibullMuSigma(const WeibullMuSigma & other) { return new OT::WeibullMuSigma(other); } } }
