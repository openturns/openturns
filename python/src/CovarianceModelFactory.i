// SWIG file CovarianceModelFactory.i

%{
#include "openturns/CovarianceModelFactory.hxx"
%}

%include CovarianceModelFactory_doc.i

OTTypedInterfaceObjectHelper(CovarianceModelFactory)

%include openturns/CovarianceModelFactory.hxx

namespace OT { 

  %extend CovarianceModelFactory {

     CovarianceModelFactory(const CovarianceModelFactory & other) { return new OT::CovarianceModelFactory(other); }

   }
 }
