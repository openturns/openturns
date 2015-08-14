// SWIG file ChiSquare.i

%{
#include "ChiSquare.hxx"
%}

%include ChiSquare_doc.i

%include ChiSquare.hxx
namespace OT { %extend ChiSquare { ChiSquare(const ChiSquare & other) { return new OT::ChiSquare(other); } } }
