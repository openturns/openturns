// SWIG file HSICEstimatorImplementation.i

%include HSICEstimatorImplementation_doc.i

%include openturns/HSICEstimatorImplementation.hxx

namespace OT {
%extend HSICEstimatorImplementation {

HSICEstimatorImplementation(const HSICEstimatorImplementation & other) { return new OT::HSICEstimatorImplementation(other); }

} }
