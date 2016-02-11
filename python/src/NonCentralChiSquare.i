// SWIG file NonCentralChiSquare.i

%{
#include "openturns/NonCentralChiSquare.hxx"
%}

%include NonCentralChiSquare_doc.i

%include openturns/NonCentralChiSquare.hxx
namespace OT { %extend NonCentralChiSquare { NonCentralChiSquare(const NonCentralChiSquare & other) { return new OT::NonCentralChiSquare(other); } } }
