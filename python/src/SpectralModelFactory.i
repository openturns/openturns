// SWIG file SpectralModelFactory.i

%{
#include "openturns/SpectralModelFactory.hxx"
%}

%include SpectralModelFactory_doc.i

OTTypedInterfaceObjectHelper(SpectralModelFactory)

%copyctor OT::SpectralModelFactory;
%include openturns/SpectralModelFactory.hxx

