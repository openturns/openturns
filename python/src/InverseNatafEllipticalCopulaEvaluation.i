// SWIG file InverseNatafEllipticalCopulaEvaluation.i

OTAutodoc(InverseNatafEllipticalCopulaEvaluation)

%{
#include "openturns/InverseNatafEllipticalCopulaEvaluation.hxx"
%}

%include openturns/InverseNatafEllipticalCopulaEvaluation.hxx
namespace OT { %extend InverseNatafEllipticalCopulaEvaluation { InverseNatafEllipticalCopulaEvaluation(const InverseNatafEllipticalCopulaEvaluation & other) { return new OT::InverseNatafEllipticalCopulaEvaluation(other); } } }
