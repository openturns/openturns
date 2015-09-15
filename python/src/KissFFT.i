// SWIG file KissFFT.i

%{
#include "KissFFT.hxx"
%}

%include KissFFT_doc.i

%include KissFFT.hxx
namespace OT { %extend KissFFT { KissFFT(const KissFFT & other) { return new OT::KissFFT(other); } } }
