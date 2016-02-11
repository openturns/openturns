// SWIG file VonMises.i

%{
#include "openturns/VonMises.hxx"
%}

%include VonMises_doc.i

%include openturns/VonMises.hxx
namespace OT { %extend VonMises { VonMises(const VonMises & other) { return new OT::VonMises(other); } } }
