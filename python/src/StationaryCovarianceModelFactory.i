// SWIG file StationaryCovarianceModelFactory.i

%{
#include "StationaryCovarianceModelFactory.hxx"
%}

%include StationaryCovarianceModelFactory.hxx
namespace OT { 

  %extend StationaryCovarianceModelFactory {

     StationaryCovarianceModelFactory(const StationaryCovarianceModelFactory & other)
       {
         return new OT::StationaryCovarianceModelFactory(other);
       }

   }
 }
