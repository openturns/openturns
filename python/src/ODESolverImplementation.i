// SWIG file ODESolverImplementation.i

OTAutodoc(ODESolverImplementation)

%{
#include "openturns/ODESolverImplementation.hxx"
%}

%include openturns/ODESolverImplementation.hxx
namespace OT { %extend ODESolverImplementation { ODESolverImplementation(const ODESolverImplementation & other) { return new OT::ODESolverImplementation(other); } } }
