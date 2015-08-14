// SWIG file NonCentralChiSquare.i

%{
#include "NonCentralChiSquare.hxx"
%}

%include NonCentralChiSquare_doc.i

%include NonCentralChiSquare.hxx
namespace OT { %extend NonCentralChiSquare { NonCentralChiSquare(const NonCentralChiSquare & other) { return new OT::NonCentralChiSquare(other); } } }
