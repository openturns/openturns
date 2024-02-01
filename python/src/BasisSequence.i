// SWIG file BasisSequence.i

%{
#include "openturns/BasisSequence.hxx"
%}

%include BasisSequence_doc.i

OTTypedInterfaceObjectHelper(BasisSequence)

%copyctor OT::BasisSequence;

%include openturns/BasisSequence.hxx
