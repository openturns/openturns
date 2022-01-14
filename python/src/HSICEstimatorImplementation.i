// SWIG file HSICEstimatorImplementation.i

%include HSICEstimatorImplementation_doc.i

%include openturns/HSICEstimatorImplementation.hxx

%template(CovarianceModelCollection) OT::PersistentCollection<OT::CovarianceModel>;

namespace OT {
%extend HSICEstimatorImplementation {

HSICEstimatorImplementation(const HSICEstimatorImplementation & other) { return new OT::HSICEstimatorImplementation(other); }

} }
