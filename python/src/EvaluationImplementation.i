// SWIG file EvaluationImplementation.i

%{
#include "openturns/EvaluationImplementation.hxx"
%}

%include EvaluationImplementation_doc.i

%ignore OT::EvaluationImplementation::isParallel;

%include openturns/EvaluationImplementation.hxx
namespace OT { %extend EvaluationImplementation { EvaluationImplementation(const EvaluationImplementation & other) { return new OT::EvaluationImplementation(other); } } }
