// SWIG file StationaryCovarianceModelFactory.i

%{
#include "openturns/StationaryCovarianceModelFactory.hxx"
%}

%include StationaryCovarianceModelFactory_doc.i

%include openturns/StationaryCovarianceModelFactory.hxx
namespace OT { 

  %extend StationaryCovarianceModelFactory {

     StationaryCovarianceModelFactory(const StationaryCovarianceModelFactory & other)
       {
         return new OT::StationaryCovarianceModelFactory(other);
       }

   }
 }
