// SWIG file CovarianceModelFactoryImplementation.i

%{
#include "CovarianceModelFactoryImplementation.hxx"
%}

%include CovarianceModelFactoryImplementation_doc.i

%include CovarianceModelFactoryImplementation.hxx
namespace OT {%extend CovarianceModelFactoryImplementation {CovarianceModelFactoryImplementation(const CovarianceModelFactoryImplementation & other){return new OT::CovarianceModelFactoryImplementation(other);}}}
