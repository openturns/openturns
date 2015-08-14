// SWIG file NoCalibration.i

%{
#include "NoCalibration.hxx"
%}

%include NoCalibration.hxx
namespace OT { %extend NoCalibration { NoCalibration(const NoCalibration & other) { return new OT::NoCalibration(other); } } }
