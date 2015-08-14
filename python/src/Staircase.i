// SWIG file Staircase.i

%{
#include "Staircase.hxx"
%}

%include Staircase_doc.i

%include Staircase.hxx
namespace OT { %extend Staircase { Staircase(const Staircase & other) { return new OT::Staircase(other); } } }
