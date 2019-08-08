// SWIG file LinearModelStepwiseAlgorithm.i

%{
#include "openturns/LinearModelStepwiseAlgorithm.hxx"
%}

%include LinearModelStepwiseAlgorithm_doc.i

%include openturns/LinearModelStepwiseAlgorithm.hxx
namespace OT { %extend LinearModelStepwiseAlgorithm { LinearModelStepwiseAlgorithm(const LinearModelStepwiseAlgorithm & other) { return new OT::LinearModelStepwiseAlgorithm(other); } } }
