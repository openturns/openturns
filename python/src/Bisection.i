// SWIG file Bisection.i

%{
#include "openturns/Bisection.hxx"
%}

%include Bisection_doc.i

%include openturns/Bisection.hxx
namespace OT { %extend Bisection { Bisection(const Bisection & other) { return new OT::Bisection(other); } } }
