// SWIG file Beta.i

%{
#include "openturns/Beta.hxx"
%}

%include Beta_doc.i

%include openturns/Beta.hxx
namespace OT { %extend Beta { Beta(const Beta & other) { return new OT::Beta(other); } } }
