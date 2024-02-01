// SWIG file FFT.i

%{
#include "openturns/FFT.hxx"
%}

%include FFT_doc.i

OTTypedInterfaceObjectHelper(FFT)

%copyctor OT::FFT;

%include openturns/FFT.hxx
