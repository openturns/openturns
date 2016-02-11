// SWIG file RungeKutta.i

OTAutodoc(RungeKutta)

%{
#include "openturns/RungeKutta.hxx"
%}

%include openturns/RungeKutta.hxx
namespace OT { %extend RungeKutta { RungeKutta(const RungeKutta & other) { return new OT::RungeKutta(other); } } }
