// SWIG file FFT.i

%{
#include "FFT.hxx"
%}

%include FFT_doc.i

OTTypedInterfaceObjectHelper(FFT)

%include FFT.hxx
namespace OT{ %extend FFT { FFT(const FFT & other) { return new OT::FFT(other); } } }
