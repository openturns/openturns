// SWIG file Rayleigh.i

%{
#include "openturns/Rayleigh.hxx"
%}

%include Rayleigh_doc.i

%include openturns/Rayleigh.hxx
namespace OT { %extend Rayleigh { Rayleigh(const Rayleigh & other) { return new OT::Rayleigh(other); } } }
