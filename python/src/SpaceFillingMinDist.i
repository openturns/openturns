// SWIG file

%{
#include "openturns/SpaceFillingMinDist.hxx"
%}

%include SpaceFillingMinDist_doc.i

%include openturns/SpaceFillingMinDist.hxx
namespace OT { %extend SpaceFillingMinDist { SpaceFillingMinDist(const SpaceFillingMinDist & other) { return new OT::SpaceFillingMinDist(other); } } }
