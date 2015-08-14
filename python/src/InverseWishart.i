// SWIG file InverseWishart.i

%{
#include "InverseWishart.hxx"
%}

%include InverseWishart.hxx
namespace OT { %extend InverseWishart { InverseWishart(const InverseWishart & other) { return new OT::InverseWishart(other); } } }
