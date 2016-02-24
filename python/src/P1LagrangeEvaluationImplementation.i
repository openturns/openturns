// SWIG file P1LagrangeEvaluationImplementation.i

%{
#include "openturns/P1LagrangeEvaluationImplementation.hxx"
%}

%include P1LagrangeEvaluationImplementation_doc.i

%include openturns/P1LagrangeEvaluationImplementation.hxx
namespace OT { %extend P1LagrangeEvaluationImplementation { P1LagrangeEvaluationImplementation(const P1LagrangeEvaluationImplementation & other) { return new OT::P1LagrangeEvaluationImplementation(other); } } }
