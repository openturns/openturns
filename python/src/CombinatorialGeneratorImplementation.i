// SWIG file CombinatorialGeneratorImplementation.i

%{
#include "openturns/CombinatorialGeneratorImplementation.hxx"
%}

%include CombinatorialGeneratorImplementation_doc.i

%include openturns/CombinatorialGeneratorImplementation.hxx
namespace OT { %extend CombinatorialGeneratorImplementation { CombinatorialGeneratorImplementation(const CombinatorialGeneratorImplementation & other) { return new OT::CombinatorialGeneratorImplementation(other); } } }
