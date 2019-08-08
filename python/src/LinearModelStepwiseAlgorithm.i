// SWIG file LinearModelStepwiseAlgorithm.i

%{
#include "otlm/LinearModelStepwiseAlgorithm.hxx"
%}

%include LinearModelStepwiseAlgorithm_doc.i

%include otlm/LinearModelStepwiseAlgorithm.hxx
namespace OTLM { %extend LinearModelStepwiseAlgorithm { LinearModelStepwiseAlgorithm(const LinearModelStepwiseAlgorithm & other) { return new OTLM::LinearModelStepwiseAlgorithm(other); } } }
