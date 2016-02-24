// SWIG file Hamming.i

%{
#include "openturns/Hamming.hxx"
%}

%include Hamming_doc.i

%include openturns/Hamming.hxx


namespace OT { %extend Hamming { Hamming(const Hamming & other) { return new OT::Hamming(other); } } }
