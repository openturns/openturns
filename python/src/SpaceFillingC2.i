// SWIG file

%{
#include "openturns/SpaceFillingC2.hxx"
%}

%include SpaceFillingC2_doc.i

%include openturns/SpaceFillingC2.hxx
namespace OT { %extend SpaceFillingC2 { SpaceFillingC2(const SpaceFillingC2 & other) { return new OT::SpaceFillingC2(other); } } }
