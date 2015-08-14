// SWIG file Factorial.i

%{
#include "Factorial.hxx"
%}

%include Factorial_doc.i

%include Factorial.hxx
namespace OT { %extend Factorial { Factorial(const Factorial & other) { return new OT::Factorial(other); } } }
