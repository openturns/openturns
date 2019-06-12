// SWIG file LogNormalMuErrorFactor.i

%{
#include "openturns/LogNormalMuErrorFactor.hxx"
%}

%include LogNormalMuErrorFactor_doc.i

%include openturns/LogNormalMuErrorFactor.hxx
namespace OT{ %extend LogNormalMuErrorFactor { LogNormalMuErrorFactor(const LogNormalMuErrorFactor & other) { return new OT::LogNormalMuErrorFactor(other); } } }
