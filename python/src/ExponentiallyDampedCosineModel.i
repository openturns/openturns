// SWIG file ExponentiallyDampedCosineModel.i

%{
#include "openturns/ExponentiallyDampedCosineModel.hxx"
%}

%include ExponentiallyDampedCosineModel_doc.i

%include openturns/ExponentiallyDampedCosineModel.hxx
namespace OT { %extend ExponentiallyDampedCosineModel { ExponentiallyDampedCosineModel(const ExponentiallyDampedCosineModel & other) { return new OT::ExponentiallyDampedCosineModel(other); } } }
