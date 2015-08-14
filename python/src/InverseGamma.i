// SWIG file InverseGamma.i

%{
#include "InverseGamma.hxx"
%}

%include InverseGamma_doc.i

%include InverseGamma.hxx
namespace OT { %extend InverseGamma { InverseGamma(const InverseGamma & other) { return new OT::InverseGamma(other); } } }
