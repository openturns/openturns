// SWIG file ODESolver.i

%{
#include "openturns/ODESolver.hxx"
%}

%include ODESolver_doc.i

OTTypedInterfaceObjectHelper(ODESolver)

%copyctor OT::ODESolver;

%include openturns/ODESolver.hxx
