// SWIG file ExponentiallyDampedCosineModel.i

%{
#include "ExponentiallyDampedCosineModel.hxx"
%}

%include ExponentiallyDampedCosineModel.hxx
namespace OT { %extend ExponentiallyDampedCosineModel { ExponentiallyDampedCosineModel(const ExponentiallyDampedCosineModel & other) { return new OT::ExponentiallyDampedCosineModel(other); } } }
