// SWIG file LinearGradient.i

%{
#include "openturns/LinearGradient.hxx"
%}

%include LinearGradient_doc.i

%include openturns/LinearGradient.hxx
namespace OT { %extend LinearGradient { LinearGradient(const LinearGradient & other) { return new OT::LinearGradient(other); } } }
