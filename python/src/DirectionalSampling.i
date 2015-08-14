// SWIG file DirectionalSampling.i

%{
#include "DirectionalSampling.hxx"
%}

%include DirectionalSampling_doc.i

%include DirectionalSampling.hxx
namespace OT{ %extend DirectionalSampling { DirectionalSampling(const DirectionalSampling & other) { return new OT::DirectionalSampling(other); } } }
