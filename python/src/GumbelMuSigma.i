// SWIG file GumbelMuSigma.i

%{
#include "GumbelMuSigma.hxx"
%}

%include GumbelMuSigma_doc.i

%include GumbelMuSigma.hxx
namespace OT{ %extend GumbelMuSigma { GumbelMuSigma(const GumbelMuSigma & other) { return new OT::GumbelMuSigma(other); } } }
