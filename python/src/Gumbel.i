// SWIG file Gumbel.i

%{
#include "openturns/Gumbel.hxx"
%}

%include Gumbel_doc.i

%include openturns/Gumbel.hxx
namespace OT { %extend Gumbel { Gumbel(const Gumbel & other) { return new OT::Gumbel(other); } } }
