// SWIG file Secant.i

%{
#include "openturns/Secant.hxx"
%}

%include Secant_doc.i

%include openturns/Secant.hxx
namespace OT { %extend Secant { Secant(const Secant & other) { return new OT::Secant(other); } } }
