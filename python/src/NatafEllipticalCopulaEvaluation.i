// SWIG file NatafEllipticalCopulaEvaluation.i

OTAutodoc(NatafEllipticalCopulaEvaluation)

%{
#include "NatafEllipticalCopulaEvaluation.hxx"
%}

%include NatafEllipticalCopulaEvaluation.hxx
namespace OT { %extend NatafEllipticalCopulaEvaluation { NatafEllipticalCopulaEvaluation(const NatafEllipticalCopulaEvaluation & other) { return new OT::NatafEllipticalCopulaEvaluation(other); } } }
