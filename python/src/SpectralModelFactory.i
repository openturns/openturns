// SWIG file SpectralModelFactory.i

%{
#include "openturns/SpectralModelFactory.hxx"
%}

%include SpectralModelFactory_doc.i

OTTypedInterfaceObjectHelper(SpectralModelFactory)

%include openturns/SpectralModelFactory.hxx

%copyctor OT::SpectralModelFactory;
