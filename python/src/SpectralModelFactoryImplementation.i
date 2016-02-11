// SWIG file SpectralModelFactoryImplementation.i

%{
#include "openturns/SpectralModelFactoryImplementation.hxx"
%}

%include SpectralModelFactoryImplementation_doc.i

%include openturns/SpectralModelFactoryImplementation.hxx
namespace OT {%extend SpectralModelFactoryImplementation {SpectralModelFactoryImplementation(const SpectralModelFactoryImplementation & other){return new OT::SpectralModelFactoryImplementation(other);}}}
