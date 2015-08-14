// SWIG file Tuples.i

%{
#include "Tuples.hxx"
%}

%include Tuples.hxx
namespace OT { %extend Tuples { Tuples(const Tuples & other) { return new OT::Tuples(other); } } }
