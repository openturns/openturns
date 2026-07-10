// SWIG file GaussianProcessFitter.i

%{
#include "openturns/GaussianProcessFitter.hxx"
%}

%include GaussianProcessFitter_doc.i

%copyctor OT::GaussianProcessFitter;

%ignore OT::GaussianProcessFitter::setCholeskyFactor;

%include openturns/GaussianProcessFitter.hxx

