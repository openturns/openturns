// SWIG file AbsoluteExponential.i

%{
#include "openturns/AbsoluteExponential.hxx"
%}

%include AbsoluteExponential_doc.i

%include openturns/AbsoluteExponential.hxx
namespace OT { %extend AbsoluteExponential { AbsoluteExponential(const AbsoluteExponential & other) { return new OT::AbsoluteExponential(other); } } }
