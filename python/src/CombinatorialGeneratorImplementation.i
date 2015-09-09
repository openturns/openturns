// SWIG file CombinatorialGeneratorImplementation.i

%{
#include "CombinatorialGeneratorImplementation.hxx"
%}

%include CombinatorialGeneratorImplementation_doc.i

%include CombinatorialGeneratorImplementation.hxx
namespace OT { %extend CombinatorialGeneratorImplementation { CombinatorialGeneratorImplementation(const CombinatorialGeneratorImplementation & other) { return new OT::CombinatorialGeneratorImplementation(other); } } }
