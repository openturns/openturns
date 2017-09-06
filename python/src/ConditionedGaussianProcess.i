// SWIG file ConditionedGaussianProcess.i

%{
#include "openturns/ConditionedGaussianProcess.hxx"
%}

%include ConditionedGaussianProcess_doc.i

%include openturns/ConditionedGaussianProcess.hxx
namespace OT { %extend ConditionedGaussianProcess { ConditionedGaussianProcess(const ConditionedGaussianProcess & other) { return new OT::ConditionedGaussianProcess(other); } } }

%pythoncode %{
class ConditionedNormalProcess(ConditionedGaussianProcess):
    def __init__(self, *args):
        super(ConditionedNormalProcess, self).__init__(*args)
        openturns.common.Log.Warn('class ConditionedNormalProcess is deprecated in favor of ConditionedGaussianProcess')
%}

