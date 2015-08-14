// SWIG file PosteriorDistribution.i

%{
#include "PosteriorDistribution.hxx"
%}

%include PosteriorDistribution.hxx
namespace OT { %extend PosteriorDistribution { PosteriorDistribution(const PosteriorDistribution & other) { return new OT::PosteriorDistribution(other); } } }
