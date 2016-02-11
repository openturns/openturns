// SWIG file Factorial.i

%{
#include "openturns/Factorial.hxx"
%}

%include Factorial_doc.i

%include openturns/Factorial.hxx
namespace OT { %extend Factorial { Factorial(const Factorial & other) { return new OT::Factorial(other); } } }
