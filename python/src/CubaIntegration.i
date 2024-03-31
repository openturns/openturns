// SWIG file CuubaIntegration.i

//This is a friend function, let's refrain from wrapping it.
%ignore OT::computeIntegrand(const int *ndim, const double x[], const int *ncomp, double f[], void *userdata);

%{
#include "openturns/CubaIntegration.hxx"
%}

%include CubaIntegration_doc.i

%copyctor OT::CubaIntegration;

%include openturns/CubaIntegration.hxx
