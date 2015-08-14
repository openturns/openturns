// SWIG file RandomVectorImplementation.i

%{
#include "RandomVectorImplementation.hxx"
%}

%include RandomVectorImplementation_doc.i

%include RandomVectorImplementation.hxx
namespace OT { %extend RandomVectorImplementation { RandomVectorImplementation(const RandomVectorImplementation & other) { return new OT::RandomVectorImplementation(other); } } }
