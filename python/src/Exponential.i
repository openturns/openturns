// SWIG file Exponential.i

%{
#include "Exponential.hxx"
%}

%include Exponential_doc.i

%include Exponential.hxx
namespace OT { %extend Exponential { Exponential(const Exponential & other) { return new OT::Exponential(other); } } }
