// SWIG file KissFFT.i

%{
#include "openturns/KissFFT.hxx"
%}

%include KissFFT_doc.i

%include openturns/KissFFT.hxx
namespace OT { %extend KissFFT { KissFFT(const KissFFT & other) { return new OT::KissFFT(other); } } }
