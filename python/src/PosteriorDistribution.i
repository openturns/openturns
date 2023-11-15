// SWIG file PosteriorDistribution.i

%{
#include "openturns/PosteriorDistribution.hxx"
%}

%include PosteriorDistribution_doc.i

%ignore OT::PosteriorDistribution::computeLikelihood;
%ignore OT::PosteriorDistribution::computeLogLikelihood;

%copyctor OT::PosteriorDistribution;

%include openturns/PosteriorDistribution.hxx
