// SWIG file Bisection.i

%{
#include "Bisection.hxx"
%}

%include Bisection.hxx
namespace OT { %extend Bisection { Bisection(const Bisection & other) { return new OT::Bisection(other); } } }
