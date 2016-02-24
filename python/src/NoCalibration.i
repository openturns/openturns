// SWIG file NoCalibration.i

%{
#include "openturns/NoCalibration.hxx"
%}

%include openturns/NoCalibration.hxx
namespace OT { %extend NoCalibration { NoCalibration(const NoCalibration & other) { return new OT::NoCalibration(other); } } }
