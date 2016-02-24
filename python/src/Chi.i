// SWIG file Chi.i

%{
#include "openturns/Chi.hxx"
%}

%include Chi_doc.i

%include openturns/Chi.hxx
namespace OT { %extend Chi { Chi(const Chi & other) { return new OT::Chi(other); } } }
