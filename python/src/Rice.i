// SWIG file Rice.i

%{
#include "openturns/Rice.hxx"
%}

%include Rice_doc.i

%include openturns/Rice.hxx
namespace OT { %extend Rice { Rice(const Rice & other) { return new OT::Rice(other); } } }
