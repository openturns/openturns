// SWIG file FiniteBasis.i

%{
#include "openturns/FiniteBasis.hxx"
%}

%ignore OT::FiniteBasis::operator[];

%copyctor OT::FiniteBasis;

%include openturns/FiniteBasis.hxx
