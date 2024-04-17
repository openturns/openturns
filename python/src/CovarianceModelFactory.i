// SWIG file CovarianceModelFactory.i

%{
#include "openturns/CovarianceModelFactory.hxx"
%}

%include CovarianceModelFactory_doc.i

OTTypedInterfaceObjectHelper(CovarianceModelFactory)

%include openturns/CovarianceModelFactory.hxx

%copyctor OT::CovarianceModelFactory;
