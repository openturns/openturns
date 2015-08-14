// SWIG file Beta.i

%{
#include "Beta.hxx"
%}

%include Beta_doc.i

%include Beta.hxx
namespace OT { %extend Beta { Beta(const Beta & other) { return new OT::Beta(other); } } }
