// SWIG file SpectralGaussianProcess.i

%{
#include "openturns/SpectralGaussianProcess.hxx"
%}

%include SpectralGaussianProcess_doc.i

%include openturns/SpectralGaussianProcess.hxx
namespace OT { %extend SpectralGaussianProcess { SpectralGaussianProcess(const SpectralGaussianProcess & other) { return new OT::SpectralGaussianProcess(other); } } }





