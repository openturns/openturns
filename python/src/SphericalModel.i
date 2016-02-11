// SWIG file SphericalModel.i

%{
#include "openturns/SphericalModel.hxx"
%}

%include SphericalModel_doc.i

%include openturns/SphericalModel.hxx
namespace OT { %extend SphericalModel { SphericalModel(const SphericalModel & other) { return new OT::SphericalModel(other); } } }
