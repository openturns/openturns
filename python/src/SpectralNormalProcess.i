// SWIG file SpectralNormalProcess.i

%{
#include "SpectralNormalProcess.hxx"
%}

%include SpectralNormalProcess_doc.i

%include SpectralNormalProcess.hxx
namespace OT { %extend SpectralNormalProcess { SpectralNormalProcess(const SpectralNormalProcess & other) { return new OT::SpectralNormalProcess(other); } } }
