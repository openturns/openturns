// SWIG file Dirichlet.i

%{
#include "openturns/Dirichlet.hxx"
%}

%include Dirichlet_doc.i

%include openturns/Dirichlet.hxx
namespace OT { %extend Dirichlet { Dirichlet(const Dirichlet & other) { return new OT::Dirichlet(other); } } }
