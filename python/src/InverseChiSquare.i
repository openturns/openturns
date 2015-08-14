// SWIG file InverseChiSquare.i

%{
#include "InverseChiSquare.hxx"
%}

%include InverseChiSquare_doc.i

%include InverseChiSquare.hxx
namespace OT { %extend InverseChiSquare { InverseChiSquare(const InverseChiSquare & other) { return new OT::InverseChiSquare(other); } } }
