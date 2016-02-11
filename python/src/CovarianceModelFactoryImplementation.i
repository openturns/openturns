// SWIG file CovarianceModelFactoryImplementation.i

%{
#include "openturns/CovarianceModelFactoryImplementation.hxx"
%}

%include CovarianceModelFactoryImplementation_doc.i

%include openturns/CovarianceModelFactoryImplementation.hxx
namespace OT {%extend CovarianceModelFactoryImplementation {CovarianceModelFactoryImplementation(const CovarianceModelFactoryImplementation & other){return new OT::CovarianceModelFactoryImplementation(other);}}}
