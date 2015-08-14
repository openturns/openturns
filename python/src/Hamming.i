// SWIG file Hamming.i

%{
#include "Hamming.hxx"
%}

%include Hamming_doc.i

%include Hamming.hxx


namespace OT { %extend Hamming { Hamming(const Hamming & other) { return new OT::Hamming(other); } } }
