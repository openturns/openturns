// SWIG file InverseNatafEllipticalCopulaEvaluation.i

%{
#include "InverseNatafEllipticalCopulaEvaluation.hxx"
%}

%include InverseNatafEllipticalCopulaEvaluation.hxx
namespace OT { %extend InverseNatafEllipticalCopulaEvaluation { InverseNatafEllipticalCopulaEvaluation(const InverseNatafEllipticalCopulaEvaluation & other) { return new OT::InverseNatafEllipticalCopulaEvaluation(other); } } }
