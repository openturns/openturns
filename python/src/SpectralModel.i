// SWIG file SpectralModel.i

%{
#include "SpectralModel.hxx"
%}

OTTypedInterfaceObjectHelper(SpectralModel)

%include SpectralModel.hxx
namespace OT{ %extend SpectralModel { SpectralModel(const SpectralModel & other) { return new OT::SpectralModel(other); } } }
