// SWIG file EllipticalDistribution.i

%{
#include "openturns/EllipticalDistribution.hxx"
%}

%include openturns/EllipticalDistribution.hxx
namespace OT { %extend EllipticalDistribution { EllipticalDistribution(const EllipticalDistribution & other) { return new OT::EllipticalDistribution(other); } } }
