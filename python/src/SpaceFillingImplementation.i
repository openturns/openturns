// SWIG file

%{
#include "openturns/SpaceFillingImplementation.hxx"
%}

%include SpaceFillingImplementation_doc.i

%include openturns/SpaceFillingImplementation.hxx
namespace OT { %extend SpaceFillingImplementation { SpaceFillingImplementation(const SpaceFillingImplementation & other) { return new OT::SpaceFillingImplementation(other); } } }
