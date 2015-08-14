// SWIG file ArcsineMuSigma.i

%{
#include "ArcsineMuSigma.hxx"
%}

%include ArcsineMuSigma_doc.i

%include ArcsineMuSigma.hxx
namespace OT{ %extend ArcsineMuSigma { ArcsineMuSigma(const ArcsineMuSigma & other) { return new OT::ArcsineMuSigma(other); } } }
