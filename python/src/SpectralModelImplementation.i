// SWIG file SpectralModelImplementation.i

%{
#include "openturns/SpectralModelImplementation.hxx"
%}

%include SpectralModelImplementation_doc.i

%include openturns/SpectralModelImplementation.hxx
namespace OT{ %extend SpectralModelImplementation { SpectralModelImplementation(const SpectralModelImplementation & other) { return new OT::SpectralModelImplementation(other); } } }
