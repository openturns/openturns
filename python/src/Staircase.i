// SWIG file Staircase.i

%{
#include "openturns/Staircase.hxx"
%}

%include Staircase_doc.i

%include openturns/Staircase.hxx
namespace OT { %extend Staircase { Staircase(const Staircase & other) { return new OT::Staircase(other); } } }
