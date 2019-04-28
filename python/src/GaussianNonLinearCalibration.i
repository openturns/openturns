// SWIG file GaussianNonLinearCalibration.i

%{
#include "openturns/GaussianNonLinearCalibration.hxx"
%}

%include GaussianNonLinearCalibration_doc.i

%include openturns/GaussianNonLinearCalibration.hxx
namespace OT { %extend GaussianNonLinearCalibration { GaussianNonLinearCalibration(const GaussianNonLinearCalibration & other) { return new OT::GaussianNonLinearCalibration(other); } } }
