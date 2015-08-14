// SWIG file LogNormalMuSigma.i

%{
#include "LogNormalMuSigma.hxx"
%}

%include LogNormalMuSigma_doc.i

%include LogNormalMuSigma.hxx
namespace OT{ %extend LogNormalMuSigma { LogNormalMuSigma(const LogNormalMuSigma & other) { return new OT::LogNormalMuSigma(other); } } }
