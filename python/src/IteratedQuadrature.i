// SWIG file IteratedQuadrature.i

%{
#include "openturns/IteratedQuadrature.hxx"
%}

%include IteratedQuadrature_doc.i

%include openturns/IteratedQuadrature.hxx
namespace OT { %extend IteratedQuadrature { IteratedQuadrature(const IteratedQuadrature & other) { return new OT::IteratedQuadrature(other); } } }
