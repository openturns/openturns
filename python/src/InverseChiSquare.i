// SWIG file InverseChiSquare.i

%{
#include "openturns/InverseChiSquare.hxx"
%}

%include InverseChiSquare_doc.i

%include openturns/InverseChiSquare.hxx
namespace OT { %extend InverseChiSquare { InverseChiSquare(const InverseChiSquare & other) { return new OT::InverseChiSquare(other); } } }
