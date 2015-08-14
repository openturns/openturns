// SWIG file CobylaSpecificParameters.i

%{
#include "CobylaSpecificParameters.hxx"
%}

%include CobylaSpecificParameters.hxx
namespace OT{ %extend CobylaSpecificParameters { CobylaSpecificParameters(const CobylaSpecificParameters & other) { return new OT::CobylaSpecificParameters(other); } } }
