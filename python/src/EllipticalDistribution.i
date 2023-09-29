// SWIG file EllipticalDistribution.i

%{
#include "openturns/EllipticalDistribution.hxx"
%}

%include EllipticalDistribution_doc.i

%include openturns/EllipticalDistribution.hxx
namespace OT { %extend EllipticalDistribution { EllipticalDistribution(const EllipticalDistribution & other) { return new OT::EllipticalDistribution(other); } } }
