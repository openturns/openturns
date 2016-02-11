// SWIG file Laplace.i

%{
#include "openturns/Laplace.hxx"
%}

%include Laplace_doc.i

%include openturns/Laplace.hxx
namespace OT { %extend Laplace { Laplace(const Laplace & other) { return new OT::Laplace(other); } } }
