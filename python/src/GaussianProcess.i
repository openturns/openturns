// SWIG file GaussianProcess.i

%{
#include "openturns/GaussianProcess.hxx"
%}

%include GaussianProcess_doc.i

%include openturns/GaussianProcess.hxx
namespace OT { %extend GaussianProcess { GaussianProcess(const GaussianProcess & other) { return new OT::GaussianProcess(other); } } }

%pythoncode %{
# deprecated
class TemporalNormalProcess(GaussianProcess):
    def __init__(self, *args):
        super(TemporalNormalProcess, self).__init__(*args)
        openturns.common.Log.Warn('class TemporalNormalProcess is deprecated in favor of GaussianProcess')
%}

