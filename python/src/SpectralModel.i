// SWIG file SpectralModel.i

%{
#include "openturns/SpectralModel.hxx"
%}

%include SpectralModel_doc.i

OTTypedInterfaceObjectHelper(SpectralModel)

%include openturns/SpectralModel.hxx
namespace OT{ %extend SpectralModel { SpectralModel(const SpectralModel & other) { return new OT::SpectralModel(other); } } }
