// SWIG file ParametrizedDistribution.i

%{
#include "openturns/ParametrizedDistribution.hxx"
%}

%include ParametrizedDistribution_doc.i

%include openturns/ParametrizedDistribution.hxx
namespace OT{ %extend ParametrizedDistribution { ParametrizedDistribution(const ParametrizedDistribution & other) { return new OT::ParametrizedDistribution(other); } } }
