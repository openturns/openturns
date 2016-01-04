// SWIG file InverseNatafEllipticalCopulaGradient.i

OTAutodoc(InverseNatafEllipticalCopulaGradient)

%{
#include "InverseNatafEllipticalCopulaGradient.hxx"
%}

%include InverseNatafEllipticalCopulaGradient.hxx
namespace OT { %extend InverseNatafEllipticalCopulaGradient { InverseNatafEllipticalCopulaGradient(const InverseNatafEllipticalCopulaGradient & other) { return new OT::InverseNatafEllipticalCopulaGradient(other); } } }
