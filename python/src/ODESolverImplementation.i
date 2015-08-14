// SWIG file ODESolverImplementation.i

%{
#include "ODESolverImplementation.hxx"
%}

%include ODESolverImplementation.hxx
namespace OT { %extend ODESolverImplementation { ODESolverImplementation(const ODESolverImplementation & other) { return new OT::ODESolverImplementation(other); } } }
