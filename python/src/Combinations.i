// SWIG file Combinations.i

%{
#include "openturns/Combinations.hxx"
%}

%include Combinations_doc.i

%include openturns/Combinations.hxx
namespace OT { %extend Combinations { Combinations(const Combinations & other) { return new OT::Combinations(other); } } }
