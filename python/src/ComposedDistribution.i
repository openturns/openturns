// SWIG file ComposedDistribution.i

%{
#include "ComposedDistribution.hxx"
%}

%include ComposedDistribution_doc.i

%include ComposedDistribution.hxx
namespace OT { %extend ComposedDistribution { ComposedDistribution(const ComposedDistribution & other) { return new OT::ComposedDistribution(other); } } }
