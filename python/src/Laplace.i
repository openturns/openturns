// SWIG file Laplace.i

%{
#include "Laplace.hxx"
%}

%include Laplace_doc.i

%include Laplace.hxx
namespace OT { %extend Laplace { Laplace(const Laplace & other) { return new OT::Laplace(other); } } }
