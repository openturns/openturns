// SWIG file ParametrizedDistribution.i

%{
#include "ParametrizedDistribution.hxx"
%}

%include ParametrizedDistribution_doc.i

%include ParametrizedDistribution.hxx
namespace OT{ %extend ParametrizedDistribution { ParametrizedDistribution(const ParametrizedDistribution & other) { return new OT::ParametrizedDistribution(other); } } }
