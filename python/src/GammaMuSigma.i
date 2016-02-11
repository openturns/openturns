// SWIG file GammaMuSigma.i

%{
#include "openturns/GammaMuSigma.hxx"
%}

%include GammaMuSigma_doc.i

%include openturns/GammaMuSigma.hxx
namespace OT{ %extend GammaMuSigma { GammaMuSigma(const GammaMuSigma & other) { return new OT::GammaMuSigma(other); } } }
