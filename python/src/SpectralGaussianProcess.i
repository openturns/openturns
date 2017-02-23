// SWIG file SpectralGaussianProcess.i

%{
#include "openturns/SpectralGaussianProcess.hxx"
%}

%include SpectralGaussianProcess_doc.i

%include openturns/SpectralGaussianProcess.hxx
namespace OT { %extend SpectralGaussianProcess { SpectralGaussianProcess(const SpectralGaussianProcess & other) { return new OT::SpectralGaussianProcess(other); } } }

%pythoncode %{
# deprecated
class SpectralNormalProcess(SpectralGaussianProcess):
    def __init__(self, *args):
        super(SpectralNormalProcess, self).__init__(*args)
        openturns.common.Log.Warn('class SpectralNormalProcess is deprecated in favor of SpectralGaussianProcess')
%}





