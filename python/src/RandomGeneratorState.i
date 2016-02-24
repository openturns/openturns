// SWIG file RandomGeneratorState.i

%{
#include "openturns/RandomGeneratorState.hxx"
%}

%include openturns/RandomGeneratorState.hxx


namespace OT {
%extend RandomGeneratorState {
  RandomGeneratorState(const RandomGeneratorState & other) { return new OT::RandomGeneratorState(other); }
  Bool __eq__(const RandomGeneratorState & other) { return (*self) == other; }

} //RandomGeneratorState
} //OT