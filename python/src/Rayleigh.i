// SWIG file Rayleigh.i

%{
#include "Rayleigh.hxx"
%}

%include Rayleigh_doc.i

%include Rayleigh.hxx
namespace OT { %extend Rayleigh { Rayleigh(const Rayleigh & other) { return new OT::Rayleigh(other); } } }
