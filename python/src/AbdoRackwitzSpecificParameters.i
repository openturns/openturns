// SWIG file AbdoRackwitzSpecificParameters.i

%{
#include "AbdoRackwitzSpecificParameters.hxx"
%}

%include AbdoRackwitzSpecificParameters.hxx

namespace OT{ %extend AbdoRackwitzSpecificParameters { AbdoRackwitzSpecificParameters(const AbdoRackwitzSpecificParameters & other) { return new OT::AbdoRackwitzSpecificParameters(other); } } }
