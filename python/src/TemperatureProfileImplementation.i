// SWIG file

%{
#include "openturns/TemperatureProfileImplementation.hxx"
%}

%include TemperatureProfileImplementation_doc.i

%include openturns/TemperatureProfileImplementation.hxx
namespace OT { %extend TemperatureProfileImplementation { TemperatureProfileImplementation(const TemperatureProfileImplementation & other) { return new OT::TemperatureProfileImplementation(other); } } }
