// SWIG file SpectralModel.i

%{
#include "SpectralModel.hxx"
%}

%include SpectralModel_doc.i

OTTypedInterfaceObjectHelper(SpectralModel)

%include SpectralModel.hxx
namespace OT{ %extend SpectralModel { SpectralModel(const SpectralModel & other) { return new OT::SpectralModel(other); } } }
