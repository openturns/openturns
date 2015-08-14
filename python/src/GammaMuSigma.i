// SWIG file GammaMuSigma.i

%{
#include "GammaMuSigma.hxx"
%}

%include GammaMuSigma_doc.i

%include GammaMuSigma.hxx
namespace OT{ %extend GammaMuSigma { GammaMuSigma(const GammaMuSigma & other) { return new OT::GammaMuSigma(other); } } }
