// SWIG file BayesDistribution.i

%{
#include "BayesDistribution.hxx"
%}

%include BayesDistribution_doc.i

%include BayesDistribution.hxx
namespace OT { %extend BayesDistribution { BayesDistribution(const BayesDistribution & other) { return new OT::BayesDistribution(other); } } }
