// SWIG file P1LagrangeEvaluation.i

%{
#include "openturns/P1LagrangeEvaluation.hxx"
%}

%include P1LagrangeEvaluation_doc.i

%include openturns/P1LagrangeEvaluation.hxx
namespace OT { %extend P1LagrangeEvaluation { P1LagrangeEvaluation(const P1LagrangeEvaluation & other) { return new OT::P1LagrangeEvaluation(other); } } }
