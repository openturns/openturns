// SWIG file Solver.i

%{
#include "Solver.hxx"
%}

OTTypedInterfaceObjectHelper(Solver)

%include Solver.hxx
namespace OT { %extend Solver { Solver(const Solver & other) { return new OT::Solver(other); } } }
