// SWIG file SolverImplementation.i

%{
#include "SolverImplementation.hxx"
%}

%include SolverImplementation.hxx
namespace OT { %extend SolverImplementation { SolverImplementation(const SolverImplementation & other) { return new OT::SolverImplementation(other); } } }
