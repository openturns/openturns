// SWIG file

%{
#include "openturns/MonteCarloLHS.hxx"
%}

%include MonteCarloLHS_doc.i

%include openturns/MonteCarloLHS.hxx
namespace OT { %extend MonteCarloLHS { MonteCarloLHS(const MonteCarloLHS & other) { return new OT::MonteCarloLHS(other); } } }
