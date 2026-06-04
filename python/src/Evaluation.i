// SWIG file Evaluation.i

%{
#include "openturns/Evaluation.hxx"
%}

%include Evaluation_doc.i

OTTypedInterfaceObjectHelper(Evaluation)

%copyctor OT::Evaluation;

%ignore OT::Evaluation::setDetachCallback;

%include openturns/Evaluation.hxx
