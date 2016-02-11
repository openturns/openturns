// SWIG file NormalGamma.i

%{
#include "openturns/NormalGamma.hxx"
%}

%include NormalGamma_doc.i

%include openturns/NormalGamma.hxx
namespace OT { %extend NormalGamma { NormalGamma(const NormalGamma & other) { return new OT::NormalGamma(other); } } }
