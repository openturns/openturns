// SWIG file InverseGamma.i

%{
#include "openturns/InverseGamma.hxx"
%}

%include InverseGamma_doc.i

%include openturns/InverseGamma.hxx
namespace OT { %extend InverseGamma { InverseGamma(const InverseGamma & other) { return new OT::InverseGamma(other); } } }
