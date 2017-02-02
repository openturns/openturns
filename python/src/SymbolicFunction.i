// SWIG file SymbolicFunction.i

%{
#include "openturns/SymbolicFunction.hxx"
%}

%include SymbolicFunction_doc.i

%ignore OT::SymbolicFunction::SymbolicFunction(const String & inputVariableName, const String & formula);

%include openturns/SymbolicFunction.hxx
namespace OT { %extend SymbolicFunction { SymbolicFunction(const SymbolicFunction & other) { return new OT::SymbolicFunction(other); } } }
