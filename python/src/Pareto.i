// SWIG file Pareto.i

%{
#include "openturns/Pareto.hxx"
%}

%include Pareto_doc.i

%include openturns/Pareto.hxx
namespace OT { %extend Pareto { Pareto(const Pareto & other) { return new OT::Pareto(other); } } }
