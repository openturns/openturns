// SWIG file ConditionalDistribution.i

%{
#include "ConditionalDistribution.hxx"
%}

%include ConditionalDistribution_doc.i

%include ConditionalDistribution.hxx
namespace OT { %extend ConditionalDistribution { ConditionalDistribution(const ConditionalDistribution & other) { return new OT::ConditionalDistribution(other); } } }
