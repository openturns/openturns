// SWIG file NatafEllipticalCopulaGradient.i

%{
#include "NatafEllipticalCopulaGradient.hxx"
%}

%include NatafEllipticalCopulaGradient.hxx
namespace OT { %extend NatafEllipticalCopulaGradient { NatafEllipticalCopulaGradient(const NatafEllipticalCopulaGradient & other) { return new OT::NatafEllipticalCopulaGradient(other); } } }
