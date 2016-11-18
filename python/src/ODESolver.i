// SWIG file ODESolver.i

%{
#include "openturns/ODESolver.hxx"
%}

%include ODESolver_doc.i

OTTypedInterfaceObjectHelper(ODESolver)

%include openturns/ODESolver.hxx
namespace OT { %extend ODESolver { ODESolver(const ODESolver & other) { return new OT::ODESolver(other); } } }
