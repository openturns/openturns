// SWIG file GaussianProcessFitter.i

%{
#include "openturns/GaussianProcessFitter.hxx"
%}

%include GaussianProcessFitter_doc.i

%ignore OT::GaussianProcessFitter::setCholeskyFactor;

%include openturns/GaussianProcessFitter.hxx

namespace OT{ %extend GaussianProcessFitter { GaussianProcessFitter(const GaussianProcessFitter & other) { return new OT::GaussianProcessFitter(other); } } }

