// SWIG file NormalGamma.i

%{
#include "NormalGamma.hxx"
%}

%include NormalGamma_doc.i

%include NormalGamma.hxx
namespace OT { %extend NormalGamma { NormalGamma(const NormalGamma & other) { return new OT::NormalGamma(other); } } }
