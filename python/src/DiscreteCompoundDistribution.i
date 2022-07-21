// SWIG file DiscreteCompoundDistribution.i

%{
#include "openturns/DiscreteCompoundDistribution.hxx"
%}

%include DiscreteCompoundDistribution_doc.i

%include openturns/DiscreteCompoundDistribution.hxx
namespace OT { %extend DiscreteCompoundDistribution { DiscreteCompoundDistribution(const DiscreteCompoundDistribution & other) { return new OT::DiscreteCompoundDistribution(other); } } }
