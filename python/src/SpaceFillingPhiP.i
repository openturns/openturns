// SWIG file

%{
#include "openturns/SpaceFillingPhiP.hxx"
%}

%include SpaceFillingPhiP_doc.i

%include openturns/SpaceFillingPhiP.hxx
namespace OT { %extend SpaceFillingPhiP { SpaceFillingPhiP(const SpaceFillingPhiP & other) { return new OT::SpaceFillingPhiP(other); } } }
