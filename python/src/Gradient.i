// SWIG file Gradient.i

%{
#include "openturns/Gradient.hxx"
%}

%include Gradient_doc.i

OTTypedInterfaceObjectHelper(Gradient)

%copyctor OT::Gradient;

%include openturns/Gradient.hxx
