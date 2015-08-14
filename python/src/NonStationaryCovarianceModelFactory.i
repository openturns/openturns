// SWIG file NonStationaryCovarianceModelFactory.i

%{
#include "NonStationaryCovarianceModelFactory.hxx"
%}

%include NonStationaryCovarianceModelFactory.hxx
namespace OT { 

  %extend NonStationaryCovarianceModelFactory {

     NonStationaryCovarianceModelFactory(const NonStationaryCovarianceModelFactory & other)
       {
         return new OT::NonStationaryCovarianceModelFactory(other);
       }

   }
 }
