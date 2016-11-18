// SWIG file ODESolverImplementation.i

%{
#include "openturns/ODESolverImplementation.hxx"
%}

%include ODESolverImplementation_doc.i

%include openturns/ODESolverImplementation.hxx
namespace OT { %extend ODESolverImplementation { ODESolverImplementation(const ODESolverImplementation & other) { return new OT::ODESolverImplementation(other); } } }
