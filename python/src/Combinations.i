// SWIG file Combinations.i

%{
#include "Combinations.hxx"
%}

%include Combinations_doc.i

%include Combinations.hxx
namespace OT { %extend Combinations { Combinations(const Combinations & other) { return new OT::Combinations(other); } } }
