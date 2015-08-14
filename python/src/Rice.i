// SWIG file Rice.i

%{
#include "Rice.hxx"
%}

%include Rice_doc.i

%include Rice.hxx
namespace OT { %extend Rice { Rice(const Rice & other) { return new OT::Rice(other); } } }
