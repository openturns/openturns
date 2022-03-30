// SWIG file HSICEstimator.i


%{
#include "openturns/HSICEstimator.hxx"
%}

%include HSICEstimator_doc.i

OTTypedInterfaceObjectHelper(HSICEstimator)

%include openturns/HSICEstimator.hxx

namespace OT {
%extend HSICEstimator {

HSICEstimator(const HSICEstimator & other) { return new OT::HSICEstimator(other); }

} }
