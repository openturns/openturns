// SWIG file NatafEllipticalCopulaGradient.i

OTAutodoc(NatafEllipticalCopulaGradient)

%{
#include "openturns/NatafEllipticalCopulaGradient.hxx"
%}

%include openturns/NatafEllipticalCopulaGradient.hxx
namespace OT { %extend NatafEllipticalCopulaGradient { NatafEllipticalCopulaGradient(const NatafEllipticalCopulaGradient & other) { return new OT::NatafEllipticalCopulaGradient(other); } } }
