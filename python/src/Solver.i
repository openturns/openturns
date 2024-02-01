// SWIG file Solver.i

%{
#include "openturns/Solver.hxx"
%}

%include Solver_doc.i

OTTypedInterfaceObjectHelper(Solver)

%copyctor OT::Solver;

%include openturns/Solver.hxx
