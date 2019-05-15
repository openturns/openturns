// SWIG file PInverseDistanceWeightingInterpolation.i

%{
#include "openturns/InverseDistanceWeightingInterpolation.hxx"
%}

%include InverseDistanceWeightingInterpolation_doc.i

%include openturns/InverseDistanceWeightingInterpolation.hxx
namespace OT { %extend InverseDistanceWeightingInterpolation { InverseDistanceWeightingInterpolation(const InverseDistanceWeightingInterpolation & other) { return new OT::InverseDistanceWeightingInterpolation(other); } } }
