// SWIG file CovarianceModelFactory.i

%{
#include "CovarianceModelFactory.hxx"
%}

OTTypedInterfaceObjectHelper(CovarianceModelFactory)

%include CovarianceModelFactory.hxx

namespace OT { 

  %extend CovarianceModelFactory {

     CovarianceModelFactory(const CovarianceModelFactory & other) { return new OT::CovarianceModelFactory(other); }

   }
 }
