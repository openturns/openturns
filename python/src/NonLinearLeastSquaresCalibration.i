// SWIG file NonLinearLeastSquaresCalibration.i

%{
#include "openturns/NonLinearLeastSquaresCalibration.hxx"
%}

%include NonLinearLeastSquaresCalibration_doc.i

%include openturns/NonLinearLeastSquaresCalibration.hxx
namespace OT { %extend NonLinearLeastSquaresCalibration { NonLinearLeastSquaresCalibration(const NonLinearLeastSquaresCalibration & other) { return new OT::NonLinearLeastSquaresCalibration(other); } } }
