// SWIG file Compact.i

%{
#include "Compact.hxx"
%}

%include Compact.hxx
namespace OT{ %extend Compact { Compact(const Compact & other) { return new OT::Compact(other); } } }
