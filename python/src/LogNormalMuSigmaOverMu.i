// SWIG file LogNormalMuSigmaOverMu.i

%{
#include "LogNormalMuSigmaOverMu.hxx"
%}

%include LogNormalMuSigmaOverMu_doc.i

%include LogNormalMuSigmaOverMu.hxx
namespace OT{ %extend LogNormalMuSigmaOverMu { LogNormalMuSigmaOverMu(const LogNormalMuSigmaOverMu & other) { return new OT::LogNormalMuSigmaOverMu(other); } } }
