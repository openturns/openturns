// SWIG file NatafEllipticalCopulaEvaluation.i

OTAutodoc(NatafEllipticalCopulaEvaluation)

%{
#include "openturns/NatafEllipticalCopulaEvaluation.hxx"
%}

%include openturns/NatafEllipticalCopulaEvaluation.hxx
namespace OT { %extend NatafEllipticalCopulaEvaluation { NatafEllipticalCopulaEvaluation(const NatafEllipticalCopulaEvaluation & other) { return new OT::NatafEllipticalCopulaEvaluation(other); } } }
