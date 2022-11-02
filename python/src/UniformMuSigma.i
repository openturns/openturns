// SWIG file UniformMuSigma.i

%{
#include "openturns/UniformMuSigma.hxx"
%}

%include UniformMuSigma_doc.i

%include openturns/UniformMuSigma.hxx
namespace OT{ %extend UniformMuSigma { UniformMuSigma(const UniformMuSigma & other) { return new OT::UniformMuSigma(other); } } }
