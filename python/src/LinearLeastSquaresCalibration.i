// SWIG file LinearLeastSquaresCalibration.i

%{
#include "openturns/LinearLeastSquaresCalibration.hxx"
%}

%include LinearLeastSquaresCalibration_doc.i

%include openturns/LinearLeastSquaresCalibration.hxx
namespace OT { %extend LinearLeastSquaresCalibration { LinearLeastSquaresCalibration(const LinearLeastSquaresCalibration & other) { return new OT::LinearLeastSquaresCalibration(other); } } }
