// SWIG file Pairs.i

%{
#include "Pairs.hxx"
%}

%include Pairs_doc.i

%include Pairs.hxx

namespace OT { %extend Pairs { Pairs(const Pairs & other) { return new OT::Pairs(other); } } }  

