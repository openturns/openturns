// SWIG file CovarianceModelFactoryImplementation.i

%{
#include "CovarianceModelFactoryImplementation.hxx"
%}

%include CovarianceModelFactoryImplementation.hxx
namespace OT {%extend CovarianceModelFactoryImplementation {CovarianceModelFactoryImplementation(const CovarianceModelFactoryImplementation & other){return new OT::CovarianceModelFactoryImplementation(other);}}}
