// SWIG file ConditionedGaussianProcess.i

%{
#include "openturns/ConditionedGaussianProcess.hxx"
%}

%include ConditionedGaussianProcess_doc.i

%include openturns/ConditionedGaussianProcess.hxx
namespace OT { %extend ConditionedGaussianProcess { ConditionedGaussianProcess(const ConditionedGaussianProcess & other) { return new OT::ConditionedGaussianProcess(other); } } }

