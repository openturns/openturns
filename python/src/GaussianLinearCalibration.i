// SWIG file GaussianLinearCalibration.i

%{
#include "openturns/GaussianLinearCalibration.hxx"
%}

%include GaussianLinearCalibration_doc.i

%include openturns/GaussianLinearCalibration.hxx
namespace OT { %extend GaussianLinearCalibration { GaussianLinearCalibration(const GaussianLinearCalibration & other) { return new OT::GaussianLinearCalibration(other); } } }
