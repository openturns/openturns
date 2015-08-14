// SWIG file SphericalModel.i

%{
#include "SphericalModel.hxx"
%}

%include SphericalModel.hxx
namespace OT { %extend SphericalModel { SphericalModel(const SphericalModel & other) { return new OT::SphericalModel(other); } } }
