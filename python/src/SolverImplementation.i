// SWIG file SolverImplementation.i

%{
#include "openturns/SolverImplementation.hxx"
%}

%include SolverImplementation_doc.i

%include openturns/SolverImplementation.hxx
namespace OT { %extend SolverImplementation { SolverImplementation(const SolverImplementation & other) { return new OT::SolverImplementation(other); } } }
