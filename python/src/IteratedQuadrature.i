// SWIG file IteratedQuadrature.i

%{
#include "IteratedQuadrature.hxx"
%}

%include IteratedQuadrature_doc.i

%include IteratedQuadrature.hxx
namespace OT { %extend IteratedQuadrature { IteratedQuadrature(const IteratedQuadrature & other) { return new OT::IteratedQuadrature(other); } } }
