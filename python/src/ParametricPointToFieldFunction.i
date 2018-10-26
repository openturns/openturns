// SWIG file ParametricPointToFieldFunction.i

%{
#include "openturns/ParametricPointToFieldFunction.hxx"
%}

%include ParametricPointToFieldFunction_doc.i

%include openturns/ParametricPointToFieldFunction.hxx

namespace OT {
%extend ParametricPointToFieldFunction {

ParametricPointToFieldFunction(const ParametricPointToFieldFunction & other) { return new OT::ParametricPointToFieldFunction(other); }

}}

