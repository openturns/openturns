// SWIG file

%{
#include "openturns/TemperatureProfile.hxx"
%}

%include TemperatureProfile_doc.i

OTTypedInterfaceObjectHelper(TemperatureProfile)

%include openturns/TemperatureProfile.hxx
namespace OT { %extend TemperatureProfile { TemperatureProfile(const TemperatureProfile & other) { return new OT::TemperatureProfile(other); } } }
