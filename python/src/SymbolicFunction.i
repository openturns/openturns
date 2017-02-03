// SWIG file SymbolicFunction.i

%{
#include "openturns/SymbolicFunction.hxx"
%}

%include SymbolicFunction_doc.i

%include openturns/SymbolicFunction.hxx
namespace OT { %extend SymbolicFunction { SymbolicFunction(const SymbolicFunction & other) { return new OT::SymbolicFunction(other); } } }
