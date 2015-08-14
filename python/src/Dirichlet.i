// SWIG file Dirichlet.i

%{
#include "Dirichlet.hxx"
%}

%include Dirichlet_doc.i

%include Dirichlet.hxx
namespace OT { %extend Dirichlet { Dirichlet(const Dirichlet & other) { return new OT::Dirichlet(other); } } }
