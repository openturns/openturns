// SWIG file LinearLeastSquaresMethodImplementation.i

%{
#include "openturns/LinearLeastSquaresMethodImplementation.hxx"
%}

%include LinearLeastSquaresMethodImplementation_doc.i

%copyctor OT::LinearLeastSquaresMethodImplementation;

%include openturns/LinearLeastSquaresMethodImplementation.hxx

%pythoncode %{
# Deprecated alias kept for backward compatibility, see issue #3254
LeastSquaresMethodImplementation = LinearLeastSquaresMethodImplementation
%}
