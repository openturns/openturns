// SWIG file EllipticalDistribution.i

%{
#include "EllipticalDistribution.hxx"
%}

%include EllipticalDistribution.hxx
namespace OT { %extend EllipticalDistribution { EllipticalDistribution(const EllipticalDistribution & other) { return new OT::EllipticalDistribution(other); } } }
