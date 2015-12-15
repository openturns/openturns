// SWIG file NatafEllipticalCopulaGradient.i

OTAutodoc(NatafEllipticalCopulaGradient)

%{
#include "NatafEllipticalCopulaGradient.hxx"
%}

%include NatafEllipticalCopulaGradient.hxx
namespace OT { %extend NatafEllipticalCopulaGradient { NatafEllipticalCopulaGradient(const NatafEllipticalCopulaGradient & other) { return new OT::NatafEllipticalCopulaGradient(other); } } }
