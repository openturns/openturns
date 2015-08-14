// SWIG file Secant.i

%{
#include "Secant.hxx"
%}

%include Secant.hxx
namespace OT { %extend Secant { Secant(const Secant & other) { return new OT::Secant(other); } } }
