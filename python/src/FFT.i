// SWIG file FFT.i

%{
#include "openturns/FFT.hxx"
%}

%include FFT_doc.i

OTTypedInterfaceObjectHelper(FFT)

%include openturns/FFT.hxx
namespace OT{ %extend FFT { FFT(const FFT & other) { return new OT::FFT(other); } } }
