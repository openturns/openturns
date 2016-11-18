// SWIG file

%{
#include "openturns/SpaceFillingImplementation.hxx"
%}

%include SpaceFillingImplementation_doc.i

%template(SpaceFillingImplementationdInterfaceObject)           OT::TypedInterfaceObject<OT::SpaceFillingImplementation>;

%include openturns/SpaceFillingImplementation.hxx
namespace OT { %extend SpaceFillingImplementation { SpaceFillingImplementation(const SpaceFillingImplementation & other) { return new OT::SpaceFillingImplementation(other); } } }
