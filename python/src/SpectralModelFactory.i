// SWIG file SpectralModelFactory.i

%{
#include "openturns/SpectralModelFactory.hxx"
%}

%include SpectralModelFactory_doc.i

OTTypedInterfaceObjectHelper(SpectralModelFactory)

%include openturns/SpectralModelFactory.hxx

namespace OT { 

  %extend SpectralModelFactory {

     SpectralModelFactory(const SpectralModelFactory & other) { return new OT::SpectralModelFactory(other); }

   }
 }
