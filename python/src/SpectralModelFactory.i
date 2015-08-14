// SWIG file SpectralModelFactory.i

%{
#include "SpectralModelFactory.hxx"
%}

%include SpectralModelFactory_doc.i

OTTypedInterfaceObjectHelper(SpectralModelFactory)

%include SpectralModelFactory.hxx

namespace OT { 

  %extend SpectralModelFactory {

     SpectralModelFactory(const SpectralModelFactory & other) { return new OT::SpectralModelFactory(other); }

   }
 }
