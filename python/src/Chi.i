// SWIG file Chi.i

%{
#include "Chi.hxx"
%}

%include Chi_doc.i

%include Chi.hxx
namespace OT { %extend Chi { Chi(const Chi & other) { return new OT::Chi(other); } } }
