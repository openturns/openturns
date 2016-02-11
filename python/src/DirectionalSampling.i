// SWIG file DirectionalSampling.i

%{
#include "openturns/DirectionalSampling.hxx"
%}

%include DirectionalSampling_doc.i

%include openturns/DirectionalSampling.hxx
namespace OT{ %extend DirectionalSampling { DirectionalSampling(const DirectionalSampling & other) { return new OT::DirectionalSampling(other); } } }
