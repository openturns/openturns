// SWIG file HSICEstimatorConditionalSensitivity.i


%{
#include "openturns/HSICEstimatorConditionalSensitivity.hxx"
%}

%include HSICEstimatorConditionalSensitivity_doc.i

%include openturns/HSICEstimatorConditionalSensitivity.hxx

namespace OT {
%extend HSICEstimatorConditionalSensitivity {

HSICEstimatorConditionalSensitivity(const HSICEstimatorConditionalSensitivity & other) { return new OT::HSICEstimatorConditionalSensitivity(other); }

} }
