// SWIG file HSICEstimatorGlobalSensitivity.i


%{
#include "openturns/HSICEstimatorGlobalSensitivity.hxx"
%}

%include HSICEstimatorGlobalSensitivity_doc.i

%include openturns/HSICEstimatorGlobalSensitivity.hxx

namespace OT {
%extend HSICEstimatorGlobalSensitivity {

HSICEstimatorGlobalSensitivity(const HSICEstimatorGlobalSensitivity & other) { return new OT::HSICEstimatorGlobalSensitivity(other); }

} }
