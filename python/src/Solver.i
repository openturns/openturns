// SWIG file Solver.i

%{
#include "openturns/Solver.hxx"
%}

%include Solver_doc.i

OTTypedInterfaceObjectHelper(Solver)

%include openturns/Solver.hxx
namespace OT { %extend Solver { Solver(const Solver & other) { return new OT::Solver(other); } } }
