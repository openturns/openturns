// SWIG file NonStationaryCovarianceModelFactory.i

%{
#include "openturns/NonStationaryCovarianceModelFactory.hxx"
%}

%include NonStationaryCovarianceModelFactory_doc.i

%include openturns/NonStationaryCovarianceModelFactory.hxx
namespace OT { 

  %extend NonStationaryCovarianceModelFactory {

     NonStationaryCovarianceModelFactory(const NonStationaryCovarianceModelFactory & other)
       {
         return new OT::NonStationaryCovarianceModelFactory(other);
       }

   }
 }
