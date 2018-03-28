// SWIG file P1LagrangeInterpolation.i

%{
#include "openturns/P1LagrangeInterpolation.hxx"
%}

%include P1LagrangeInterpolation_doc.i

%include openturns/P1LagrangeInterpolation.hxx
namespace OT { %extend P1LagrangeInterpolation { P1LagrangeInterpolation(const P1LagrangeInterpolation & other) { return new OT::P1LagrangeInterpolation(other); } } }
