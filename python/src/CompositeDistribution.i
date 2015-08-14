// SWIG file CompositeDistribution.i

%{
#include "CompositeDistribution.hxx"
%}

%include CompositeDistribution_doc.i

%include CompositeDistribution.hxx
namespace OT { %extend CompositeDistribution { CompositeDistribution(const CompositeDistribution & other) { return new OT::CompositeDistribution(other); } } }
