// SWIG file SpectralModelFactoryImplementation.i

%{
#include "SpectralModelFactoryImplementation.hxx"
%}

%include SpectralModelFactoryImplementation_doc.i

%include SpectralModelFactoryImplementation.hxx
namespace OT {%extend SpectralModelFactoryImplementation {SpectralModelFactoryImplementation(const SpectralModelFactoryImplementation & other){return new OT::SpectralModelFactoryImplementation(other);}}}
