// SWIG file ODESolver.i

OTAutodoc(ODESolver)

%{
#include "openturns/ODESolver.hxx"
%}

OTTypedInterfaceObjectHelper(ODESolver)

%include openturns/ODESolver.hxx
namespace OT { %extend ODESolver { ODESolver(const ODESolver & other) { return new OT::ODESolver(other); } } }
