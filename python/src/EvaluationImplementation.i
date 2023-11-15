// SWIG file EvaluationImplementation.i

%{
#include "openturns/EvaluationImplementation.hxx"
%}

%include EvaluationImplementation_doc.i

%ignore OT::EvaluationImplementation::isParallel;

%copyctor OT::EvaluationImplementation;

%include openturns/EvaluationImplementation.hxx
