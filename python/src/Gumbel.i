// SWIG file Gumbel.i

%{
#include "Gumbel.hxx"
%}

%include Gumbel_doc.i

%include Gumbel.hxx
namespace OT { %extend Gumbel { Gumbel(const Gumbel & other) { return new OT::Gumbel(other); } } }
