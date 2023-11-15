// SWIG file SpectralModel.i

%{
#include "openturns/SpectralModel.hxx"
%}

%include SpectralModel_doc.i

OTTypedInterfaceObjectHelper(SpectralModel)

%copyctor OT::SpectralModel;

%include openturns/SpectralModel.hxx
