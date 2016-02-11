// SWIG file InverseWishart.i

%{
#include "openturns/InverseWishart.hxx"
%}

%include openturns/InverseWishart.hxx
namespace OT { %extend InverseWishart { InverseWishart(const InverseWishart & other) { return new OT::InverseWishart(other); } } }
