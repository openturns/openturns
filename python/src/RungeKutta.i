// SWIG file RungeKutta.i

OTAutodoc(RungeKutta)

%{
#include "RungeKutta.hxx"
%}

%include RungeKutta.hxx
namespace OT { %extend RungeKutta { RungeKutta(const RungeKutta & other) { return new OT::RungeKutta(other); } } }
