// SWIG file TNCSpecificParameters.i

%{
#include "TNCSpecificParameters.hxx"
%}

%include TNCSpecificParameters.hxx
namespace OT{ %extend TNCSpecificParameters { TNCSpecificParameters(const TNCSpecificParameters & other) { return new OT::TNCSpecificParameters(other); } } }
