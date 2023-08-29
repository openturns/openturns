// SWIG file TTY.i

%{
#include "openturns/TTY.hxx"
%}

%include TTY_doc.i

%ignore OT::TTY::GetColor;
%nodefaultctor TTY;

%include openturns/TTY.hxx
