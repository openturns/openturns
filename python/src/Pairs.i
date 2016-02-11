// SWIG file Pairs.i

%{
#include "openturns/Pairs.hxx"
%}

%include Pairs_doc.i

%include openturns/Pairs.hxx

namespace OT { %extend Pairs { Pairs(const Pairs & other) { return new OT::Pairs(other); } } }  

