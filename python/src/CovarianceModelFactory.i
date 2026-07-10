// SWIG file CovarianceModelFactory.i

%{
#include "openturns/CovarianceModelFactory.hxx"
%}

%include CovarianceModelFactory_doc.i

OTTypedInterfaceObjectHelper(CovarianceModelFactory)

%copyctor OT::CovarianceModelFactory;
%include openturns/CovarianceModelFactory.hxx

