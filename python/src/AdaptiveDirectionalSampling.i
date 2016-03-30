// SWIG file

%{
#include "openturns/AdaptiveDirectionalSampling.hxx"
%}

%include AdaptiveDirectionalSampling_doc.i

%include openturns/AdaptiveDirectionalSampling.hxx
namespace OT { %extend AdaptiveDirectionalSampling { AdaptiveDirectionalSampling(const AdaptiveDirectionalSampling & other) { return new OT::AdaptiveDirectionalSampling(other); } } }
