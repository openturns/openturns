// SWIG file MaximumDistribution.i

%{
#include "openturns/MaximumDistribution.hxx"
%}
%include MaximumDistribution_doc.i

%include openturns/MaximumDistribution.hxx
namespace OT { %extend MaximumDistribution { MaximumDistribution(const MaximumDistribution & other) { return new OT::MaximumDistribution(other); } } }
