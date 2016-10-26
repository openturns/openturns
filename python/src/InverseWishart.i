// SWIG file InverseWishart.i

%{
#include "openturns/InverseWishart.hxx"
%}

%include InverseWishart_doc.i

%include openturns/InverseWishart.hxx
namespace OT { %extend InverseWishart { InverseWishart(const InverseWishart & other) { return new OT::InverseWishart(other); } } }
