// SWIG file LogNormalMuSigma.i

%{
#include "openturns/LogNormalMuSigma.hxx"
%}

%include LogNormalMuSigma_doc.i

%include openturns/LogNormalMuSigma.hxx
namespace OT{ %extend LogNormalMuSigma { LogNormalMuSigma(const LogNormalMuSigma & other) { return new OT::LogNormalMuSigma(other); } } }
