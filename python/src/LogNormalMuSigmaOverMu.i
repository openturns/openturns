// SWIG file LogNormalMuSigmaOverMu.i

%{
#include "openturns/LogNormalMuSigmaOverMu.hxx"
%}

%include LogNormalMuSigmaOverMu_doc.i

%include openturns/LogNormalMuSigmaOverMu.hxx
namespace OT{ %extend LogNormalMuSigmaOverMu { LogNormalMuSigmaOverMu(const LogNormalMuSigmaOverMu & other) { return new OT::LogNormalMuSigmaOverMu(other); } } }
