// SWIG file ConditionalDistribution.i

%{
#include "openturns/ConditionalDistribution.hxx"
%}

%include ConditionalDistribution_doc.i

%include openturns/ConditionalDistribution.hxx
namespace OT { %extend ConditionalDistribution { ConditionalDistribution(const ConditionalDistribution & other) { return new OT::ConditionalDistribution(other); } } }
