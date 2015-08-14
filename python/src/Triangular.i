// SWIG file Triangular.i

%{
#include "Triangular.hxx"
%}

%include Triangular_doc.i

%include Triangular.hxx
namespace OT { %extend Triangular { Triangular(const Triangular & other) { return new OT::Triangular(other); } } }
