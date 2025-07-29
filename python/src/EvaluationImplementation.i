// SWIG file EvaluationImplementation.i

%{
#include "openturns/EvaluationImplementation.hxx"
%}

%include EvaluationImplementation_doc.i

%ignore OT::EvaluationImplementation::isParallel;
%ignore OT::EvaluationImplementation::setDetachCallback;

%copyctor OT::EvaluationImplementation;

%include openturns/EvaluationImplementation.hxx
