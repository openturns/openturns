// SWIG file Null.i

%{
#include "Null.hxx"
%}

%include Null.hxx
namespace OT{ %extend Null { Null(const Null & other) { return new OT::Null(other); } } }
