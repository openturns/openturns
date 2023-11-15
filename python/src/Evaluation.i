// SWIG file Evaluation.i

%{
#include "openturns/Evaluation.hxx"
%}

%include Evaluation_doc.i

OTTypedInterfaceObjectHelper(Evaluation)

%copyctor OT::Evaluation;

%include openturns/Evaluation.hxx
