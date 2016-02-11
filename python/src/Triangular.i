// SWIG file Triangular.i

%{
#include "openturns/Triangular.hxx"
%}

%include Triangular_doc.i

%include openturns/Triangular.hxx
namespace OT { %extend Triangular { Triangular(const Triangular & other) { return new OT::Triangular(other); } } }
