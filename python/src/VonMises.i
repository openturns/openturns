// SWIG file VonMises.i

%{
#include "VonMises.hxx"
%}

%include VonMises_doc.i

%include VonMises.hxx
namespace OT { %extend VonMises { VonMises(const VonMises & other) { return new OT::VonMises(other); } } }
