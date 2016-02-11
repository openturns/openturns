// SWIG file Exponential.i

%{
#include "openturns/Exponential.hxx"
%}

%include Exponential_doc.i

%include openturns/Exponential.hxx
namespace OT { %extend Exponential { Exponential(const Exponential & other) { return new OT::Exponential(other); } } }
