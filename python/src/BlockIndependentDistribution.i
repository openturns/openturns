// SWIG file BlockIndependentDistribution.i

%{
#include "openturns/BlockIndependentDistribution.hxx"
%}

%include BlockIndependentDistribution_doc.i

%include openturns/BlockIndependentDistribution.hxx
namespace OT { %extend BlockIndependentDistribution { BlockIndependentDistribution(const BlockIndependentDistribution & other) { return new OT::BlockIndependentDistribution(other); } } }
