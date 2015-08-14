// SWIG file AbsoluteExponential.i

%{
#include "AbsoluteExponential.hxx"
%}

%include AbsoluteExponential.hxx
namespace OT { %extend AbsoluteExponential { AbsoluteExponential(const AbsoluteExponential & other) { return new OT::AbsoluteExponential(other); } } }
