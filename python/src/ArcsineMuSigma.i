// SWIG file ArcsineMuSigma.i

%{
#include "openturns/ArcsineMuSigma.hxx"
%}

%include ArcsineMuSigma_doc.i

%include openturns/ArcsineMuSigma.hxx
namespace OT{ %extend ArcsineMuSigma { ArcsineMuSigma(const ArcsineMuSigma & other) { return new OT::ArcsineMuSigma(other); } } }
