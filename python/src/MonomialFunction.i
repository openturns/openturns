// SWIG file MonomialFunction.i

%{
#include "openturns/MonomialFunction.hxx"
%}

%include MonomialFunction_doc.i

%include openturns/MonomialFunction.hxx
namespace OT { %extend MonomialFunction { MonomialFunction(const MonomialFunction & other) { return new OT::MonomialFunction(other); } } }
