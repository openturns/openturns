// SWIG file

%{
#include "openturns/TemperatureProfileImplementation.hxx"
%}

%include TemperatureProfileImplementation_doc.i

%template(TemperatureProfileImplementationdInterfaceObject)           OT::TypedInterfaceObject<OT::TemperatureProfileImplementation>;

%include openturns/TemperatureProfileImplementation.hxx
namespace OT { %extend TemperatureProfileImplementation { TemperatureProfileImplementation(const TemperatureProfileImplementation & other) { return new OT::TemperatureProfileImplementation(other); } } }
