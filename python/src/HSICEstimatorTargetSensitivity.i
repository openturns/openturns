// SWIG file HSICEstimatorTargetSensitivity.i


%{
#include "openturns/HSICEstimatorTargetSensitivity.hxx"
%}

%include HSICEstimatorTargetSensitivity_doc.i

%include openturns/HSICEstimatorTargetSensitivity.hxx

namespace OT {
%extend HSICEstimatorTargetSensitivity {

HSICEstimatorTargetSensitivity(const HSICEstimatorTargetSensitivity & other) { return new OT::HSICEstimatorTargetSensitivity(other); }

} }
