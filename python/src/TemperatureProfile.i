// SWIG file

%{
#include "openturns/TemperatureProfile.hxx"
%}

%include TemperatureProfile_doc.i

OTTypedInterfaceObjectHelper(TemperatureProfile)

%copyctor OT::TemperatureProfile;

%include openturns/TemperatureProfile.hxx
