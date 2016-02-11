// SWIG file InverseNatafEllipticalCopulaGradient.i

OTAutodoc(InverseNatafEllipticalCopulaGradient)

%{
#include "openturns/InverseNatafEllipticalCopulaGradient.hxx"
%}

%include openturns/InverseNatafEllipticalCopulaGradient.hxx
namespace OT { %extend InverseNatafEllipticalCopulaGradient { InverseNatafEllipticalCopulaGradient(const InverseNatafEllipticalCopulaGradient & other) { return new OT::InverseNatafEllipticalCopulaGradient(other); } } }
