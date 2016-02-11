// SWIG file BayesDistribution.i

%{
#include "openturns/BayesDistribution.hxx"
%}

%include BayesDistribution_doc.i

%include openturns/BayesDistribution.hxx
namespace OT { %extend BayesDistribution { BayesDistribution(const BayesDistribution & other) { return new OT::BayesDistribution(other); } } }
