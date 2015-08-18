// SWIG file Solver.i

%{
#include "Solver.hxx"
%}

%include Solver_doc.i

OTTypedInterfaceObjectHelper(Solver)

%include Solver.hxx
namespace OT { %extend Solver { Solver(const Solver & other) { return new OT::Solver(other); } } }
