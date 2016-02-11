// SWIG file CompositeDistribution.i

%{
#include "openturns/CompositeDistribution.hxx"
%}

%include CompositeDistribution_doc.i

%include openturns/CompositeDistribution.hxx
namespace OT { %extend CompositeDistribution { CompositeDistribution(const CompositeDistribution & other) { return new OT::CompositeDistribution(other); } } }
