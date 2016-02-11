// SWIG file SpectralNormalProcess.i

%{
#include "openturns/SpectralNormalProcess.hxx"
%}

%include SpectralNormalProcess_doc.i

%include openturns/SpectralNormalProcess.hxx
namespace OT { %extend SpectralNormalProcess { SpectralNormalProcess(const SpectralNormalProcess & other) { return new OT::SpectralNormalProcess(other); } } }
