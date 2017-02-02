// SWIG file PosteriorDistribution.i

%{
#include "openturns/PosteriorDistribution.hxx"
%}

%include PosteriorDistribution_doc.i

%include openturns/PosteriorDistribution.hxx
namespace OT { %extend PosteriorDistribution { PosteriorDistribution(const PosteriorDistribution & other) { return new OT::PosteriorDistribution(other); } } }
