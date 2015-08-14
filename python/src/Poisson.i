// SWIG file Poisson.i

%{
#include "Poisson.hxx"
%}

%include Poisson_doc.i

%include Poisson.hxx
namespace OT { %extend Poisson { Poisson(const Poisson & other) { return new OT::Poisson(other); } } }
