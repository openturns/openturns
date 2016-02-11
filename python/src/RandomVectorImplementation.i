// SWIG file RandomVectorImplementation.i

%{
#include "openturns/RandomVectorImplementation.hxx"
%}

%include RandomVectorImplementation_doc.i

%include openturns/RandomVectorImplementation.hxx
namespace OT { %extend RandomVectorImplementation { RandomVectorImplementation(const RandomVectorImplementation & other) { return new OT::RandomVectorImplementation(other); } } }
