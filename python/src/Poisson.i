// SWIG file Poisson.i

%{
#include "openturns/Poisson.hxx"
%}

%include Poisson_doc.i

%include openturns/Poisson.hxx
namespace OT { %extend Poisson { Poisson(const Poisson & other) { return new OT::Poisson(other); } } }
