// SWIG file GumbelMuSigma.i

%{
#include "openturns/GumbelMuSigma.hxx"
%}

%include GumbelMuSigma_doc.i

%include openturns/GumbelMuSigma.hxx
namespace OT{ %extend GumbelMuSigma { GumbelMuSigma(const GumbelMuSigma & other) { return new OT::GumbelMuSigma(other); } } }
