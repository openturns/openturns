// SWIG file ComposedDistribution.i

%{
#include "openturns/ComposedDistribution.hxx"
%}

%include ComposedDistribution_doc.i

%include openturns/ComposedDistribution.hxx
namespace OT { %extend ComposedDistribution { ComposedDistribution(const ComposedDistribution & other) { return new OT::ComposedDistribution(other); } } }
