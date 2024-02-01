// SWIG file BasisImplementation.i

%{
#include "openturns/BasisImplementation.hxx"
%}

%include BasisImplementation_doc.i

%ignore OT::BasisImplementation::operator[];

%copyctor OT::BasisImplementation;

%include openturns/BasisImplementation.hxx
