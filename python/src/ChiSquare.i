// SWIG file ChiSquare.i

%{
#include "openturns/ChiSquare.hxx"
%}

%include ChiSquare_doc.i

%include openturns/ChiSquare.hxx
namespace OT { %extend ChiSquare { ChiSquare(const ChiSquare & other) { return new OT::ChiSquare(other); } } }
