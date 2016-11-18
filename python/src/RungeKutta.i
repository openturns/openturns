// SWIG file RungeKutta.i

%{
#include "openturns/RungeKutta.hxx"
%}

%include RungeKutta_doc.i

%include openturns/RungeKutta.hxx
namespace OT { %extend RungeKutta { RungeKutta(const RungeKutta & other) { return new OT::RungeKutta(other); } } }
