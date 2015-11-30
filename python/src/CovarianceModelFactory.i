// SWIG file CovarianceModelFactory.i

%{
#include "CovarianceModelFactory.hxx"
%}

%include CovarianceModelFactory_doc.i

OTTypedInterfaceObjectHelper(CovarianceModelFactory)

%include CovarianceModelFactory.hxx

namespace OT { 

  %extend CovarianceModelFactory {

     CovarianceModelFactory(const CovarianceModelFactory & other) { return new OT::CovarianceModelFactory(other); }

   }
 }
